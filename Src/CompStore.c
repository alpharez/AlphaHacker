#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void CompStoreFormFreePointers() {
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(CompStoreCurCPUField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(CompStoreCurRamField)));
}

static void CompStoreFormDisplayCPU() {
	FieldType *fldCPU;
	Char *cpu;
	
	cpu = FldGetTextPtr(GetObjectPtr(CompStoreCurCPUField));
	if(!cpu) cpu = MemPtrNew(20);
	
	switch(g_prefs.cpu) {
		case CPU200MHZ:
			StrCopy(cpu, "200 MHZ CPU");
			break;
		case CPU300MHZ:
			StrCopy(cpu, "300 MHZ CPU");
			break;
		case CPU400MHZ:
			StrCopy(cpu, "400 MHZ CPU");
			break;
		case CPU500MHZ:
			StrCopy(cpu, "500 MHZ CPU");
			break;
		case CPU650MHZ:
			StrCopy(cpu, "650 MHZ CPU");
			break;
		case CPU900MHZ:
			StrCopy(cpu, "900 MHZ CPU");
			break;
		case CPU1GHZ:
			StrCopy(cpu, "1 GHZ CPU");
			break;
		case CPU2GHZ:
			StrCopy(cpu, "2 GHZ CPU");
			break;
		case CPU35GHZ:
			StrCopy(cpu, "3.5 GHZ CPU");
			break;
		default:
			StrCopy(cpu, "CPU ERROR");
			break;
	}
	fldCPU = GetObjectPtr(CompStoreCurCPUField);
	FldSetTextPtr(fldCPU, cpu);
	FldDrawField(fldCPU);
}

static void CompStoreFormDisplayRAM() {
	FieldType *fldRAM;
	Char *ram;
	
	ram = FldGetTextPtr(GetObjectPtr(CompStoreCurRamField));
	if(!ram) ram = MemPtrNew(20);

	switch(g_prefs.ram) {
		case RAM32MB:
			StrCopy(ram, "32MB RAM");
			break;
		case RAM64MB:
			StrCopy(ram, "64MB RAM");
			break;
		case RAM128MB:
			StrCopy(ram, "128MB RAM");
			break;
		case RAM384MB:
			StrCopy(ram, "384MB RAM");
			break;
		case RAM512MB:
			StrCopy(ram, "512MB RAM");
			break;
		case RAM750MB:
			StrCopy(ram, "750MB RAM");
			break;
		case RAM1GB:
			StrCopy(ram, "1 GigaByte");
			break;
		case RAM2GB:
			StrCopy(ram, "2 GigaBytes");
			break;
		case RAM5GB:
			StrCopy(ram, "5 GigaBytes!");
			break;
		default:
			StrCopy(ram, "RAM ERROR!");
			break;
	}
	fldRAM = GetObjectPtr(CompStoreCurRamField);
	FldSetTextPtr(fldRAM, ram);
	FldDrawField(fldRAM);
}

static void CompStoreFormInit(FormType *frmP) {
	CompStoreFormDisplayCPU();
	CompStoreFormDisplayRAM();
}


static void CompStoreFormBuyCPU() {
	switch(LstGetSelection(GetObjectPtr(CompStoreCPUList))) {
		case 0: // 300 Mhz
			if((g_prefs.credits >= 500) && (g_prefs.cpu < CPU300MHZ)) { 
				g_prefs.credits -= 500;
				g_prefs.cpu = CPU300MHZ;
				CompStoreFormDisplayCPU();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 1: // 400 Mhz
			if((g_prefs.credits >= 600) && (g_prefs.cpu < CPU400MHZ)) { 
				g_prefs.credits -= 600;
				g_prefs.cpu = CPU400MHZ;
				CompStoreFormDisplayCPU();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 2: // 500 Mhz
			if((g_prefs.credits >= 700) && (g_prefs.cpu < CPU500MHZ)) { 
				g_prefs.credits -= 700;
				g_prefs.cpu = CPU500MHZ;
				CompStoreFormDisplayCPU();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 3: // 650 Mhz
			if((g_prefs.credits >= 900) && (g_prefs.cpu < CPU650MHZ)) { 
				g_prefs.credits -= 900;
				g_prefs.cpu = CPU650MHZ;
				CompStoreFormDisplayCPU();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 4: // 900 Mhz
			if((g_prefs.credits >= 1100) && (g_prefs.cpu < CPU900MHZ)) { 
				g_prefs.credits -= 1100;
				g_prefs.cpu = CPU900MHZ;
				CompStoreFormDisplayCPU();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 5: // 1 Ghz
			if((g_prefs.credits >= 1500) && (g_prefs.cpu < CPU1GHZ)) {
				g_prefs.credits -= 1500;
				g_prefs.cpu = CPU1GHZ;
				CompStoreFormDisplayCPU();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 6: // 2 Ghz
			if((g_prefs.credits >= 4000) && (g_prefs.cpu < CPU2GHZ)) { 
				g_prefs.credits -= 4000;
				g_prefs.cpu = CPU2GHZ;
				CompStoreFormDisplayCPU();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 7: // 3.5 Ghz
			if((g_prefs.credits >= 9000) && (g_prefs.cpu < CPU35GHZ)) {
				g_prefs.credits -= 9000;
				g_prefs.cpu = CPU35GHZ;
				CompStoreFormDisplayCPU();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
	}
}

static void CompStoreFormBuyRAM() {
	switch(LstGetSelection(GetObjectPtr(CompStoreRAMList))) {
		case 0: // 64 MB
			if((g_prefs.credits >= 100) && (g_prefs.ram < RAM64MB)) {
				g_prefs.credits -= 100;
				g_prefs.ram = RAM64MB;
				CompStoreFormDisplayRAM();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 1: // 128 MB
			if((g_prefs.credits >= 250) && (g_prefs.ram < RAM128MB)) {
				g_prefs.credits -= 250;
				g_prefs.ram = RAM128MB;
				CompStoreFormDisplayRAM();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 2: // 384 MB
			if((g_prefs.credits >= 500) && (g_prefs.ram < RAM384MB)) {
				g_prefs.credits -= 500;
				g_prefs.ram = RAM384MB;
				CompStoreFormDisplayRAM();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 3: // 512 MB
			if((g_prefs.credits >= 1000) && (g_prefs.ram < RAM512MB)) {
				g_prefs.credits -= 1000;
				g_prefs.ram = RAM512MB;
				CompStoreFormDisplayRAM();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 4: // 750 MB
			if((g_prefs.credits >= 2000) && (g_prefs.ram < RAM750MB)) {
				g_prefs.credits -= 2000;
				g_prefs.ram = RAM750MB;
				CompStoreFormDisplayRAM();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 5: // 1 GB
			if((g_prefs.credits >= 5000) && (g_prefs.ram < RAM1GB)) {
				g_prefs.credits -= 5000;
				g_prefs.ram = RAM1GB;
				CompStoreFormDisplayRAM();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 6: // 2 GB
			if((g_prefs.credits >= 10000) && (g_prefs.ram < RAM2GB)) {
				g_prefs.credits -= 10000;
				g_prefs.ram = RAM2GB;
				CompStoreFormDisplayRAM();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
		case 7: // 5 GB
			if((g_prefs.credits >= 20000) && (g_prefs.ram < RAM5GB)) {
				g_prefs.credits -= 20000;
				g_prefs.ram = RAM5GB;
				CompStoreFormDisplayRAM();
			} else FrmAlert(NotEnoughCreditsAlert);
			break;
	}
}

static Boolean CompStoreFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;
	
	switch(command) {
		case CompStoreOkButton:
			FrmGotoForm(MainForm);
			handled = true;
			break;
		case CompStoreBuyCPUButton:
			CompStoreFormBuyCPU();
			handled = true;
			break;
		case CompStoreBuyRAMButton:
			CompStoreFormBuyRAM();
			handled = true;
			break;
	}
	return handled;
}

Boolean CompStoreFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return CompStoreFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            CompStoreFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	CompStoreFormFreePointers();
        	break;

        default:
            break;
        }
    
    return handled;
}
