#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void AccountsPayableFormFreePointers() {
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(AccountsPayableBalanceField)));
}

static void AccountsPayableFormInit(FormType *frmP) {
	FieldType *fldBalance;
	MemHandle recH;
	netDBRecord *recP;
	Char *balanceP, *temp;
	
	g_downloading = false;
	g_downloadindex = 0;
	
	temp = MemPtrNew(25);
	fldBalance = GetObjectPtr(AccountsPayableBalanceField);
	balanceP = FldGetTextPtr(fldBalance);
	if(!balanceP) balanceP = MemPtrNew(128);
	recH = DmQueryRecord(gNetDB, g_server);
	if(recH) {
		recP = MemHandleLock(recH);
		StrIToA(temp, recP->balance);
		StrCopy(balanceP, temp);
		// Does the server want something?  If so, we can't transfer money
		if((recP->wantsSourceCode) || (recP->wantsPayrollData) || (recP->wantsUFOData))
			CtlHideControl(GetObjectPtr(AccountsPayableTransferButton));
		MemHandleUnlock(recH);
	}
	FldSetTextPtr(fldBalance, balanceP);
	FldDrawField(fldBalance);
	MemPtrFree(temp);
}

static void AccountsPayableFormDrawProgressGadget() {
	FormType *frmP;
	UInt16 gadgetIndex;
	RectangleType bounds, pbar;
	RGBColorType blueRGB;
	IndexedColorType blueIndexed, oldIndexed;
	
	frmP = FrmGetActiveForm();
	gadgetIndex = FrmGetObjectIndex(frmP, AccountsPayableProgressGadget);
	FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
	pbar.topLeft = bounds.topLeft;
	pbar.extent.x = g_downloadindex;
	pbar.extent.y = bounds.extent.y;
	if(0x80 & g_SupportedDepths) {
		blueRGB.r = 0; blueRGB.g = 0; blueRGB.b = 255;
		blueIndexed = WinRGBToIndex(&blueRGB);
		oldIndexed = WinSetForeColor(blueIndexed);
		WinDrawRectangle(&pbar, 1);
		WinSetForeColor(oldIndexed);
	} else {
		WinDrawRectangle(&pbar, 1);
	}
}

static Boolean AccountsPayableFormFundsTaken() {
	Boolean taken = false;
	MemHandle recH;
	netDBRecord *recP;
	
	recH = DmQueryRecord(gNetDB, g_server);
	if(recH) {
		recP = MemHandleLock(recH);
		taken = recP->FundsTaken;
		MemHandleUnlock(recH);
	}
	return taken;
}

static void AccountsPayableFormTakeCredits() {
	MemHandle recH;
	netDBRecord *recP, temp;
	FieldType *fldBalance;
	Char *balanceP, *oldP;

	fldBalance = GetObjectPtr(AccountsPayableBalanceField);
	recH = DmGetRecord(gNetDB, g_server);
	if(recH) {
		recP = MemHandleLock(recH);
		if(recP->FundsTaken == false) g_prefs.score += SCORE_TRANSFER;
		temp = *recP;
		temp.balance = 0;
		temp.FundsTaken = true;
		g_prefs.credits += recP->balance;
		g_prefs.numCracked++;
		DmWrite(recP, 0, &temp, sizeof(temp));
		MemHandleUnlock(recH);
		DmReleaseRecord(gNetDB, g_server, true);
		oldP = FldGetTextPtr(fldBalance);
		balanceP = MemPtrNew(2);
		StrCopy(balanceP, "0");
		FldSetTextPtr(fldBalance, balanceP);
		if(oldP) MemPtrFree(oldP);
		FldDrawField(fldBalance);
	}
}

static Boolean AccountsPayableFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case AccountsPayableTransferButton:
			// have funds been taken already?
			if(!AccountsPayableFormFundsTaken()) 
				g_downloading = true;
			handled = true;
			break;
		case AccountsPayableExitButton:
			FrmGotoForm(ServerForm);
			handled = true;
			break;
	}

	return handled;
}

Boolean AccountsPayableFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;
    UInt16 gadgetIndex;
    RectangleType bounds;
    UInt32 ticks;
    UInt16 unit;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return AccountsPayableFormDoButtonCommand(eventP->data.ctlSelect.controlID);
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
			if(g_downloading) {
				AccountsPayableFormDrawProgressGadget();
				if(g_downloadindex >= 100) {
					g_downloading = false;
					AccountsPayableFormTakeCredits();
					frmP = FrmGetActiveForm();
					gadgetIndex = FrmGetObjectIndex(frmP, AccountsPayableProgressGadget);
					FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
					WinEraseRectangle(&bounds, 1);
				}
			}
			g_downloadindex += 2;
			break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            AccountsPayableFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	AccountsPayableFormFreePointers();
        	break;

        default:
            break;
        }
    
    return handled;
}