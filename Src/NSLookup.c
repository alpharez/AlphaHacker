#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void NSLookupFormSaveData() {
	MemHandle addrH, nameH;
	nslookupDBRecord newRecord;
	nslookupDBRecord *newRecordP;
	MemHandle newRecordH;
	UInt16 index = 0;
	Char *addrP, *nameP;
	
	addrH = FldGetTextHandle(GetObjectPtr(NSLookupAddressField));
	nameH = FldGetTextHandle(GetObjectPtr(NSLookupNameField));
	if(addrH && nameH) { // neither field is null, so save data
		addrP = MemHandleLock(addrH);
		nameP = MemHandleLock(nameH);
		StrCopy(newRecord.address, addrP);
		StrCopy(newRecord.name, nameP);
		StrCopy(newRecord.passcode, "");
		newRecord.status = 0;
		MemHandleUnlock(addrH);
		MemHandleUnlock(nameH);
		newRecordH = DmNewRecord(gNSLookupDB, &index, sizeof(newRecord));
		newRecordP = MemHandleLock(newRecordH);
		DmWrite(newRecordP, 0, &newRecord, sizeof(newRecord));
		MemHandleUnlock(newRecordH);
		DmReleaseRecord(gNSLookupDB, index, true);
	}
}

static void NSLookupFormInit(FormType *frmP) {
	MemHandle addrH, nameH;
	FieldType *fldAddr, *fldName;
	Char *addr, *name;
	
	addrH = MemHandleNew(25);
	nameH = MemHandleNew(25);
	addr = MemHandleLock(addrH);
	name = MemHandleLock(nameH);
	StrCopy(addr, "");
	StrCopy(name, "");
	MemHandleUnlock(addrH);
	MemHandleUnlock(nameH);
	fldAddr = GetObjectPtr(NSLookupAddressField);
	fldName = GetObjectPtr(NSLookupNameField);
	FldSetTextHandle(fldAddr, addrH);
	FldSetTextHandle(fldName, nameH);
}

static Boolean NSLookupFormHasGovtICE(Int16 host) {
	Boolean hasGovtICE;
	MemHandle recordH;
	netDBRecord *recP;

	hasGovtICE = false;
	recordH = DmQueryRecord(gNetDB, host);
	if(recordH) {
		recP = MemHandleLock(recordH);
		if(recP->hasGovtICE) hasGovtICE = true;
		MemHandleUnlock(recordH);
	}
	return hasGovtICE;
}

static void NSLookupFormDoLookup() {
	MemHandle nameH, addressH, recordH;
	Char *addressP, *netP, *hostP;
	FieldType *fldName, *fldAddress;
	netDBRecord *recP;
	Int16 index;
	Char *nameP;
	
	fldName = GetObjectPtr(NSLookupNameField);
	fldAddress = GetObjectPtr(NSLookupAddressField);
	nameH = FldGetTextHandle(fldName);
	if(nameH) {
		nameP = MemHandleLock(nameH);
		if(StrCompare(nameP, "")) {
			index = AHDBFindHostByName(nameP);
			if(index != NOSUCHRECORD) {
				if(!NSLookupFormHasGovtICE(index)) {
					addressH = FldGetTextHandle(fldAddress);
					addressP = MemHandleLock(addressH); 
					recordH = DmQueryRecord(gNetDB, index);
					recP = MemHandleLock(recordH);
					netP = MemPtrNew(10);
					hostP = MemPtrNew(10);
					StrIToA(netP, recP->network);
					StrIToA(hostP, recP->host);
					StrCopy(addressP, netP);
					StrCat(addressP, ".");
					StrCat(addressP, hostP);
					MemHandleUnlock(recordH);
					MemHandleUnlock(addressH);
					
					FldSetTextHandle(fldAddress, addressH);
					FldDrawField(fldAddress);
					MemPtrFree(hostP);
					MemPtrFree(netP);
				}
			}
		} else { // find name from address
			addressH = FldGetTextHandle(fldAddress);
			if(addressH) {
				addressP = MemHandleLock(addressH);
				index = AHDBFindHostByAddr(addressP);
				if(index != NOSUCHRECORD) {
					recordH = DmQueryRecord(gNetDB, index);
					recP = MemHandleLock(recordH);
					StrCopy(nameP, recP->name);
					MemHandleUnlock(recordH);
					//MemHandleUnlock(nameH);
					FldSetTextHandle(fldName, nameH);
					FldDrawField(fldName);
				}
				MemHandleUnlock(addressH);
			}
		}
		MemHandleUnlock(nameH);
	}
}

static Boolean NSLookupFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case NSLookupLookupButton:
			NSLookupFormDoLookup();
			handled = true;
			break;
		case NSLookupOKButton:
			FrmGotoForm(MainForm);
			handled = true;
			break;
		case NSLookupSaveButton:
			NSLookupFormSaveData();
			FrmGotoForm(MainForm);
			handled = true;
			break;
	}
	return handled;
}

Boolean NSLookupFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return NSLookupFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            NSLookupFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	break;

        default:
            break;
        }
    
    return handled;
}
