#ifndef PC_SPEAKER_H
#define PC_SPEAKER_H

#define PCSPK_CMD 0x61

#define PCSPK_CONNECT_PIT2 0b11

void pcspk_connect_pit(void);
void pcspk_disconnect_pit(void);

#endif
