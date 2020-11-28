#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void HostFormFreePointers() {
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(HostNameField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(HostAddressField)));
}

static void HostFormInit(FormType *frmP) {
	FieldType *fldName, *fldAddress, *fldPassword;
	Char *name, *address, *password;
	MemHandle recH, passwordH;
	nslookupDBRecord *recP;
	Int16 status;
	
	fldName = GetObjectPtr(HostNameField);
	fldAddress = GetObjectPtr(HostAddressField);
	fldPassword = GetObjectPtr(HostPasswordField);
	
	address = MemPtrNew(10);
	name = MemPtrNew(16);
	passwordH = MemHandleNew(10);
	
	password = MemHandleLock(passwordH);
	
	recH = DmQueryRecord(gNSLookupDB, g_hostRecNum);
	recP = MemHandleLock(recH);
	
	StrCopy(name, recP->name);
	StrCopy(address, recP->address);
	StrCopy(password, recP->passcode);
	switch(recP->status) {
		case 0:
			status = HostWorkingPushButton;
			break;
		case 1:
			status = HostCrackedPushButton;
			break;
		case 2:
			status = HostShutDownPushButton;
			break;
		case 3:
			status = HostICEPushButton;
			break;
		default:
			status = HostWorkingPushButton;
			break;
	}
	
	MemHandleUnlock(recH);
	MemHandleUnlock(passwordH);
	
	FldSetTextPtr(fldName, name);
	FldSetTextPtr(fldAddress, address);
	FldSetTextHandle(fldPassword, passwordH);
	FrmSetControlValue(frmP, FrmGetObjectIndex(frmP, status), true);
	
	FldDrawField(fldName);
	FldDrawField(fldAddress);
	FldDrawField(fldPassword);
}

static void HostFormUpdateData() {
	MemHandle passH;
	nslookupDBRecord tempRecord;
	nslookupDBRecord *recordP;
	MemHandle recordH;
	Char *passP;
	UInt16 control;
	
	passH = FldGetTextHandle(GetObjectPtr(HostPasswordField));
	if(passH) { // field is null, so save data
		passP = MemHandleLock(passH);
		recordH = DmGetRecord(gNSLookupDB, g_hostRecNum);
		if(recordH) {
			recordP = MemHandleLock(recordH);
			tempRecord = *recordP;
			StrCopy(tempRecord.passcode, passP);
			control = FrmGetControlGroupSelection(FrmGetActiveForm(), HostGroupID);
			control += 2700; // to get FormID;
			switch(control) {
				case HostWorkingPushButton:
					tempRecord.status = 0;
					break;
				case HostCrackedPushButton:
					tempRecord.status = 1;
					break;
				case HostShutDownPushButton:
					tempRecord.status = 2;
					break;
				case HostICEPushButton:
					tempRecord.status = 3;
					break;
				default:
					tempRecord.status = 0;
					break;
			}
			DmWrite(recordP, 0, &tempRecord, sizeof(tempRecord));
			MemHandleUnlock(recordH);
			DmReleaseRecord(gNSLookupDB, g_hostRecNum, true);
		}
		MemHandleUnlock(passH);
	}
}

static Boolean HostFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case HostOkButton:
			HostFormUpdateData();
			FrmGotoForm(MainForm);
			handled = true;
			break;
		case HostDeleteButton:
			if(FrmAlert(DeleteHostAlert) == 0) {
				DmRemoveRecord(gNSLookupDB, g_hostRecNum);
				FrmGotoForm(MainForm);
			}
			handled = true;
			break;
	}
	return handled;
}

Boolean HostFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return HostFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            HostFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	HostFormFreePointers();
        	break;

        default:
            break;
        }
    
    return handled;
}