#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void DatabaseFormInit(FormType *frmP) {
	MemHandle recH, fldH, fldClueH, recordH;
	netDBRecord *recP, *rec2P;
	Char *msg, *temp, *msgclue;
	FieldType *fldP;
	Int16 host;

	g_downloading = false;
	g_downloadindex = 0;
	
	recH = DmQueryRecord(gNetDB, g_server);
	recP = MemHandleLock(recH);
	if(recH) {
		if(recP->hasSourceCode)
			CtlSetUsable(GetObjectPtr(DatabaseSourceCodeButton), true);
		else if(recP->hasPayrollData)
			CtlSetUsable(GetObjectPtr(DatabasePayrollButton), true);
		else if(recP->hasUFOData)
			CtlSetUsable(GetObjectPtr(DatabaseUFOButton), true);
		else if(recP->wantsSourceCode)
			CtlSetUsable(GetObjectPtr(DatabaseUploadSourceButton), true);
		else if(recP->wantsPayrollData)
			CtlSetUsable(GetObjectPtr(DatabaseUploadPayrollButton), true);
		else if(recP->wantsUFOData)
			CtlSetUsable(GetObjectPtr(DatabaseUploadUFOButton), true);
		else {
			fldP = GetObjectPtr(DatabaseNoDataField);
			fldH = FldGetTextHandle(fldP);
			if(!fldH) fldH = MemHandleNew(10);
			msg = MemHandleLock(fldH);
			StrCopy(msg, "No Data");
			MemHandleUnlock(fldH);
			FldSetTextHandle(fldP, fldH);
			FldDrawField(fldP);
		}
		if(recP->hasClue) {
			fldP = GetObjectPtr(DatabaseDataField);
			fldClueH = FldGetTextHandle(fldP);
			if(!fldClueH) fldClueH = MemHandleNew(55);
			msg = MemHandleLock(fldClueH);
			StrCopy(msg, recP->clue);
			// add hexagon.mil address to msg
			temp = MemPtrNew(25);
			msgclue = MemPtrNew(55);
			if(!StrCompare(recP->name, "octogon.gov"))
				StrCopy(temp, "hexagon.mil");
			else
				StrCopy(temp, "payyou.com");
				
			host = AHDBFindHostByName(temp);
			recordH = DmQueryRecord(gNetDB, host);
			if(recordH) {
				rec2P = MemHandleLock(recordH);
				StrCopy(msgclue, " ");
				StrIToA(temp, rec2P->network);
				StrCat(msgclue, temp);
				StrCat(msgclue, ".");
				StrIToA(temp, rec2P->host);
				StrCat(msgclue, temp);
				MemHandleUnlock(recordH);
			}
			StrCat(msg, msgclue);

			MemHandleUnlock(fldClueH);
			FldSetTextHandle(fldP, fldClueH);
			FldDrawField(fldP);
			MemPtrFree(temp);
			MemPtrFree(msgclue);
		}
		MemHandleUnlock(recH);
		FrmDrawForm(frmP);
	}
}

static void DatabaseFormTakeCredits() {
	MemHandle recH;
	netDBRecord *recP, temp;

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
	}
}

static void DatabaseFormDrawProgressGadget() {
	FormType *frmP;
	UInt16 gadgetIndex;
	RectangleType bounds, pbar;
	RGBColorType blueRGB;
	IndexedColorType blueIndexed, oldIndexed;
	
	frmP = FrmGetActiveForm();
	gadgetIndex = FrmGetObjectIndex(frmP, DatabaseProgressGadget);
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

static void DatabaseFormTakeObject(UInt16 object) {
	netDBRecord tempRec;
	netDBRecord *recP;
	MemHandle recH;
	
	if(g_server != NOSUCHRECORD) {
		recH = DmGetRecord(gNetDB, g_server);
		if(recH) {
			recP = MemHandleLock(recH);
			tempRec = *recP;
			switch(object) {
				case 1:
					tempRec.hasSourceCode = false;
					break;
				case 2:
					tempRec.hasPayrollData = false;
					break;
				case 3:
					tempRec.hasUFOData = false;
					break;
				case 4:
					tempRec.wantsSourceCode = false;
					break;
				case 5:
					tempRec.wantsPayrollData = false;
					break;
				case 6:
					tempRec.wantsUFOData = false;
					break;
			}
			DmWrite(recP, 0, &tempRec, sizeof(tempRec));
			MemHandleUnlock(recH);
			DmReleaseRecord(gNetDB, g_server, true);
		}
	}

}

static Boolean DatabaseFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;
	FormType *frmP;

	frmP = FrmGetActiveForm();
	switch(command) {
		case DatabaseSourceCodeButton:
			g_databaseobject=1;
			g_downloading=true;
			handled = true;
			break;
		case DatabasePayrollButton:
			g_databaseobject=2;
			g_downloading=true;
			handled = true;
			break;
		case DatabaseUFOButton:
			g_databaseobject=3;
			g_downloading=true;
			handled = true;
			break;
		case DatabaseUploadSourceButton:
			if(g_prefs.source > 0) {
				g_databaseobject=4;
				g_downloading=true;
			} else FrmAlert(NoSourceAlert);
			handled=true;
			break;
		case DatabaseUploadPayrollButton:
			if(g_prefs.payroll > 0) {
				g_databaseobject=5;
				g_downloading=true;
			} else FrmAlert(NoPayrollAlert);
			handled=true;
			break;
		case DatabaseUploadUFOButton:
			if(g_prefs.ufo > 0) {
				g_databaseobject=6;
				g_downloading=true;
			} else FrmAlert(NoUFOAlert);
			handled=true;
			break;
		case DatabaseDoneButton:
			FrmGotoForm(ServerForm);
			handled = true;
			break;
	}

	return handled;
}

Boolean DatabaseFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;
    UInt16 gadgetIndex;
    RectangleType bounds;
	MemHandle recH;
	netDBRecord *recP;
    UInt32 ticks;
    UInt16 unit;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return DatabaseFormDoButtonCommand(eventP->data.ctlSelect.controlID);
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
				DatabaseFormDrawProgressGadget();
				if(g_downloadindex >= 100) {
					g_downloading = false;
					DatabaseFormTakeObject(g_databaseobject);
					frmP = FrmGetActiveForm();
					switch(g_databaseobject) {
						case 1:
							CtlHideControl(GetObjectPtr(DatabaseSourceCodeButton));
							gadgetIndex = FrmGetObjectIndex(frmP, DatabaseProgressGadget);
							FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
							WinEraseRectangle(&bounds, 1);
							g_prefs.score += SCORE_SOURCE_CODE;
							g_prefs.source++;
							break;
						case 2:
							CtlHideControl(GetObjectPtr(DatabasePayrollButton));
							gadgetIndex = FrmGetObjectIndex(frmP, DatabaseProgressGadget);
							FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
							WinEraseRectangle(&bounds, 1);
							g_prefs.score += SCORE_PAYROLL;
							g_prefs.payroll++;
							break;
						case 3:
							CtlHideControl(GetObjectPtr(DatabaseUFOButton));
							gadgetIndex = FrmGetObjectIndex(frmP, DatabaseProgressGadget);
							FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
							WinEraseRectangle(&bounds, 1);
							g_prefs.score += SCORE_UFO;
							g_prefs.ufo++;
							// ufo data downloaded send email 10
							AHDBAddEmailToDatabase(10);
							break;
						case 4:
							CtlHideControl(GetObjectPtr(DatabaseUploadSourceButton));
							gadgetIndex = FrmGetObjectIndex(frmP, DatabaseProgressGadget);
							FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
							WinEraseRectangle(&bounds, 1);
							g_prefs.score += SCORE_UPLOAD_SOURCE;
							g_prefs.source--;
							DatabaseFormTakeCredits();
							// if this is sourcef.net then send email 8
							recH = DmQueryRecord(gNetDB, g_server);
							if(recH) {
								recP = MemHandleLock(recH);
								if(!StrCompare(recP->name, "sourcef.net")) 
									AHDBAddEmailToDatabase(8);
								MemHandleUnlock(recH);
							}
							break;
						case 5:
							CtlHideControl(GetObjectPtr(DatabaseUploadPayrollButton));
							gadgetIndex = FrmGetObjectIndex(frmP, DatabaseProgressGadget);
							FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
							WinEraseRectangle(&bounds, 1);
							g_prefs.score += SCORE_UPLOAD_PAYROLL;
							g_prefs.payroll--;
							DatabaseFormTakeCredits();
							// if this is newgrounds.com then send email 9
							recH = DmQueryRecord(gNetDB, g_server);
							if(recH) {
								recP = MemHandleLock(recH);
								if(!StrCompare(recP->name, "newgrounds.com")) 
									AHDBAddEmailToDatabase(9);
								MemHandleUnlock(recH);
							}
							break;
						case 6:
							CtlHideControl(GetObjectPtr(DatabaseUploadUFOButton));
							gadgetIndex = FrmGetObjectIndex(frmP, DatabaseProgressGadget);
							FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
							WinEraseRectangle(&bounds, 1);
							g_prefs.score += SCORE_UPLOAD_UFO;
							g_prefs.ufo--;
							DatabaseFormTakeCredits();
							AHDBAddEmailToDatabase(11);  // send winning email
							break;
					}
				}
				g_downloadindex += 2;
			}
			break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            DatabaseFormInit(frmP);
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