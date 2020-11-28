#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void BlackHatFormFreePointers() {
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(BlackHatOSField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(BlackHatScannerField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(BlackHatCrackerField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(BlackHatICEBreakerField)));
}

static void BlackHatFormDisplayOS() {
	FieldType *fldOS;
	Char *os;
	
	os = FldGetTextPtr(GetObjectPtr(BlackHatOSField));
	if(!os) os = MemPtrNew(20);
	
	switch(g_prefs.os) {
		case OSZDOS:
			StrCopy(os, "ZDOS");
			break;
		case OSWINBLOWS:
			StrCopy(os, "Winblows 95");
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
			StrCopy(os, "NO OS!");
			break;
	}
	fldOS = GetObjectPtr(BlackHatOSField);
	FldSetTextPtr(fldOS, os);
	FldDrawField(fldOS);
}

static void BlackHatFormDisplayScanner() {
	FieldType *fldScanner;
	Char *scanner;
	
	scanner = FldGetTextPtr(GetObjectPtr(BlackHatScannerField));
	if(!scanner) scanner = MemPtrNew(20);
	
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
	fldScanner = GetObjectPtr(BlackHatScannerField);
	FldSetTextPtr(fldScanner, scanner);
	FldDrawField(fldScanner);
}

static void BlackHatFormDisplayCracker() {
	FieldType *fldCracker;
	Char *cracker;
	
	cracker = FldGetTextPtr(GetObjectPtr(BlackHatCrackerField));
	if(!cracker) cracker = MemPtrNew(20);
	
	switch(g_prefs.cracker) {
		case CRACKERPASSCRACK:
			StrCopy(cracker, "Pass Crack");
			break;
		case CRACKERL337CRACK:
			StrCopy(cracker, "L337 Crack");
			break;
		case CRACKERSUPERNOVA:
			StrCopy(cracker, "SuperNova");
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
	fldCracker = GetObjectPtr(BlackHatCrackerField);
	FldSetTextPtr(fldCracker, cracker);
	FldDrawField(fldCracker);
}

static void BlackHatFormDisplayICE() {
	FieldType *fldICE;
	Char *ice;
	
	ice = FldGetTextPtr(GetObjectPtr(BlackHatICEBreakerField));
	if(!ice) ice = MemPtrNew(20);
	
	switch(g_prefs.ice) {
		case ICETECHICE:
			StrCopy(ice, "Tech ICE");
			break;
		case ICEGOVTICE:
			StrCopy(ice, "Gov't ICE");
			break;
		default:
			StrCopy(ice, "NO ICE");
			break;
	}
	fldICE = GetObjectPtr(BlackHatICEBreakerField);
	FldSetTextPtr(fldICE, ice);
	FldDrawField(fldICE);
}

static void BlackHatFormInit(FormType *frmP) {
	BlackHatFormDisplayOS();
	BlackHatFormDisplayScanner();
	BlackHatFormDisplayCracker();
	BlackHatFormDisplayICE();
}

static void BlackHatFormBuyOS() {
	switch(LstGetSelection(GetObjectPtr(BlackHatOSList))) {
		case 0: // Winblows 95
			if(g_prefs.ram >= RAM64MB) { 
				if((g_prefs.credits >= 95) && (g_prefs.os < OSWINBLOWS)) { 
					g_prefs.credits -= 95;
					g_prefs.os = OSWINBLOWS;
					BlackHatFormDisplayOS();
				} else FrmAlert(NotEnoughCreditsAlert);
			} else FrmCustomAlert(SWIncompatibleAlert, "200Mhz", "64MB", "ZDOS");
			break;
		case 1: // BlackHat 1.0
			if(registered) {
				if((g_prefs.ram >= RAM128MB) && (g_prefs.os >= OSWINBLOWS) && (g_prefs.cpu >= CPU300MHZ)) {
					if((g_prefs.credits >= 200) && (g_prefs.os < OSBLACKHAT1)) { 
						g_prefs.credits -= 200;
						g_prefs.os = OSBLACKHAT1;
						BlackHatFormDisplayOS();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "300Mhz", "128MB", "Winblows");
			} else FrmAlert(RegisterAlert);
			break;
		case 2: // BlackHat 2.0
			if(registered) {
				if((g_prefs.ram >= RAM384MB) && (g_prefs.os >= OSBLACKHAT1) && (g_prefs.cpu >= CPU300MHZ)) {
					if((g_prefs.credits >= 700) && (g_prefs.os < OSBLACKHAT2)) { 
						g_prefs.credits -= 700;
						g_prefs.os = OSBLACKHAT2;
						BlackHatFormDisplayOS();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "300Mhz", "384MB", "BlackHat 1.0");
			} else FrmAlert(RegisterAlert);
			break;
		case 3: // BlackHat Pro
			if(registered) {
				if((g_prefs.ram >= RAM512MB) && (g_prefs.os >= OSBLACKHAT2) && (g_prefs.cpu >= CPU400MHZ)) {
					if((g_prefs.credits >= 2000) && (g_prefs.os < OSBLACKHATPRO)) { 
						g_prefs.credits -= 2000;
						g_prefs.os = OSBLACKHATPRO;
						BlackHatFormDisplayOS();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "400Mhz", "512MB", "BlackHat 2.0");
			} else FrmAlert(RegisterAlert);
			break;
		case 4: // Kornix
			if(registered) {
				if((g_prefs.ram >= RAM1GB) && (g_prefs.os >= OSBLACKHATPRO) && (g_prefs.cpu >= CPU500MHZ)) {
					if((g_prefs.credits >= 10000) && (g_prefs.os < OSKORNIX)) { 
						g_prefs.credits -= 10000;
						g_prefs.os = OSKORNIX;
						BlackHatFormDisplayOS();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "500Mhz", "1GB", "BlackHat Pro");
			} else FrmAlert(RegisterAlert);
			break;
	}
}

static void BlackHatFormBuyScanner() {
	switch(LstGetSelection(GetObjectPtr(BlackHatScannerList))) {
		case 0: // Snoop
			if(registered) {
				if((g_prefs.ram >= RAM384MB) && (g_prefs.os >= OSBLACKHAT1) && (g_prefs.cpu >= CPU400MHZ)) {
					if((g_prefs.credits >= 500) && (g_prefs.scanner < SCANNERSNOOP)) { 
						g_prefs.credits -= 500;
						g_prefs.scanner = SCANNERSNOOP;
						BlackHatFormDisplayScanner();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "400Mhz", "384MB", "BlackHat 1.0");
			} else FrmAlert(RegisterAlert);
			break;
		case 1: // Sniff
			if(registered) {
				if((g_prefs.ram >= RAM512MB) && (g_prefs.os >= OSBLACKHAT2) && (g_prefs.cpu >= CPU500MHZ)) {
					if((g_prefs.credits >= 2000) && (g_prefs.scanner < SCANNERSNIFF2)) { 
						g_prefs.credits -= 2000;
						g_prefs.scanner = SCANNERSNIFF2;
						BlackHatFormDisplayScanner();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "500Mhz", "512MB", "BlackHat 2.0");
			} else FrmAlert(RegisterAlert);
			break;
		case 2: // SuperSniff
			if(registered) {
				if((g_prefs.ram >= RAM750MB) && (g_prefs.os >= OSBLACKHAT2) && (g_prefs.cpu >= CPU900MHZ)) {
					if((g_prefs.credits >= 20000) && (g_prefs.scanner < SCANNERSUPERSNIFF)) { 
						g_prefs.credits -= 20000;
						g_prefs.scanner = SCANNERSUPERSNIFF;
						BlackHatFormDisplayScanner();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "900Mhz", "750MB", "BlackHat 2.0");
			} else FrmAlert(RegisterAlert);
			break;
	}
}

static void BlackHatFormBuyCracker() {
	switch(LstGetSelection(GetObjectPtr(BlackHatCrackerList))) {
		case 0: // PassCrack
			if((g_prefs.ram >= RAM128MB) && (g_prefs.os >= OSWINBLOWS) && (g_prefs.cpu >= CPU300MHZ)) {
				if((g_prefs.credits >= 200) && (g_prefs.cracker < CRACKERPASSCRACK)) { 
					g_prefs.credits -= 200;
					g_prefs.cracker = CRACKERPASSCRACK;
					BlackHatFormDisplayCracker();
				} else FrmAlert(NotEnoughCreditsAlert);
			} else FrmCustomAlert(SWIncompatibleAlert, "300Mhz", "128MB", "Winblows");
			break;
		case 1: // L337Crack
			if(registered) {
				if((g_prefs.ram >= RAM384MB) && (g_prefs.os >= OSBLACKHAT1) && (g_prefs.cpu >= CPU400MHZ)) {
					if((g_prefs.credits >= 1000) && (g_prefs.cracker < CRACKERL337CRACK)) { 
						g_prefs.credits -= 1000;
						g_prefs.cracker = CRACKERL337CRACK;
						BlackHatFormDisplayCracker();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "400Mhz", "384MB", "BlackHat 1.0");
			} else FrmAlert(RegisterAlert);
			break;
		case 2: // SuperNova
			if(registered) {
				if((g_prefs.ram >= RAM512MB) && (g_prefs.os >= OSBLACKHAT2) && (g_prefs.cpu >= CPU500MHZ)) {
					if((g_prefs.credits >= 10000) && (g_prefs.cracker < CRACKERSUPERNOVA)) { 
						g_prefs.credits -= 10000;
						g_prefs.cracker = CRACKERSUPERNOVA;
						BlackHatFormDisplayCracker();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "500Mhz", "512MB", "BlackHat 2.0");
			} else FrmAlert(RegisterAlert);
			break;
		case 3: // DES Crack
			if(registered) {
				if((g_prefs.ram >= RAM1GB) && (g_prefs.os >= OSBLACKHATPRO) && (g_prefs.cpu >= CPU1GHZ)) {
					if((g_prefs.credits >= 50000) && (g_prefs.cracker < CRACKERDESCRACK)) { 
						g_prefs.credits -= 50000;
						g_prefs.cracker = CRACKERDESCRACK;
						BlackHatFormDisplayCracker();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "1Ghz", "1GB", "BlackHat Pro");
			} else FrmAlert(RegisterAlert);
			break;
		case 4: // Quanta
			if(registered) {
				if((g_prefs.ram >= RAM2GB) && (g_prefs.os >= OSKORNIX) && (g_prefs.cpu >= CPU2GHZ)) {
					if((g_prefs.credits >= 200000) && (g_prefs.cracker < CRACKERQUANTA)) { 
						g_prefs.credits -= 200000;
						g_prefs.cracker = CRACKERQUANTA;
						BlackHatFormDisplayCracker();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "2Ghz", "2GB", "Kornix");
			} else FrmAlert(RegisterAlert);
			break;
	}
}

static void BlackHatFormBuyICE() {
	switch(LstGetSelection(GetObjectPtr(BlackHatICEBreakerList))) {
		case 0: // Tech ICE
			if(registered) {
				if((g_prefs.ram >= RAM2GB) && (g_prefs.os >= OSKORNIX) && (g_prefs.cpu >= CPU2GHZ)) {
					if((g_prefs.credits >= 25000) && (g_prefs.ice < ICETECHICE)) { 
						g_prefs.credits -= 25000;
						g_prefs.ice = ICETECHICE;
						BlackHatFormDisplayICE();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "2Ghz", "2GB", "Kornix");
			} else FrmAlert(RegisterAlert);
			break;
		case 1: // Gov't ICE
			if(registered) {
				if((g_prefs.ram >= RAM5GB) && (g_prefs.os >= OSKORNIX) && (g_prefs.cpu >= CPU35GHZ)) {
					if((g_prefs.credits >= 150000) && (g_prefs.ice < ICEGOVTICE)) { 
						g_prefs.credits -= 150000;
						g_prefs.ice = ICEGOVTICE;
						BlackHatFormDisplayICE();
					} else FrmAlert(NotEnoughCreditsAlert);
				} else FrmCustomAlert(SWIncompatibleAlert, "3.5Ghz", "5GB", "Kornix");
			} else FrmAlert(RegisterAlert);
			break;
	}
}

static Boolean BlackHatFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;
	
	switch(command) {
		case BlackHatOKButton:
			FrmGotoForm(MainForm);
			handled = true;
			break;
		case BlackHatBuyOSButton:
			BlackHatFormBuyOS();
			handled = true;
			break;
		case BlackHatBuyScannerButton:
			BlackHatFormBuyScanner();
			handled = true;
			break;
		case BlackHatBuyCrackerButton:
			BlackHatFormBuyCracker();
			handled = true;
			break;
		case BlackHatBuyICEButton:
			BlackHatFormBuyICE();
			handled = true;
			break;
	}
	return handled;
}

Boolean BlackHatFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;

    switch (eventP->eType) 
        {
        
        case ctlSelectEvent:
        	return BlackHatFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            BlackHatFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	BlackHatFormFreePointers();
        	break;

        default:
            break;
        }
    
    return handled;
}
