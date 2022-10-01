#include <ppm.h>
#include <io.h>

static
u8 isspace(u8 c) {
	switch (c) {
	case ' ': case '\t': case '\r': case '\n': return 1;
	default: return 0;
	}
}

static INLINE
u8 isdigit(u8 c) {
	return (c >= '0') && (c <= '9');
}

static
u8 consume_whitespace(u8** it, u8* end) {
	u8 success = 0;
	while (*it < end) {
		if (isspace(**it))
			++*it;
		else if (**it == '#')
			while (*it < end && **it != '\n')
				++*it;
		else
			break;
		success = 1;
	}
	return success;
}

static
usz consume_value(u8** it, u8* end) {
	u8* start = *it;
	while (*it < end) {
		if (!isdigit(**it))
			return *it - start;
		++*it;
	}
	return *it - start;
}

static
u32 parse_uint32(u8* str, usz len) {
	u32 val = 0;

	usz i = 0;
	while (i < len) {
		val *= 10;
		val += str[i++] - '0';
	}

	return val;
}

err_t ppm_load(void* data, usz len, img_t* out_img, u32* out_data) {
	u8* it = data, *end = it + len;

	if (it + sizeof(u16) > end || *(u16*)it != PPM_MAGIC)
		return ERR_FMTINVAL;
	it += sizeof(u16);
	if (!consume_whitespace(&it, end))
		return ERR_FMTINVAL;

	u8* width_str = it;
	usz width_len = consume_value(&it, end);
	if (!width_len)
		return ERR_FMTINVAL;
	if (!consume_whitespace(&it, end))
		return ERR_FMTINVAL;

	u8* height_str = it;
	usz height_len = consume_value(&it, end);
	if (!height_len)
		return ERR_FMTINVAL;
	if (!consume_whitespace(&it, end))
		return ERR_FMTINVAL;

	u8* clrmax_str = it;
	usz clrmax_len = consume_value(&it, end);
	if (!clrmax_len)
		return ERR_FMTINVAL;
	if (!consume_whitespace(&it, end))
		return ERR_FMTINVAL;

	u32 w = parse_uint32(width_str, width_len);
	u32 h = parse_uint32(height_str, height_len);
	u32 clrmax = parse_uint32(clrmax_str, clrmax_len);

	if (clrmax != 255)
		return ERR_FMTINVAL;

	usz px_count = w * h;
	if (it + px_count * 3 > end)
		return ERR_FMTINVAL;

	out_img->width = w;
	out_img->height = h;
	out_img->px_count = px_count;
	out_img->px_data = out_data;

	if (!out_data)
		return OK;

	for (usz i = 0; i < px_count; ++i) {
		u8 r = *it++;
		u8 g = *it++;
		u8 b = *it++;
		out_data[i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
	}

	return OK;
}

