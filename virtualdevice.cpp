/*
KurzCdRead 
read and decode media in native Kurzweil format

virtualdevice.cpp - a simple implementation of the Device class.
	You can write an interactive version of this class by overriding
	the Reserve() and Release() methods.
   

Copyright (c) 2002 Marc Halbruegge
  
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


Contact the author:
Marc Halbruegge
Auf Torf 21
49328 Melle
Germany
eMail: mhalbrue@uni-osnabrueck.de

*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "device.h"
#include "virtualdevice.h"

VirtualDevice::VirtualDevice (char *s) {
	name = strdup("file");
	vendor = strdup("virtual device");
	f = fopen (s, "rb");
}

VirtualDevice::~VirtualDevice() {
	if (f!=NULL) fclose(f);
	free (name);
	free (vendor);
}

int VirtualDevice::getBlockCount() {
	return 0;
}

char* VirtualDevice::ReadSector(int s) {
	return ReadSector(s,1);
}

char* VirtualDevice::ReadSector(int s, int c) {
	char *buf;
	if (fseek(f, s*VIRTUAL_BLOCKSIZE, SEEK_SET)) return NULL;
	if ((buf=(char*)malloc(VIRTUAL_BLOCKSIZE*c))==NULL) return NULL;
	fread (buf, VIRTUAL_BLOCKSIZE, c, f);
	return buf;
}
