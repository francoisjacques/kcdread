/*
KurzCdRead 
read and decode media in native Kurzweil format

sensecode.cpp - Error messages for KMSI_Direct


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


#include "sensecodes.h"

char *errorStrings[]={
	"No Error",
	"Recovered Error",
	"Unit not Ready",
	"Medium Error",
	"Hardware Error",
	"Illegal Request",
	"Unit Attention",
	"Data Protect",
	"Blank Check",
	"Vendor Specific",
	"Copy Aborted",
	"Aborted Command",
	"Obsolete",
	"Volume Overflow",
	"Miscompare"
};


char * GetErrorString (int sense) {
	if (sense>=SENSE_NO_SENSE) {
		if (sense<=MAX_SENSE_CODE) return errorStrings[sense];
		else return "Unknown Error";
	} else return "";
}
