#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void ShutdownFormFreePointers() {
}

static void ShutdownFormInit(FormType *frmP) {
	g_servertimeindex = 0;
	g_shutdownitems = 0;
	g_shuttingdown = true;
}

static void ShutdownFormDoShutdown() {
	switch(g_shutdownitems) {
		case 0:
			WinDrawChars("shutting down httpd...[Done]", 28, 10, 30);
			break;
		case 1:
			WinDrawChars("shutting down nfsd....[Done]", 28, 10, 40);
			break;
		case 2:
			WinDrawChars("shutting down smbd....[Done]", 28, 10, 50);
			break;
		case 3:
			WinDrawChars("shutting down named...[Done]", 28, 10, 60);
			break;
		case 4:
			WinDrawChars("syncing file systems..[Done]", 28, 10, 70);
			break;
	}
}

Boolean ShutdownFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;
    UInt32 ticks;
    UInt16 unit;

    switch (eventP->eType) 
        {

		case nilEvent:
			if(g_shuttingdown) {
				ticks = TimGetTicks();
				unit = ticks - g_servertimeindex;
				if(unit >= (g_tickspersec)) {
					g_servertimeindex = TimGetTicks();
					// show items shutting down
					if(g_shutdownitems < 5) {
						ShutdownFormDoShutdown();
						g_shutdownitems++;
					} else {
						g_shuttingdown = false;
						//CtlShowControl(GetObjectPtr(ShutdownDoneButton));
						FrmGotoForm(MainForm);
					}
				}
			}
			break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            ShutdownFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	ShutdownFormFreePointers();
        	break;

        default:
            break;
        }
    return handled;
}