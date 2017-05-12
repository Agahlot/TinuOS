#include "ide.h"
#include <boot.h>

struct IDEChannelRegisters {
	unsigned short base;	// I/O base
	unsigned short ctrl;	// Control Base
	unsigned short bmide;	// Bus Master IDE
	unsigned char nIEN;		// nIEN (No Interrupt)
} channels[2];

