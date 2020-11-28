#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

Int16 g_crackindex=0;
Int16 g_crackmax=0;
Int16 g_cracktimeindex=0;

static void CrackFormFreePointers() {
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(CrackOutput1Field)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(CrackOutput2Field)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(CrackOutput3Field)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(CrackOutput4Field)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(CrackOutput5Field)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(CrackOutput6Field)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(CrackOutput7Field)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(CrackOutput8Field)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(CrackTimerField)));
	FreeCharPointer((char *)FrmGetTitle(FrmGetActiveForm()));
}

static Boolean CrackFormCheckPassChar() {
	MemHandle recordH;
	netDBRecord *recP;
	Char *outP;
	Boolean match = false;
	
	switch(g_crackpos) {
		case 0:
			outP = FldGetTextPtr(GetObjectPtr(CrackOutput1Field));
			break;
		case 1:
			outP = FldGetTextPtr(GetObjectPtr(CrackOutput2Field));
			break;
		case 2:
			outP = FldGetTextPtr(GetObjectPtr(CrackOutput3Field));
			break;
		case 3:
			outP = FldGetTextPtr(GetObjectPtr(CrackOutput4Field));
			break;
		case 4:
			outP = FldGetTextPtr(GetObjectPtr(CrackOutput5Field));
			break;
		case 5:
			outP = FldGetTextPtr(GetObjectPtr(CrackOutput6Field));
			break;
		case 6:
			outP = FldGetTextPtr(GetObjectPtr(CrackOutput7Field));
			break;
		case 7:
			outP = FldGetTextPtr(GetObjectPtr(CrackOutput8Field));
			break;
		default:
			outP = FldGetTextPtr(GetObjectPtr(CrackOutput1Field));
			break;
	}
	recordH = DmQueryRecord(gNetDB, g_server);
	if(recordH) {
		recP = MemHandleLock(recordH);
		if(recP->passcode[g_crackpos] == outP[0]) match = true;
		MemHandleUnlock(recordH);
	}
	return match;
}

static Int16 CrackFormGetPassCodeSize() {
	MemHandle recordH;
	netDBRecord *recP;
	Int16 size=8;
	
	recordH = DmQueryRecord(gNetDB, g_server);
	if(recordH) {
		recP = MemHandleLock(recordH);
		size = StrLen(recP->passcode);
		MemHandleUnlock(recordH);
	}
	return size;
}

static void CrackFormShowOutput() {
	FieldType *fldOut;
	Char *out;
	
	if(!g_cracking) return;
	switch(g_crackpos) {
		case 0:
			fldOut = GetObjectPtr(CrackOutput1Field);
			break;
		case 1:
			fldOut = GetObjectPtr(CrackOutput2Field);
			break;
		case 2:
			fldOut = GetObjectPtr(CrackOutput3Field);
			break;
		case 3:
			fldOut = GetObjectPtr(CrackOutput4Field);
			break;
		case 4:
			fldOut = GetObjectPtr(CrackOutput5Field);
			break;
		case 5:
			fldOut = GetObjectPtr(CrackOutput6Field);
			break;
		case 6:
			fldOut = GetObjectPtr(CrackOutput7Field);
			break;
		case 7:
			fldOut = GetObjectPtr(CrackOutput8Field);
			break;
		default:
			fldOut = GetObjectPtr(CrackOutput1Field);
			break;
	}
	out = FldGetTextPtr(fldOut);
	if(out == NULL) out = MemPtrNew(2);
	if(out[0] >= 'z') out[0] = 'a';
	else if(out[0] == '*') out[0] = 'a';
	else if(g_crackpos <= g_crackposmax-1) out[0]++;
	if(CrackFormCheckPassChar()) g_crackpos++;
	if(g_crackpos >= g_crackposmax) g_cracking = false;
	FldSetTextPtr(fldOut, out);
	FldDrawField(fldOut);
}

static void CrackFormOutputReset() {
	FieldType *fldOut;
	Char *out[9];
	int i, j, i2;

	j=0;
	g_crackpos = 0;
	g_crackposmax = CrackFormGetPassCodeSize();
	// set characters to '*'
	for(i=CrackOutput1Field; i<=CrackOutput1Field+g_crackposmax-1; i++) {
		fldOut = GetObjectPtr(i);	
		out[j] = FldGetTextPtr(fldOut);
		if(!out[j]) out[j] = MemPtrNew(2);
		StrCopy(out[j], "*");
		FldSetTextPtr(fldOut, out[j]);
		FldDrawField(fldOut);
		j++;
	}
	// blank out remaining characters
	for(i2=i; i2<=CrackOutput8Field; i2++) {
		fldOut = GetObjectPtr(i2);	
		out[j] = FldGetTextPtr(fldOut);
		if(!out[j]) out[j] = MemPtrNew(2);
		StrCopy(out[j], " ");
		FldSetTextPtr(fldOut, out[j]);
		FldDrawField(fldOut);
		j++;	
	}		
}

static UInt16 CrackFormGetTimeout() {
	MemHandle recordH;
	netDBRecord *recP;
	UInt16 timeout = 20;
	
	recordH = DmQueryRecord(gNetDB, g_server);
	if(recordH) {
		recP = MemHandleLock(recordH);
		timeout = recP->timeout;
		MemHandleUnlock(recordH);
	}
	return timeout;
}

static void CrackFormShowTimeout() {
	FieldType *fldP;
	Char *timer;
	
	fldP = GetObjectPtr(CrackTimerField);
	timer = FldGetTextPtr(fldP);
	if(timer == NULL) timer = MemPtrNew(5);
	StrIToA(timer, g_timeout);
	FldSetTextPtr(fldP, timer);
	FldDrawField(fldP);
}

static void CrackFormInit(FormType *frmP) {
	Char *titleP;

	g_crackindex=0;
	g_crackmax=0;
	g_cracktimeindex=0;
	
	titleP = MemPtrNew(16);
	switch(g_prefs.cracker) {
		case CRACKERPASSCRACK:
			StrCopy(titleP, "PassCrack");
			g_crackmax=7;
			break;
		case CRACKERL337CRACK:
			StrCopy(titleP, "L337Crack");
			g_crackmax=5;
			break;
		case CRACKERSUPERNOVA:
			StrCopy(titleP, "SuperNova Crack");
			g_crackmax=3;
			break;
		case CRACKERDESCRACK:
			StrCopy(titleP, "DES Crack");
			g_crackmax=2;
			break;
		case CRACKERQUANTA:
			StrCopy(titleP, "Quanta");
			g_crackmax=1;
			break;
		default:
			StrCopy(titleP, "Cracker");
			g_crackmax=15;
			break;
	}
	FrmSetTitle(frmP, titleP);
	CrackFormOutputReset();
}

static void CrackFormGetServer() {
	MemHandle addrH;
	Char *addrP;
	
	addrH = FldGetTextHandle(GetObjectPtr(CrackAddressField));
	if(addrH) {
		addrP = MemHandleLock(addrH);
		g_server = AHDBFindHostByAddr(addrP);
		MemHandleUnlock(addrH);
	} else g_server = NOSUCHRECORD;
}

static Boolean CrackFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case CrackCrackButton:
			CrackFormGetServer();
			CrackFormOutputReset();
			g_timeout = CrackFormGetTimeout();
			if(g_server != NOSUCHRECORD)
				g_cracking = true;
			handled = true;
			break;
		case CrackDisconnectButton:
			g_cracking = false;
			FrmGotoForm(MainForm);
			handled = true;
			break;
	}
	return handled;
}

Boolean CrackFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;
    UInt32 ticks;
    UInt16 unit;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return CrackFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

		case nilEvent:
			if(g_cracking) {
				if(g_crackindex == g_crackmax) {
					g_crackindex = 0;
					CrackFormShowOutput();
				}
				g_crackindex++;
				ticks = TimGetTicks();
				unit = ticks - g_cracktimeindex;
				if(unit >= g_tickspersec) {
					g_cracktimeindex = TimGetTicks();
					CrackFormShowTimeout();
					if(g_timeout <= 0) {
						g_cracking = false;
						g_prefs.gameover = true;
						FrmGotoForm(GameOverForm);  // caught!
					}
					g_timeout--;
				}
			}
			break;
			
        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            CrackFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	CrackFormFreePointers();
        	g_cracking = false;
        	break;

        default:
            break;
        }
    
    return handled;
}
