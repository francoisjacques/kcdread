/*
KurzCdRead 
read and decode media in native Kurzweil format

kmsi_direct.h - class for direct reading of media in native 
   Kurzweil format. KMSI stands for Kurzweil Music System Inc. (I guess)


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

#include "direntry.h"

class KMSI_Direct {
protected:
	int sektor_size;
	int cluster_size;
	int max_direntries_per_cluster;

	int fat_start;
	int fat_size;
	int root_start;
	int root_size;
	
	int cluster_base;

	fatEntry *fat;

	Ausgabe *ausgabe;
	void displayErrors();

	Device* device;
	char*  readClusters(int start, int count);
	virtual int readMBR(char * sektor);
	virtual int readBootSector(long startpos);
	virtual int readFat();

	int cluster2sektor (int c) {return cluster_base+c*cluster_size;};
	int sektor2byte (int s) {return s*sektor_size;};
	int cluster2byte (int c) {return sektor2byte(cluster2sektor(c));};
	fatEntry next_cluster(fatEntry cl) {return fat[cl];};
	virtual int copyFile (MsDosDirEntry *e);
	virtual int copyDirectory (MsDosDirEntry *e);
	virtual int readDirectory (MsDosDirEntry *e);

public:
	KMSI_Direct(Device *d) {
		device=d;
		ausgabe=new Ausgabe();
	}
	~KMSI_Direct() {
		if (fat!=NULL) free (fat);
	}
	void setAusgabe (Ausgabe *aus);
	virtual int init();
	virtual int deinit();
	virtual int readRootDir();
};
