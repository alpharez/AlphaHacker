#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void ICEFormFreePointers() {
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(ICEBreakerMessageField)));
}

static void ICEFormInit(FormType *frmP) {
}

static void ICEFormGotoServer() {
	FieldType *fldAddr;
	MemHandle addrH;
	Char *addrP;
	
	fldAddr = GetObjectPtr(ICEBreakerAddressField);
	addrH = FldGetTextHandle(fldAddr);
	if(addrH) {
		addrP = MemHandleLock(addrH);
		if(addrP) g_server = AHDBFindHostByAddr(addrP);
		else g_server = NOSUCHRECORD;
		MemHandleUnlock(addrH);
	} else g_server = NOSUCHRECORD;
}

static void ICEFormDoICEBreak() {
	netDBRecord tempRec;
	netDBRecord *recP;
	MemHandle recH;
	FieldType *fldP;
	Char *msg;
	
	fldP = GetObjectPtr(ICEBreakerMessageField);
	if(g_server != NOSUCHRECORD) {
		recH = DmGetRecord(gNetDB, g_server);
		if(recH) {
			recP = MemHandleLock(recH);
			if((recP->hasICE) || (recP->hasGovtICE)) {
				if((g_prefs.ice == ICETECHICE) && (recP->hasGovtICE)) {
					// show message ICEBreak failed
					msg = FldGetTextPtr(fldP);
					if(!msg) msg = MemPtrNew(25);
					StrCopy(msg, "ICE Break failed!");
					FldSetTextPtr(fldP, msg);
					FldDrawField(fldP);
				} else {
					tempRec = *recP;
					tempRec.hasICE = false;
					if(g_prefs.ice == ICEGOVTICE)
						tempRec.hasGovtICE = false;
					DmWrite(recP, 0, &tempRec, sizeof(tempRec));
					// show message ICEBreak successful
					msg = FldGetTextPtr(fldP);
					if(!msg) msg = MemPtrNew(25);
					StrCopy(msg, "SUCCESS!");
					FldSetTextPtr(fldP, msg);
					FldDrawField(fldP);
				}
			} else {
				// show message server has no ICE
				msg = FldGetTextPtr(fldP);
				if(!msg) msg = MemPtrNew(25);
				StrCopy(msg, "Server has no ICE");
				FldSetTextPtr(fldP, msg);
				FldDrawField(fldP);
			}
			MemHandleUnlock(recH);
			DmReleaseRecord(gNetDB, g_server, true);
		}
	} else {
		// show no such server message
		msg = FldGetTextPtr(fldP);
		if(!msg) msg = MemPtrNew(25);
		StrCopy(msg, "No Such Server.");
		FldSetTextPtr(fldP, msg);
		FldDrawField(fldP);
	}
}

static Boolean ICEFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case ICEBreakerBreakButton:
			ICEFormGotoServer();
			ICEFormDoICEBreak();
			handled = true;
			break;
		case ICEBreakerDoneButton:
			FrmGotoForm(MainForm);
			handled = true;
			break;
	}

	return handled;
}

Boolean ICEFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return ICEFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            ICEFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	ICEFormFreePointers();
        	break;

        default:
            break;
        }
    
    return handled;
}