#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void SWInvFormFreePointers() {
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(SWInventoryScannerField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(SWInventoryCrackerField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(SWInventoryICEBreakerField)));
}

static void SWInvFormInit(FormType *frmP) {
	FieldType *fldScanner, *fldCracker, *fldICE;
	Char *scanner, *cracker, *ice;
	
	scanner = MemPtrNew(20);
	cracker = MemPtrNew(20);
	ice = MemPtrNew(20);

	switch(g_prefs.scanner) {
		case SCANNERSNOOP:
			StrCopy(scanner, "Snoop");
			break;
		case SCANNERSNIFF2:
			StrCopy(scanner, "Sniff 2.0");
			break;
		case SCANNERSUPERSNIFF:
			StrCopy(scanner, "Super Sniff");
			break;
		default:
			StrCopy(scanner, "NO SCANNER");
			break;
	}
	switch(g_prefs.cracker) {
		case CRACKERPASSCRACK:
			StrCopy(cracker, "Pass Crack");
			break;
		case CRACKERL337CRACK:
			StrCopy(cracker, "L337 Crack");
			break;
		case CRACKERSUPERNOVA:
			StrCopy(cracker, "Super Nova");
			break;
		case CRACKERDESCRACK:
			StrCopy(cracker, "DES Crack");
			break;
		case CRACKERQUANTA:
			StrCopy(cracker, "Quanta");
			break;
		default:
			StrCopy(cracker, "NO CRACKER");
			break;
	}
	switch(g_prefs.ice) {
		case ICETECHICE:
			StrCopy(ice, "Tech ICE");
			break;
		case ICEGOVTICE:
			StrCopy(ice, "Government ICE");
			break;
		default:
			StrCopy(ice, "NO ICE");
			break;
	}
	
	fldScanner = GetObjectPtr(SWInventoryScannerField);
	FldSetTextPtr(fldScanner, scanner);
	fldCracker = GetObjectPtr(SWInventoryCrackerField);
	FldSetTextPtr(fldCracker, cracker);
	fldICE = GetObjectPtr(SWInventoryICEBreakerField);
	FldSetTextPtr(fldICE, ice);
	FldDrawField(fldScanner);
	FldDrawField(fldCracker);
	FldDrawField(fldICE);
}

static Boolean SWInvFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;
	
	switch(command) {
		case SWInventoryOkButton:
			FrmGotoForm(MainForm);
			handled = true;
			break;
	}
	
	return handled;
}

Boolean SWInvFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return SWInvFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            SWInvFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	SWInvFormFreePointers();
        	break;

        default:
            break;
        }
    
    return handled;
}
