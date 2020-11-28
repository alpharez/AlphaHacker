#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void UserFormFreePointers() {
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(UserHandleField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(UserCPUField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(UserRAMField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(UserOSField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(UserCrackedField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(UserRankField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(UserScoreField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(UserSourceField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(UserPayrollField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(UserUFOField)));	
}

static void UserFormInit(FormType *frmP) {
	Char *handle, *cpu, *ram, *os, *cracked, *rank, *score;
	Char *source, *payroll, *ufo;
	
	handle = MemPtrNew(20);
	cpu = MemPtrNew(20);
	ram = MemPtrNew(20);
	os = MemPtrNew(20);
	cracked = MemPtrNew(20);
	rank = MemPtrNew(20);
	score = MemPtrNew(20);
	source = MemPtrNew(10);
	payroll = MemPtrNew(10);
	ufo = MemPtrNew(10);
	
	switch(g_prefs.cpu) {
		case CPU200MHZ:
			StrCopy(cpu, "200Mhz");
			break;
		case CPU300MHZ:
			StrCopy(cpu, "300Mhz");
			break;
		case CPU400MHZ:
			StrCopy(cpu, "400Mhz");
			break;
		case CPU500MHZ:
			StrCopy(cpu, "500Mhz");
			break;
		case CPU650MHZ:
			StrCopy(cpu, "650Mhz");
			break;
		case CPU900MHZ:
			StrCopy(cpu, "900Mhz");
			break;
		case CPU1GHZ:
			StrCopy(cpu, "1Ghz");
			break;
		case CPU2GHZ:
			StrCopy(cpu, "2Ghz");
			break;
		case CPU35GHZ:
			StrCopy(cpu, "3.5Ghz");
			break;
		default:
			StrCopy(cpu, "ERROR");
			break;
	}
	switch(g_prefs.ram) {
		case RAM32MB:
			StrCopy(ram, "32MB");
			break;
		case RAM64MB:
			StrCopy(ram, "64MB");
			break;
		case RAM128MB:
			StrCopy(ram, "128MB");
			break;
		case RAM384MB:
			StrCopy(ram, "384MB");
			break;
		case RAM512MB:
			StrCopy(ram, "512MB");
			break;
		case RAM750MB:
			StrCopy(ram, "750MB");
			break;
		case RAM1GB:
			StrCopy(ram, "1 GB");
			break;
		case RAM2GB:
			StrCopy(ram, "2 GB");
			break;
		case RAM5GB:
			StrCopy(ram, "5 GB");
			break;
		default:
			StrCopy(ram, "ERR");
			break;
	}
	switch(g_prefs.os) {
		case OSZDOS:
			StrCopy(os, "ZDOS");
			break;
		case OSWINBLOWS:
			StrCopy(os, "Win 95");
			break;
		case OSBLACKHAT1:
			StrCopy(os, "BlackHat 1.0");
			break;
		case OSBLACKHAT2:
			StrCopy(os, "BlackHat 2.0");
			break;
		case OSBLACKHATPRO:
			StrCopy(os, "BlackHat Pro");
			break;
		case OSKORNIX:
			StrCopy(os, "Kornix");
			break;
		default:
			StrCopy(os, "ERROR!");
			break;
	}
	StrCopy(handle, g_prefs.username);
	if(g_prefs.numCracked > 0)
		StrIToA(cracked, g_prefs.numCracked);
	else
		StrCopy(cracked, "None");
		
	if(g_prefs.score < SCRIPTKIDDIE)
		StrCopy(rank, "Lamer");
	else if(g_prefs.score < CRACKER)
		StrCopy(rank, "Script Kiddie");
	else if(g_prefs.score < HACKER)
		StrCopy(rank, "Cracker");
	else if(g_prefs.score < ELITE_CRACKER)
		StrCopy(rank, "Hacker");
	else if(g_prefs.score < ELITE_HACKER)
		StrCopy(rank, "Elite Cracker");
	else if(g_prefs.score < SUPER_CODER)
		StrCopy(rank, "Elite Hacker");
	else if(g_prefs.score < KING_GURU)
		StrCopy(rank, "Super Coder");
	else StrCopy(rank, "King Guru!");
	
	StrIToA(score, g_prefs.score);
	StrIToA(source, g_prefs.source);
	StrIToA(payroll, g_prefs.payroll);
	StrIToA(ufo, g_prefs.ufo);
	
	FldSetTextPtr(GetObjectPtr(UserHandleField), handle);
	FldSetTextPtr(GetObjectPtr(UserCPUField), cpu);
	FldSetTextPtr(GetObjectPtr(UserRAMField), ram);
	FldSetTextPtr(GetObjectPtr(UserOSField), os);
	FldSetTextPtr(GetObjectPtr(UserCrackedField), cracked);
	FldSetTextPtr(GetObjectPtr(UserRankField), rank);
	FldSetTextPtr(GetObjectPtr(UserScoreField), score);
	FldSetTextPtr(GetObjectPtr(UserSourceField), source);
	FldSetTextPtr(GetObjectPtr(UserPayrollField), payroll);
	FldSetTextPtr(GetObjectPtr(UserUFOField), ufo);
	FrmDrawForm(FrmGetActiveForm());
}

static Boolean UserFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;
	
	switch(command) {
		case UserOkButton:
			FrmGotoForm(MainForm);
			handled = true;
			break;
	}
	
	return handled;
}

Boolean UserFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return UserFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            UserFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	UserFormFreePointers();
        	break;

        default:
            break;
        }
    
    return handled;
}
