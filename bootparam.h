#include "types.h"

#define E820MAX 128
#define E820_X_MAX E820MAX

typedef u8 __u8;
typedef u16 __u16;
typedef u32 __u32;
typedef u64 __u64;

/* loadflags */
#define LOADED_HIGH		(1 << 0)
#define CAN_USE_HEAP	(1 << 7)

/* xloadflags */
#define XLF_KERNEL_64				(1 << 0)
#define XLF_CAN_BE_LOADED_ABOVE_4G	(1 << 1)

struct e820entry {
	__u64 addr;
	__u64 size;
	__u32 type;
}__attribute__((packed));

struct e820map {
	__u32 nr_map;
	struct e820entry map[E820_X_MAX];
};

struct setup_header {
	__u8	setup_sects;
	__u16	root_flags;
	__u32	syssize;
	__u16	ram_size;
	__u16	vid_mode;
	__u16	root_dev;
	__u16	boot_flag;
	__u16	jump;
	__u32	header;
	__u16	version;
	__u32	realmode_swtch;
	__u16	start_sys;
	__u16	kernel_version;
	__u8	type_of_loader;
	__u8	loadflags;
	__u16	setup_move_size;
	__u32	code32_start;
	__u32	ramdisk_image;
	__u32	ramdisk_size;
	__u32	bootsect_kludge;
	__u16	heap_end_ptr;
	__u8	ext_loader_ver;
	__u8	ext_loader_type;
	__u32	cmd_line_ptr;
	__u32	initrd_addr_max;
	__u32	kernel_alignment;
	__u8	relocatable_kernel;
	__u8	min_alignment;
	__u16	xloadflags;
	__u32	cmdline_size;
	__u32	hardware_subarch;
	__u64	hardware_subarch_data;
	__u32	payload_offset;
	__u32	payload_length;
	__u64	setup_data;
	__u64	pref_address;
	__u32	init_size;
	__u32	handover_offset;
} __attribute__((packed));

/* So called zero-page */
struct boot_params {
		__u32 ext_ramdisk_image;
		__u32 ext_ramdisk_size;
		__u32 ext_cmd_line_ptr;

		__u8 kbd_status;	/* keyboard status */

		__u8 e820_entries;
		struct e820entry e820_map[E820MAX];

		struct setup_header hdr;
} __attribute__((packed));