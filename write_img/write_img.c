#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

FILE* of = NULL;
FILE* bs = NULL;

const char* of_path = NULL;
const char* bs_path = NULL;

typedef
struct __attribute__((packed)) DFSFileHeader {
	unsigned long sectors, next_lba;
	char name[32];
} DFSFileHeader;

// 8B Sectors
// 8B Next LBA
// 32B Filename

unsigned long current_lba = 1;

void write_file(unsigned long len, const char* data, char* file_name) {
	char header_buf[512];
	memset(header_buf, 0, 512);

	unsigned long sector_count = (len / 512) + 1;
	unsigned long sector_bytes = sector_count * 512;

	if (current_lba)
		current_lba++;

	DFSFileHeader* fh = (DFSFileHeader*)header_buf;
	fh->sectors = sector_count;
	fh->next_lba = current_lba ? current_lba += sector_count : 0;
	strncpy(fh->name, file_name, 32);

	fwrite(header_buf, 1, 512, of);

	unsigned long written_bytes = fwrite(data, 1, len, of);
	unsigned long align_bytes = sector_bytes - written_bytes;

	for (unsigned long i = 0; i < align_bytes; ++i)
		fputc(0, of);
}

size_t fsize(FILE* fp) {
	fseek(fp, 0, SEEK_END);
	size_t sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return sz;
}

int main(int argc, char** argv) {
	for (int i = 1; i < argc; ++i) {
		char* arg = argv[i];
		if (arg[0] != '-')
			continue;

		switch (arg[1]) {
		case 'b':
			bs_path = &arg[2];
			break;

		case 'o':
			of_path = &arg[2];
			break;

		default:
			printf("Unknown argument: -%c\n", arg[1]);
			return 1;
		}
	}

	if (!of_path)
		of_path = "out.img";

	of = fopen(of_path, "w");
	if (!of) {
		printf("Failed to open '%s'\n", of_path);
		return 1;
	}


	char bs_data[512];
	memset(bs_data, 0, 512);
	if (bs_path) {
		bs = fopen(bs_path, "r");
		if (!bs) {
			printf("Failed to open '%s'\n", bs_path);
			return 1;
		}

		size_t sz = fsize(bs);

		if (sz > 510) {
			printf("Bootsector exceeded 510-byte limit\n");
			return 1;
		}

		fread(bs_data, 1, sz, bs);
		bs_data[510] = 0x55;
		bs_data[511] = 0xAA;


		fclose(bs);
	}
	fwrite(bs_data, 1, 512, of);

	for (int i = 1; i < argc; ++i) {
		char* arg = argv[i];
		if (arg[0] == '-')
			continue;

		FILE* fp = fopen(arg, "r");
		if (!fp) {
			printf("Failed to open '%s'\n", arg);
			return 1;
		}

		size_t sz = fsize(fp);
		char* data = malloc(sz);

		fread(data, 1, sz, fp);

		if (i + 1 == argc)
			current_lba = 0;
		write_file(sz, data, basename(arg));

		fclose(fp);
		free(data);
	}

	for (int i = 0; i < 2048; ++i) {
		memset(bs_data, 0, 512);
		fwrite(bs_data, 1, 512, of);
	}

	fclose(of);

	return 0;
}

