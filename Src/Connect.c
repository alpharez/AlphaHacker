#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void ConnectFormFreePointers() {
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(ConnectOutputField)));
}

static void ConnectFormInit(FormType *frmP) {

}

static Boolean ConnectFormCheckPassword() {
	MemHandle recordH, passwordH;
	netDBRecord *recP;
	Char *passwordP;
	Boolean access = false;
	
	passwordH = FldGetTextHandle(GetObjectPtr(ConnectPasswordField));
	if(passwordH) {
		passwordP = MemHandleLock(passwordH);
		recordH = DmQueryRecord(gNetDB, g_server);
		if(recordH) {
			recP = MemHandleLock(recordH);
			if(!StrCompare(recP->passcode, passwordP)) access = true;
			MemHandleUnlock(recordH);
		}
		MemHandleUnlock(passwordH);
	}		
	return access;
}

static Boolean ConnectFormCheckICE() {
	MemHandle recordH;
	netDBRecord *recP;
	Boolean access = false;
	
	recordH = DmQueryRecord(gNetDB, g_server);
	if(recordH) {
		recP = MemHandleLock(recordH);
		if((!recP->hasICE) && (!recP->hasGovtICE)) access = true;
		MemHandleUnlock(recordH);
	}
	return access;
}

static void ConnectFormGotoServer() {
	FieldType *fldAddr;
	MemHandle addrH;
	Char *addrP;
	
	fldAddr = GetObjectPtr(ConnectAddressField);
	addrH = FldGetTextHandle(fldAddr);
	if(addrH) {
		addrP = MemHandleLock(addrH);
		if(addrP) g_server = AHDBFindHostByAddr(addrP);
		else g_server = NOSUCHRECORD;
		MemHandleUnlock(addrH);
	} else g_server = NOSUCHRECORD;
}

static void ConnectFormDisplayServerError(Char *error) {
	FieldType *fldOutput;
	Char *output, *old;
	
	fldOutput = GetObjectPtr(ConnectOutputField);
	old = FldGetTextPtr(fldOutput);
	output = MemPtrNew(30);
	StrCopy(output, error);
	FldSetTextPtr(fldOutput, output);
	if(old) MemPtrFree(old);
	FldDrawField(fldOutput);
}

static Boolean ConnectFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case ConnectConnectButton:
			ConnectFormGotoServer();
			if(g_server != NOSUCHRECORD) {
				if(ConnectFormCheckICE()) {
					if(ConnectFormCheckPassword()) {
						FrmGotoForm(ServerForm);
					} else ConnectFormDisplayServerError("Access Denied!");
				} else ConnectFormDisplayServerError("Connection Terminated!");	
			} else ConnectFormDisplayServerError("Connection Failed!");
			handled = true;
			break;
		case ConnectCancelButton:
			FrmGotoForm(MainForm);
			handled = true;
			break;
	}

	return handled;
}

Boolean ConnectFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return ConnectFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            ConnectFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	ConnectFormFreePointers();
        	break;

        default:
            break;
        }
    
    return handled;
}