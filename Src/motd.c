#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void MOTDFormInit(FormType *frmP) {
	FieldType *fldMOTD;
	MemHandle MOTDH, recH;
	netDBRecord *recP;
	Char *motd;
	
	fldMOTD = GetObjectPtr(MOTDMOTDField);
	MOTDH = MemHandleNew(80);
	motd = MemHandleLock(MOTDH);
	recH = DmQueryRecord(gNetDB, g_server);
	if(recH) {
		recP = MemHandleLock(recH);
		StrCopy(motd, recP->motd);
		MemHandleUnlock(recH);
	} else StrCopy(motd, "");
	MemHandleUnlock(MOTDH);
	FldSetTextHandle(fldMOTD, MOTDH);
	FldDrawField(fldMOTD);
}

static void MOTDFormSaveMOTD() {
	netDBRecord tempRec;
	netDBRecord *recP;
	MemHandle recH, MOTDH;
	Char *motd;
	
	MOTDH = FldGetTextHandle(GetObjectPtr(MOTDMOTDField));
	if(MOTDH) {
		motd = MemHandleLock(MOTDH);
		recH = DmGetRecord(gNetDB, g_server);
		if(recH) {
			recP = MemHandleLock(recH);
			if(recP->MOTDChanged == false) g_prefs.score += SCORE_CHANGE_MOTD;
			tempRec = *recP;
			StrCopy(tempRec.motd, motd);
			tempRec.MOTDChanged = true;
			DmWrite(recP, 0, &tempRec, sizeof(tempRec));
			MemHandleUnlock(recH);
			DmReleaseRecord(gNetDB, g_server, true);
		}
		MemHandleUnlock(MOTDH);
	}
}

static Boolean MOTDFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case MOTDDoneButton:
			MOTDFormSaveMOTD();
			FrmGotoForm(AdministrationForm);
			handled = true;
			break;
	}

	return handled;
}

Boolean MOTDFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    UInt32 ticks;
    UInt16 unit;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return MOTDFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

		case nilEvent:
			ticks = TimGetTicks();
			unit = ticks - g_servertimeindex;
			if(unit >= g_tickspersec) {
				g_servertimeindex = TimGetTicks();
				//ServerFormShowTimeout();
				if(g_servertimeout <= 0) {
					g_prefs.gameover = true;
					FrmGotoForm(GameOverForm);  // caught!
				}
				g_servertimeout--;
			}
			break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            MOTDFormInit(frmP);
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