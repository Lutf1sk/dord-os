#ifndef PS2_H
#define PS2_H 1

#include <common.h>

#define PS2_ACK 0xFA

// IO Ports
#define PS2_CMD		0x64
#define PS2_STATUS	0x64
#define PS2_DATA	0x60

// Status bits
#define PS2_SR_OUT		0x01
#define PS2_SR_INP		0x02
#define PS2_SR_SYS		0x04
#define PS2_SR_CMDT		0x08
#define PS2_SR_MOUSE	0x20

// Commands
#define PS2_CMD_READ	0x20
#define PS2_CMD_WRITE	0x60

#define PS2_CMD_DISABLE_P2	0xA7
#define PS2_CMD_ENABLE_P2	0xA8
#define PS2_CMD_TEST_P2		0xA9

#define PS2_CMD_TEST_CTRL	0xAA

#define PS2_CMD_TEST_P1		0xAB
#define PS2_CMD_DISABLE_P1	0xAD
#define PS2_CMD_ENABLE_P1	0xAE

#define PS2_CMD_READ_CTRL_OUT	0xD0
#define PS2_CMD_WRITE_CTRL_OUT	0xD1

#define PS2_CMD_WRITE_P1_OUT	0xD2
#define PS2_CMD_WRITE_P2_OUT	0xD3
#define PS2_CMD_WRITE_P2_INP	0xD4

// Device commands
#define PS2_DCMD_RESET				0xFF
#define PS2_DCMD_DISABLE_SCANNING	0xF5
#define PS2_DCMD_IDENTIFY 			0xF2

// Config bits
#define PS2_CFG_P1_IRQ				0x01
#define PS2_CFG_P2_IRQ				0x02
#define PS2_CFG_SYS					0x04
#define PS2_CFG_DISABLE_P1_CLOCK	0x10
#define PS2_CFG_DISABLE_P2_CLOCK	0x20
#define PS2_CFG_P1_TRANSLATION		0x40

// Controller output port
#define PS2_CTRL_RESET			0x01
#define PS2_CTRL_A20			0x02
#define PS2_CTRL_P2_CLOCK		0x04
#define PS2_CTRL_P2_DATA		0x08
#define PS2_CTRL_P1_WRITTEN		0x10
#define PS2_CTRL_P2_WRITTEN		0x20
#define PS2_CTRL_P1_CLOCK		0x40
#define PS2_CTRL_P1_DATA		0x80

// Device types
#define PS2_DEV_NONE 0xFFFF
#define PS2_DEV_GENERIC_MOUSE 0x00
#define PS2_DEV_ZAXIS_MOUSE 0x03
#define PS2_DEV_5BTN_MOUSE 0x04
#define PS2_DEV_MF2_KEYBOARD 0xAB83

// PS/2 Ports
#define PS2_PORT1 0
#define PS2_PORT2 1

extern u16 ps2_p1, ps2_p2;

i16 ps2_send_cmd(u8 cmd);
i16 ps2_send_data(u8 data);
i16 ps2_recv(void);

i16 ps2_send_port(u8 port, u8 data);

char* ps2_dev_str(u16 type);

void ps2_disable_port(u8 port);
void ps2_enable_port(u8 port);

void ps2_enable_irqs(void);

void ps2_initialize(void);

#endif
