/*
KurzCdRead 
read and decode media in native Kurzweil format

main.cpp - simple main file as a reference how to use the KMSI object


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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.h"
#include "ausgabe.h"
#include "direntry.h"
#include "kmsi_direct.h"
#include "sensecodes.h"
#include "virtualdevice.h"

class StandardAusgabe : public Ausgabe {
public:
    virtual void TextAusgabe(char *s) {
		printf("%s\n",s);
	}
};


int main (int argc, char *argv[]){
	VirtualDevice *vdev;
	KMSI_Direct *kmsi;
	StandardAusgabe textout;

	if (argc>1) vdev=new VirtualDevice(argv[1]);
	else {
	  printf ("Usage: kcdread ""filename""\n");
	  return 1;
	}

	kmsi=new KMSI_Direct(vdev);
	kmsi->setAusgabe(&textout);

	if (kmsi->init()) return 1;
	kmsi->readRootDir();
	kmsi->deinit();
	return 0;
}
