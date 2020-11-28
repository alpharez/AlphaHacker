#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

Int16 g_scanindex=0;
Int16 g_scanmax=0;
Int16 g_scantimeindex=0;
Int16 g_scantimemax=6;

static void ScanFormFreePointers() {
	UInt16 i;
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(ScanFoundField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(ScanTimeoutField)));
	FreeCharPointer((char *)FrmGetTitle(FrmGetActiveForm()));
	if (g_scanList) {
    	for (i=0; i < LstGetNumberOfItems(GetObjectPtr(ScanFoundHostsList)); i++)
        	MemPtrFree((MemPtr) g_scanList[i]);
        MemPtrFree((MemPtr) g_scanList);
    	g_scanList = 0;
	}
}

static Boolean ScanFormHasGovtICE(Int16 host) {
	Boolean hasGovtICE;
	MemHandle recordH;
	netDBRecord *recP;

	hasGovtICE = false;
	recordH = DmQueryRecord(gNetDB, host);
	if(recordH) {
		recP = MemHandleLock(recordH);
		if(recP->hasGovtICE) hasGovtICE = true;
		MemHandleUnlock(recordH);
	}
	return hasGovtICE;
}

static Boolean ScanFormCheckAddr(Char *addr) {
	Boolean good = false;
	Int16 host;
	host = AHDBFindHostByAddr(addr);
	if(host != NOSUCHRECORD) good = true;
	if(ScanFormHasGovtICE(host)) good = false;
	return good;
}

static void ScanFormAddAddr(Char *addr) {
	ListType *list;
	Int16 numItems;
	Int16 i;
	Char **scanList;
		
	list = GetObjectPtr(ScanFoundHostsList);
	numItems = LstGetNumberOfItems(list);
	if(numItems > 0) {
		scanList = (Char **) MemPtrNew((numItems+1) * sizeof(Char *));
		for(i=0; i<numItems; i++) {
			scanList[i] = MemPtrNew(9);
			StrCopy(scanList[i], g_scanList[i]);
		}
    	for (i=0; i < LstGetNumberOfItems(GetObjectPtr(ScanFoundHostsList)); i++)
        	MemPtrFree((MemPtr) g_scanList[i]);
        MemPtrFree((MemPtr) g_scanList);
        g_scanList = scanList;
	} else {
		g_scanList = (Char **) MemPtrNew(sizeof(Char *));
	}
	g_scanList[numItems] = MemPtrNew(9);
	StrCopy(g_scanList[numItems], addr);
	LstSetListChoices(list, g_scanList, numItems+1);
	LstDrawList(list);
}

static void ScanFormShowOutput() {
	FieldType *fldFound;
	MemHandle netH;
	Char *found, *temp, *net;
	
	fldFound = GetObjectPtr(ScanFoundField);	
	found = FldGetTextPtr(fldFound);
	netH = FldGetTextHandle(GetObjectPtr(ScanNetworkField));
	if(netH) {
		net = MemHandleLock(netH);
		if(!found) found = MemPtrNew(25);
		temp = MemPtrNew(9);
		StrIToA(temp, g_scanpos);
		StrCopy(found, net);
		StrCat(found, ".");
		StrCat(found, temp);
		MemHandleUnlock(netH);
		FldSetTextPtr(fldFound, found);
		FldDrawField(fldFound);
		MemPtrFree(temp);
		if(ScanFormCheckAddr(found)) ScanFormAddAddr(found);
	} //else g_scanning = false;
}

static UInt16 ScanFormGetTimeout() {
	UInt16 timeout = 100;
	
	return timeout;
}

static void ScanFormShowTimeout() {
	FieldType *fldP;
	Char *timer;
	
	fldP = GetObjectPtr(ScanTimeoutField);
	timer = FldGetTextPtr(fldP);
	if(timer == NULL) timer = MemPtrNew(5);
	StrIToA(timer, g_timeout);
	FldSetTextPtr(fldP, timer);
	FldDrawField(fldP);
}

static void ScanFormInit(FormType *frmP) {
	Char *titleP;

	titleP = MemPtrNew(16);
	switch(g_prefs.scanner) {
		case SCANNERSNOOP:
			StrCopy(titleP, "Snoop");
			g_scanmax=5;
			break;
		case SCANNERSNIFF2:
			StrCopy(titleP, "Sniff 2.0");
			g_scanmax=2;
			break;
		case SCANNERSUPERSNIFF:
			StrCopy(titleP, "Super Sniff");
			g_scanmax=1;
			break;
		default:
			StrCopy(titleP, "Scanner");
			g_scanmax=10;
			break;
	}
	FrmSetTitle(frmP, titleP);
	g_scanpos = 0;
	g_timeout = ScanFormGetTimeout();
}

static Boolean ScanFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case ScanScanButton:
			g_scanning = true;
			handled = true;
			break;
		case ScanStopButton:
			g_scanning = false;
			g_scanpos = 0;
			handled = true;
			break;
		case ScanExitButton:
			FrmGotoForm(MainForm);
			handled = true;
			break;
	}

	return handled;
}

Boolean ScanFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;
    UInt32 ticks;
    UInt16 unit;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return ScanFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

		case nilEvent:
			if(g_scanning) {
				if(g_scanindex == g_scanmax) {
					g_scanpos++;
					g_scanindex=0;
					ScanFormShowOutput();
				}
				g_scanindex++;
				ticks = TimGetTicks();
				unit = ticks - g_scantimeindex;
				if(unit >= g_tickspersec) {
					g_scantimeindex = TimGetTicks();
					ScanFormShowTimeout();
					if(g_timeout <= 0) {
						g_scanning = false;
						g_prefs.gameover = true;
						FrmGotoForm(GameOverForm);  // caught
					}
					g_timeout--;
				}
			}
			break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            ScanFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	ScanFormFreePointers();
        	g_scanning = false;
        	break;

        default:
            break;
        }
    
    return handled;
}