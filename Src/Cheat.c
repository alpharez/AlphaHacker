#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void CheatFormFreePointers() {
}

static void CheatFormInit(FormType *frmP) {
}

static void CheatFormDoCheat() {
	MemHandle cheatQuantaH, cheatGovtICEH, cheatMoneyH, cheatSuperSniffH;
	MemHandle fldCheatH;
	Char *quanta, *govtice, *money, *supersniff;
	Char *fldCheat;
	
	if(registered) {
		cheatQuantaH = DmGet1Resource(strRsc, CheatQuantaString);
		quanta = MemHandleLock(cheatQuantaH);
		cheatGovtICEH = DmGet1Resource(strRsc, CheatGovtICEString);
		govtice = MemHandleLock(cheatGovtICEH);
		cheatMoneyH = DmGet1Resource(strRsc, CheatMoneyString);
		money = MemHandleLock(cheatMoneyH);
		cheatSuperSniffH = DmGet1Resource(strRsc, CheatSuperSniffString);
		supersniff = MemHandleLock(cheatSuperSniffH);

		fldCheatH = FldGetTextHandle(GetObjectPtr(CheatCheatField));
		if(fldCheatH) {
			fldCheat = MemHandleLock(fldCheatH);
		
			if(!StrCompare(fldCheat, quanta)) g_prefs.cracker = CRACKERQUANTA;
			if(!StrCompare(fldCheat, govtice)) g_prefs.ice = ICEGOVTICE;
			if(!StrCompare(fldCheat, money)) g_prefs.credits = 200000;
			if(!StrCompare(fldCheat, supersniff)) g_prefs.scanner = SCANNERSUPERSNIFF;
		
			MemHandleUnlock(fldCheatH);
		}

		DmReleaseResource(cheatQuantaH);
		DmReleaseResource(cheatGovtICEH);
		DmReleaseResource(cheatMoneyH);
		DmReleaseResource(cheatSuperSniffH);
	} else FrmAlert(RegCheatAlert);
}

static Boolean CheatFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case CheatCheatButton:
			CheatFormDoCheat();
			handled = true;
			break;
		case CheatDoneButton:
			FrmGotoForm(MainForm);
			handled = true;
			break;
	}
	return handled;
}

Boolean CheatFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return CheatFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            CheatFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	CheatFormFreePointers();
        	break;

        default:
            break;
        }
    return handled;
}