/*
KurzCdRead 
read and decode media in native Kurzweil format

virtualdevice.h - simple example on how to derive a class from Device.


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


#define VIRTUAL_BLOCKSIZE 512

class VirtualDevice :public Device {
protected:
	FILE *f;
public:
	VirtualDevice () {
		f=NULL;
		name=strdup("");
		vendor=strdup("Virtual Device");
	};
	VirtualDevice (char *);
	virtual ~VirtualDevice();
	virtual int getType () {return 0;};
	virtual int getBlockSize() {return VIRTUAL_BLOCKSIZE;};
	virtual int getBlockCount();
	virtual char* ReadSector(int s);
	virtual char* ReadSector(int s, int c);
	virtual void RequestSense() {};
	virtual int getSenseCode() {
		return (f!=NULL) ? 0 : 2;//not ready
	};						
	virtual int getAddSenseCode() {return -1;};
	virtual int StartStopUnit(int condition, int start) {
		return (f!=NULL) ? 0 : -1;
	};
	virtual int TestUnitReady() {
		return (f!=NULL) ? 0 : -1;
	};
	virtual int Reserve(){return 0;};
	virtual int Release(){return 0;};
};

