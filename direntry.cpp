/*
KurzCdRead 
read and decode media in native Kurzweil format

direntry.cpp - implements the direntry class


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
#include <string.h>
#include "direntry.h"

char * MsDosDirEntry::getName() {
	return (char*)data;
}
char * MsDosDirEntry::getExt() {
	return (char*) (&data[8]);
}

fatEntry MsDosDirEntry::getStartCluster() {
	fatEntry s;
	s=data[27]; s<<=8;;
	s+=data[26];
	return s;
}

unsigned long MsDosDirEntry::getSize(){
	unsigned long l;
	l=data[31]; l<<=8;
	l+=data[30]; l<<=8;
	l+=data[29]; l<<=8;
	l+=data[28];
	return l;
}

char* MsDosDirEntry::getCopyName() {
	int i;
	if (copyName==NULL) {
		if ((copyName=(char*)malloc(13))==NULL) return NULL;
		memcpy(copyName, getName(), 8);
		copyName[8]=0;
		for (i=7;i>0;i--) 
			if (copyName[i]==' ') copyName[i]=0;
			else break;
		if (getExt()[0]!=' ') {
			strcat(copyName, ".");
			i=strlen(copyName);
			memcpy(copyName+i, getExt(), 3);
			copyName[i+3]=0;
			for (i+=3;i>0;i--)
				if (copyName[i]==' ') copyName[i]=0;
				else break;
		}
		for (i=0;i<13;i++) if (copyName[i]==' ') copyName[i]='_';
		//bei Leerzeichen in den Namen macht Windows blöde lange draus
		//die der Kurze dann nicht lesen kann
		for (i=0;i<13;i++) 
			if ((copyName[i]>='A')&&(copyName[i]<='Z'))
				copyName[i]^= 'a' - 'A';
			//Kleinbuchstaben sind einfach hübscher
	}
	return copyName;
}

void MsDosDirEntry::next() {
	data+=32;
	if (copyName!=NULL) {
		free (copyName);
		copyName=NULL;
	}
}


MsDosDirEntry::~MsDosDirEntry() { 
	if (copyName!=NULL) free (copyName);
};
