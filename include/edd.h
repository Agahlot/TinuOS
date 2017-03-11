#ifndef LINUX_EDD_H
#define LINUX_EDD_H

#include "types.h"
#include "cpu_feature.h"

#define EDDMAXNR	6
#define EDDMAGIC1	0x55AA
#define EDDMAGIC2	0xAA55

struct edd_device_params {
		__u8 length;
		__u8 info_flags;
		__u16 number_of_cylinders;
		__u16 number_of_heads;
		__u16 number_of_sectors;
		__u32 absolute_number_of_sectors;
		__u8 bytes_per_sector;
		__u16 optional_params;
		__u32 dpte_ptr;
		__u16 key;	/* 0BEDDh presence of device path information */
		__u8 length_key;
		__u8 reserved0;
		__u16 reserved1;
		__u8 host_bus_type[4];
		__u8 interface_type[8];
		union {
			struct {
				__u16 base_address;
				__u16 reserved0;
				__u32 reserved1;
			} __attribute__((packed)) isa;
			struct {
				__u8 bus_number;
				__u8 slot_number;
				__u8 function_number;
				__u8 channel_number;
				__u32 reserved0;
			} __attribute__((packed)) pci;
			/* PCI-X is the same as PCI */
			struct {
				__u64 reserved0;
			} __attribute__((packed)) infiband;
			struct {
				__u64 reserved0;
			} __attribute__((packed)) htpt;
			struct {
				__u64 reserved0;
			} __attribute__((packed)) unknown;
		} interface_path;

		union {
			struct {
				__u8 device;
				__u8 reserved0;
				__u16 reserved1;
				__u32 reserved3;
				__u64 reserved4;
			} __attribute__((packed)) ata;
			struct {
				__u8 device;
				__u8 logical_unit_number;
				__u8 reserved0;
				__u8 reserved1;
				__u32 reserved2;
				__u64 reserved3; 
			} __attribute__((packed)) atapi;
			struct {
				__u16 unit_number;
				__u64 logical_unit_number;
				__u16 reserved0;
				__u32 reserved2;
			} __attribute__((packed)) scsi;
			struct {
				__u64 serial_number;
				__u64 reserved0;
			} __attribute__((packed));
			struct {
				__u64 eui;
				__u64 reserved0;
			} __attribute__((packed)) i1394;
			struct {
				__u64 wwid;
				__u64 logical_unit_number;
			} __attribute__((packed)) fibre;
			struct {
				__u64 identity_tag;
				__u64 reserved0;
			} __attribute__((packed)) i2o;
			struct {
				__u32 array_number;
				__u32 reserved0;
				__u64 reserved1;
			} __attribute__((packed)) raid;
			struct {
				__u8 port_number;
				__u8 port_multiplier_number;
				__u16 reserved0;
				__u32 reserved1;
				__u64 reserved2;
			} __attribute__((packed)) sata;
			struct {
				__u64 sas_address;
				__u64 logical_unit_number;
			} __attribute__((packed)) sas;
			struct {
				__u64 reserved0;
				__u64 reserved2;
			} __attribute__((packed)) unknown;
		} device_path;
		__u16 reserved0;
		__u8 checksum;
} __attribute__((packed));

struct edd_info {
		__u8 version;
		__u16 physical_number_of_cylinders;
		__u8 physical_number_of_heads;
		__u8 sectors_per_track;
		struct edd_device_params params;
} __attribute__((packed));