/*
KurzCdRead 
read and decode media in native Kurzweil format

sensecodes.h - some SCSI Sense Codes. This file is only used
   in order to get nicer error messages when KMSI_Direct can't
   find anything.


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


#define SENSE_NO_SENSE		0x00
#define SENSE_RECOVERED_ERROR 0x01
#define SENSE_NOT_READY		0x02
#define SENSE_MEDIUM_ERROR	0x03
#define SENSE_HARDWARE_ERROR 0x04
#define SENSE_ILLEGAL_REQUEST 0x05

#define MAX_SENSE_CODE		0x0e

char * GetErrorString (int sense);
