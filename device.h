/*
KurzCdRead 
read and decode media in native Kurzweil format

device.h - abstract base class for all types of devices that
   can be used with KMSI_Direct. Feel free to derive your own
   classes (i.e. direct SCSI communication). 


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


class Device {
protected:
	char *name;
	char *vendor;
public:
	Device () {name=NULL; vendor=NULL;};
	virtual ~Device() {};
	virtual char *getName () {return name;};
	virtual char *getVendor () {return vendor;};
	virtual int getType () = 0;
	virtual int getBlockSize() {return 0;};
	virtual int getBlockCount() {return 0;};
	virtual char* ReadSector(int s) = 0;
	virtual char* ReadSector(int s, int c) = 0;
	virtual void RequestSense() = 0;
	virtual int getSenseCode() = 0;
	virtual int getAddSenseCode() = 0;
	virtual int StartStopUnit(int condition, int start) = 0;
	virtual int TestUnitReady() = 0;
	virtual int Reserve() = 0;
	virtual int Release() = 0;
};
