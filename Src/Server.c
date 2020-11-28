#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void ServerFormFreePointers() {
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(ServerMOTDField)));
	// FreeCharPointer(FldGetTextPtr(GetObjectPtr(ServerTimeoutField)));
	FreeCharPointer((char *)FrmGetTitle(FrmGetActiveForm()));
}

static UInt16 ServerFormGetServerTimeout() {
	MemHandle recordH;
	netDBRecord *recP;
	UInt16 timeout;
	
	if(g_servertimeout == 0) {
		recordH = DmQueryRecord(gNetDB, g_server);
		if(recordH) {
			recP = MemHandleLock(recordH);
			timeout = recP->timeout;
			MemHandleUnlock(recordH);
		}
	} else timeout = g_servertimeout;
	return timeout;
}

static void ServerFormShowTimeout() {
	FieldType *fldP;
	MemHandle timeoutH;
	Char *timeout;
	
	fldP = GetObjectPtr(ServerTimeoutField);
	timeoutH = FldGetTextHandle(fldP);
	if(!timeoutH) timeoutH = MemHandleNew(10);
	timeout = MemHandleLock(timeoutH);
	StrIToA(timeout, g_servertimeout);
	MemHandleUnlock(timeoutH);
	FldSetTextHandle(fldP, timeoutH);
	FldDrawField(fldP);
}

static void ServerFormInit(FormType *frmP) {
	FieldType *fldMOTD;
	MemHandle recH;
	netDBRecord *recP;
	Char *motdP;
	Char *titleP;
	
	g_servertimeindex = 0;
	g_servertimeout = ServerFormGetServerTimeout();	
	ServerFormShowTimeout();
	fldMOTD = GetObjectPtr(ServerMOTDField);
	motdP = FldGetTextPtr(fldMOTD);
	if(!motdP) motdP = MemPtrNew(128);
	recH = DmQueryRecord(gNetDB, g_server);
	if(recH) {
		recP = MemHandleLock(recH);
		StrCopy(motdP, recP->motd);
		MemHandleUnlock(recH);
	}
	titleP = MemPtrNew(25);
	StrCopy(titleP, recP->name);
	FrmSetTitle(frmP, titleP);
	FldSetTextPtr(fldMOTD, motdP);
	FldRecalculateField(fldMOTD, true);
}

static Boolean ServerFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case ServerAccountsPayableButton:
			FrmGotoForm(AccountsPayableForm);
			handled = true;
			break;
		case ServerAdministrationButton:
			FrmGotoForm(AdministrationForm);
			handled = true;
			break;
		case ServerDatabaseButton:
			FrmGotoForm(DatabaseForm);
			handled = true;
			break;
		case ServerLogoffButton:
			FrmGotoForm(MainForm);
			g_servertimeout = 0;
			handled = true;
			break;
	}

	return handled;
}

Boolean ServerFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    UInt32 ticks;
    UInt16 unit;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return ServerFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

		case nilEvent:
			ticks = TimGetTicks();
			unit = ticks - g_servertimeindex;
			if(unit >= g_tickspersec) {
				g_servertimeindex = TimGetTicks();
				ServerFormShowTimeout();
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
            ServerFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	ServerFormFreePointers();
        	break;

        default:
            break;
        }
    
    return handled;
}