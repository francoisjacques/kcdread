/*
KurzCdRead 
read and decode media in native Kurzweil format

kmsi_direct.cpp - that's where the action is. The KMSI_Direct
  object reads the data from an abstract Device and writes 
  information out to an abstract Ausgabe. Feel free to write
  your own implementations of these classes.

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


#ifdef _GCC
  #include <endian.h>
  #ifndef MY_BYTE_ORDER
    #define MY_BYTE_ORDER  __BYTE_ORDER
  #endif

  extern "C" int mkdir (char *);
  extern "C" int chdir (char *);
  #define kmsi_mkdir mkdir
#endif


#ifndef __LITTLE_ENDIAN 
   #define __LITTLE_ENDIAN 1234
   #define __BIG_ENDIAN 4321
#endif


#ifdef _WIN32
#ifndef MY_BYTE_ORDER
  #define MY_BYTE_ORDER __LITTLE_ENDIAN
#endif
  #include <direct.h>
  #define kmsi_mkdir _mkdir
#else
#ifdef _MAC
  #define MY_BYTE_ORDER __BIG_ENDIAN
#endif
#endif


#include "device.h"
#include "ausgabe.h"
#include "direntry.h"
#include "kmsi_direct.h"
#include "sensecodes.h"

int KMSI_Direct::deinit() {
  if (device->getType()==0) 
    if (device->Release()) {
      displayErrors();
      return -1;
    }
  return 0;
}

void KMSI_Direct::setAusgabe (Ausgabe *aus) {
  if (ausgabe!=NULL) delete ausgabe;
  ausgabe=aus;
}

int KMSI_Direct::readMBR(char * sektor) {
  char *partition = &sektor[0x1be];
  //if (partition[4]!=4) return -1;
  //FAT16
  unsigned long offset = 0;
  unsigned long *l = (unsigned long*)&partition[8];
#if MY_BYTE_ORDER == __LITTLE_ENDIAN
  offset = *l;
#else //__BIG_ENDIAN
  offset = (0xff000000 & ((*l)<<24)) +
    (0x00ff0000 & ((*l)<<8)) +    
    (0x0000ff00 & ((*l)>>8)) +
    (0x000000ff & ((*l)>>24));
#endif
  return readBootSector(offset);
}


#define le_ushort(lsb, msb, res) res=msb; res<<=8; res+=lsb;

int KMSI_Direct::readBootSector(long startpos) {
  int fat_count, count, curSek;
  char *sek0;
  int p;// //Startposition fürs Lesen
  char buf[80];

  unsigned char lsb, msb;
  unsigned short s;

  if (device==NULL) return -1;

  device->StartStopUnit(0,1);

  count=device->getBlockSize();
  p=0xb+startpos*count;

  curSek=p/count;

  if ((sek0=device->ReadSector(curSek))==NULL) return -1;

  p%=count;

  lsb=sek0[p++];
  msb=sek0[p++];
  le_ushort (lsb, msb, s);
  sektor_size=s;
  if (sektor_size!= 512) {
    sprintf(buf, "Sector size %i unsupported", sektor_size);
    ausgabe->TextAusgabe(buf);
    return -1;
  }
	
  cluster_size=sek0[p++];
  max_direntries_per_cluster = cluster_size;
  max_direntries_per_cluster*=sektor_size;
  max_direntries_per_cluster/=MSDOS_DIRENTRY_SIZE;
  if ((cluster_size == 0)||((cluster_size % 2) != 0)) {
    sprintf(buf, "Cluster size %i unsupported", cluster_size);
    ausgabe->TextAusgabe(buf);
    return -1;
  }

  lsb=sek0[p++];
  msb=sek0[p++];
  le_ushort (lsb, msb, s);
  fat_start=s + startpos;

  fat_count = sek0[p++];

  lsb=sek0[p++];
  msb=sek0[p++];
  le_ushort (lsb, msb, s);
  root_size=s;
  //Größe des Wurzelverzeichnisses (in Einträgen)

  lsb=sek0[p++];
  msb=sek0[p++];
  //größe in Sektoren. sollte *nicht* in ein Word passen, daher 0 sein
  le_ushort (lsb, msb, s);
  if (s!= 0) {
    ausgabe->TextAusgabe("Unexpected Size of Medium");
    return -1;
  }

  lsb=sek0[p++];
  //MediaDescriptor, sollte Festplatte (!) sein
  if (lsb!= 0xf8) {
    ausgabe->TextAusgabe("Unexpected MediaDescriptor");
    return -1;
  }

  lsb=sek0[p++];
  msb=sek0[p];
  //sektoren pro fat
  le_ushort (lsb, msb, s);
  fat_size=s;

  root_start = fat_start + fat_count * fat_size;
  cluster_base = root_start - 2*cluster_size + 
    root_size*MSDOS_DIRENTRY_SIZE/sektor_size;
  return 0;
}

void KMSI_Direct::displayErrors() {
  int sense = device->getSenseCode();
  if (sense<=MAX_SENSE_CODE)
    ausgabe->TextAusgabe(GetErrorString(sense));
  //Nur ausgeben, wenn der Fehler im bekannten
  //Bereich liegt
}

int KMSI_Direct::readFat() {
  int bytesToRead, count, readPosition, curSek;
  char *sektor, *fatPointer;

  bytesToRead=fat_size*sektor_size;
  //ist jetzt Zahl der zu lesenden Bytes!!

  fat = (fatEntry*)malloc(bytesToRead);
  if (fat==NULL) return -1;
  fatPointer=(char*)fat;


  readPosition=sektor2byte(fat_start);
  //readPosition ist Zeiger auf der CDRom


  count=readPosition%device->getBlockSize();
  if (count > 0) {
    //angebrochener erster Sektor
    curSek=readPosition/device->getBlockSize();
		
    if ((sektor=device->ReadSector(curSek))==NULL) {
      displayErrors();
      free(fat);
      fat=NULL;
      return -1;
    }
		
    //Rest aus erstem Sektor ab in die Fat
    memcpy(fatPointer, sektor+count, device->getBlockSize()-count);
    free(sektor);
    bytesToRead-=device->getBlockSize()-count;
    fatPointer+=device->getBlockSize()-count;
    readPosition+=device->getBlockSize()-count;
  }

  count=device->getBlockSize();
  while (bytesToRead>=count) {
    //ganze Sektoren ab in die Fat
    curSek=readPosition/count;
    if ((sektor=device->ReadSector(curSek))==NULL) {
      displayErrors();
      free(fat);
      fat=NULL;
      return -1;
    }

    memcpy(fatPointer, sektor, count);
    free(sektor);
    bytesToRead-=count;
    fatPointer+=count;
    readPosition+=count;
  }
  if (bytesToRead>0) {
    //Anfang eines Sektors ab in die Fat
    curSek=readPosition/count;
    if ((sektor=device->ReadSector(curSek))==NULL) {
      displayErrors();
      free(fat);
      fat=NULL;
      return -1;
    }
    memcpy(fatPointer, sektor, bytesToRead);
    free(sektor);
  }
  return 0;
}

fatEntry KMSI_Direct::next_cluster(fatEntry cl) {
#if MY_BYTE_ORDER == __LITTLE_ENDIAN
	return fat[cl];
#else
	unsigned short s;
	s=fat[cl] << 8;
	s+=fat[cl] >> 8;
	return s;
#endif
};


int KMSI_Direct::init() {
  char *sek0;

  if (device==NULL) return -1;
  ausgabe->TextAusgabe("Init");

  if (device->getType()==0) {
    //Direct Access Device
    if (device->Reserve()) {
      displayErrors();
      return -1;
    }
  }

  if (device->StartStopUnit(0,1)) {
    displayErrors();
    return -1;
  }

  if ((sek0=device->ReadSector(0))==NULL) {
    displayErrors();
    return -1;
  }

  if ((sek0[0x1fe]==(char)0x55)&&(sek0[0x1ff]==(char)0xaa)) {
    //ist MBR
    if (readMBR(sek0)) return -1;
  } else {
    //beginnt sofort
    if (readBootSector(0)) return -1;
  }
  free(sek0);

  if (readFat()) return -1;

  ausgabe->TextAusgabe("Init complete");

  return 0;
}


char* KMSI_Direct::readClusters(int start, int cl_count) {
  char *buf, *source, *dest;
  int sek, ofs, count, gesamt;
  int pos = cluster2byte(start);
  //pos ist Zeiger auf der CDRom
	
  gesamt=sektor_size*cluster_size*cl_count;

  sek=pos/device->getBlockSize();
  //sektor zum anfangen
  ofs=pos%device->getBlockSize();
  //offset in diesen
  count=(pos+gesamt)/device->getBlockSize();
  //letzter zu lesender Sektor
  if (((pos+gesamt)%device->getBlockSize())>0) count++;
  //inkrementieren falls angebrochener Sektor dahinter

  if ((buf=device->ReadSector(sek, count-sek))==NULL) {
    displayErrors();
    return NULL;
  }
  source=buf+ofs;
  if ((dest = (char*)malloc (gesamt))==NULL) return NULL;
  memcpy(dest, source, gesamt);
  free(buf);
  return dest;
}

int KMSI_Direct::copyFile (MsDosDirEntry *entry) {
  FILE *f;
  long l;
  int next_cl, clustersPerRead;
  char *clusters;

  char buf[40];

  strcpy(buf, "File ");
  strcat(buf, entry->getCopyName());
  ausgabe->TextAusgabe(buf);

  //Datei anlegen....
  if ((f = fopen (entry->getCopyName(), "w+b"))==NULL) {
    ausgabe->TextAusgabe("Unable to Create File");
    return -1;			
  };
  l=0;
  next_cl=entry->getStartCluster();

  while (entry->getSize() - l >= (unsigned) sektor_size*cluster_size) {
    ausgabe->DateiFortschritt(l, entry->getSize());
    for (clustersPerRead=1;
	 clustersPerRead*sektor_size*cluster_size<65536; 
	 clustersPerRead++) 
      if (next_cluster(next_cl)!=(next_cl+clustersPerRead)) break;

    if ((clusters=readClusters(next_cl, clustersPerRead))==NULL) {
      fclose(f);
      return -1;
    }
    if (entry->getSize() - l > (unsigned) sektor_size*cluster_size*clustersPerRead) {
      if (fwrite(clusters, sektor_size*cluster_size*clustersPerRead, 1, f) < 1) {
	//write failed
	ausgabe->TextAusgabe("Write Failed");
	fclose(f);
	free (clusters);
	return -1;			
      };
      free (clusters);
      l+=sektor_size*cluster_size*clustersPerRead;
      next_cl=next_cluster(next_cl+clustersPerRead-1);
    } else {
      if (fwrite(clusters, entry->getSize() - l, 1, f) < 1) {
	//write failed
	ausgabe->TextAusgabe("Write Failed");
	fclose(f);
	free (clusters);
	return -1;			
      };
      free (clusters);
      fclose(f);
      return 0;
    }
  }
  if (entry->getSize() - l > 0) {
    ausgabe->DateiFortschritt(l, entry->getSize());
    if ((clusters=readClusters(next_cl, 1))==NULL) {
      fclose(f);
      return -1;
    }
    if (fwrite(clusters, entry->getSize() - l, 1, f) < 1) {
      //write failed
      ausgabe->TextAusgabe("Write Failed");
      fclose(f);
      free (clusters);
      return -1;			
    };
    free (clusters);
  }
  ausgabe->DateiFortschritt(l, entry->getSize());
  fclose(f);
  return 0;
}

int KMSI_Direct::readRootDir() {
  if (fat==NULL) return -1;
  int i;

  int startsek, endsek;
  int startpos, endpos;
  char *block;
  MsDosDirEntry entry;

  startpos=sektor2byte(root_start);
  endpos=startpos+32*root_size;

  startsek=startpos/device->getBlockSize();
  endsek=endpos/device->getBlockSize();
  if ((endpos%device->getBlockSize())>0) endsek++;

  if ((block=device->ReadSector(startsek, endsek-startsek))==NULL) return -1;
  //und jetzt Einträge suchen
  entry.setData(block+startpos%device->getBlockSize());
  for (i=0;i<root_size;i++, entry.next()) {
    if (!entry.isUsed()) break; //hier stand noch nie was
    if (entry.isDeleted()) continue; //Datei gelöscht
    if (entry.startsWithDot()) continue; //Name beginnt mit Punkt
    if (entry.isLabel()) continue; //ist ein Label

    //Hier ist was echtes!!
    if (entry.isDirectory()) {
      if (copyDirectory (&entry)) {
	free (block);
	return -1;
      }
    }
    else {
      if (entry.getSize()==0) continue; //ist Scheiße
      if (copyFile (&entry)) {
	free (block);
	return -1;
      }
    }

  }
  free (block);
  return 0;
}

int KMSI_Direct::readDirectory (MsDosDirEntry *e) {
  if (fat==NULL) return -1;

  char *block;
  MsDosDirEntry entry;

  int next_cl;
  int i;

  next_cl=e->getStartCluster();
  do {
    if ((block=readClusters(next_cl, 1))==NULL) return -1;
    //und jetzt Einträge suchen
    entry.setData(block);
    for (i=0;i<max_direntries_per_cluster;i++, entry.next()) {
      if (!entry.isUsed()) break; //hier stand noch nie was
      if (entry.isDeleted()) continue; //Datei gelöscht
      if (entry.startsWithDot()) continue; //Name beginnt mit Punkt
      if (entry.isLabel()) continue; //ist ein Label
      if (entry.getStartCluster()==0) continue; //ist Scheiße

      //Hier ist was echtes!!
      if (entry.isDirectory()) {
	if (copyDirectory (&entry)) {
	  free (block);
	  return -1;
	}
      }
      else {
	if (entry.getSize()==0) continue; //ist Scheiße
	if (copyFile (&entry)) {
	  free (block);
	  return -1;
	}
      }

    }
    next_cl=next_cluster(next_cl);
    free (block);
  } while (next_cl<0xfff8);
  return 0;

}

int KMSI_Direct::copyDirectory (MsDosDirEntry *entry) {
  char buf[40];

  strcpy(buf, "Directory ");
  strcat(buf, entry->getCopyName());
  ausgabe->TextAusgabe(buf);

  //Directory anlegen und wechseln
  if (kmsi_mkdir (entry->getCopyName())==0) {
    chdir (entry->getCopyName());
    if (readDirectory (entry)) return -1;
    //rekursiv durchgehen
    chdir (".."); 
  } else {
    if (chdir (entry->getCopyName())) {
      ausgabe->TextAusgabe("Unable to Create Directory");
      return -1; 
    } else {
      //mkdir fehlgeschlagen, weil directory schon existierte
      if (readDirectory (entry)) return -1;
      //rekursiv durchgehen
      chdir (".."); 
    }
  }
  return 0;
}
