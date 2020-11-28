#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void AdministrationFormInit(FormType *frmP) {
}

static void AdministrationFormSendEmail() {
	MemHandle recH;
	netDBRecord *recP;
	
	recH = DmQueryRecord(gNetDB, g_server);
	if(recH) {
		recP = MemHandleLock(recH);
		if(!StrCompare(recP->name, "geekempire.com"))
			AHDBAddEmailToDatabase(2);
		if(!StrCompare(recP->name, "music.com"))
			AHDBAddEmailToDatabase(3);
		if(!StrCompare(recP->name, "crack.com"))
			AHDBAddEmailToDatabase(4);
		if(!StrCompare(recP->name, "porno.net"))
			AHDBAddEmailToDatabase(5);
		if(!StrCompare(recP->name, "winnuke.org"))
			AHDBAddEmailToDatabase(6);
		if(!StrCompare(recP->name, "winblows.net"))
			AHDBAddEmailToDatabase(7);
		MemHandleUnlock(recH);
	}

}

static Boolean AdministrationFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;
	
	switch(command) {
		case AdministrationExitButton:
			FrmGotoForm(ServerForm);
			handled = true;
			break;
		case AdministrationShutDownButton: 
			// remove this server record & goto MainForm
			AdministrationFormSendEmail();
			DmRemoveRecord(gNetDB, g_server);
			g_prefs.score += SCORE_SHUTDOWN;
			g_servertimeout=0;
			FrmGotoForm(ShutdownForm);
			handled = true;
			break;
		case AdministrationMOTDButton:
			// goto a form that lets you edit their MOTD
			FrmGotoForm(MOTDForm);
			handled = true;
			break;
	}
	
	return handled;
}

Boolean AdministrationFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;
    UInt32 ticks;
    UInt16 unit;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return AdministrationFormDoButtonCommand(eventP->data.ctlSelect.controlID);
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
            AdministrationFormInit(frmP);
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
