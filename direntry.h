/*
KurzCdRead 
read and decode media in native Kurzweil format

direntry.h - very simple class that capsules directory entries


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

#ifndef __DIRENTRY_H
#define __DIRENTRY_H

typedef unsigned short fatEntry;
//ports to big endian machines need another type here

#define ATR_DIRECTORY 16
#define ATR_LABEL 8
#define MSDOS_DIRENTRY_SIZE 32

class MsDosDirEntry {
	unsigned char *data;
	char *copyName;
public:
	MsDosDirEntry() {data=NULL; copyName=NULL;};
	MsDosDirEntry(char *c) {setData(c); copyName=NULL;};
	~MsDosDirEntry();
	void next();
	void setData(char *c) {data=(unsigned char *)c;};
	char* getName();
	char* getExt();
	fatEntry getStartCluster();
	unsigned long getSize();
	bool isDirectory() {
		return ((data[11]&ATR_DIRECTORY)!=0) ? true : false;
	};
	bool isLabel() {
		return ((data[11]&ATR_LABEL)!=0) ? true : false;
	};
	bool isUsed() {return (data[0]!=0) ? true : false;};
	bool isDeleted() {return (data[0]==0xe5) ? true : false;};
	bool startsWithDot() {return (data[0]=='.') ? true : false;};
	char* getCopyName();
};

#endif