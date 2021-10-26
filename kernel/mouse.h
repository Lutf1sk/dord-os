#ifndef MOUSE_H
#define MOUSE_H 1

#include "common.h"

// Commands
#define MS_CMD_RESET			0xFF
#define MS_CMD_RESEND			0xFE
#define MS_CMD_SET_DEFAULTS		0xF6
#define MS_CMD_DISABLE_DREP		0xF5
#define MS_CMD_ENABLE_DREP		0xF4
#define MS_CMD_SET_SAMPLE_RATE	0xF3
#define MS_CMD_GET_DEVICE_ID	0xF2
#define MS_CMD_REMOTE			0xF0
#define MS_CMD_WRAP				0xEE
#define MS_CMD_RESET_WRAP		0xEC
#define MS_CMD_READ_PACKET		0xEB
#define MS_CMD_STREAM			0xEA
#define MS_CMD_GET_STATUS		0xE9
#define MS_CMD_SET_RESOLUTION	0xE8
#define MS_CMD_SET_SCALING		0xE6

// Common packet bits
#define MS_CPKT_LMB		0x01
#define MS_CPKT_RMB		0x02
#define MS_CPKT_MMB		0x04
#define MS_CPKT_XSIGN	0x10
#define MS_CPKT_YSIGN	0x20
#define MS_CPKT_XOVER	0x40
#define MS_CPKT_YOVER	0x80

// 5 Button packet bits
#define MS_5PKT_ZM	0x0F
#define MS_5PKT_4MB	0x10
#define MS_5PKT_5MB	0x20

// Buttons
#define MS_BTN_1 0
#define MS_BTN_2 1
#define MS_BTN_3 2
#define MS_BTN_4 3
#define MS_BTN_5 4
#define MS_BTN_LEFT		MS_BTN_1
#define MS_BTN_RIGHT	MS_BTN_2
#define MS_BTN_MIDDLE	MS_BTN_3

void mouse_initialize(u8 port);

void mouse_handle_interrupt(void);

#endif
