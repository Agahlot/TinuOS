#ifndef APM_BIOS_H
#define APM_BIOS_H


#include "types.h"

struct apm_bios_info {
		__u16	version;
		__u16	cseg;
		__u32	offset;
		__u16	cseg_16;
		__u16	dseg;
		__u16	flags;
		__u16	cseg_len;
		__u16	cseg_16_len;
		__u16	dseg_len;
};