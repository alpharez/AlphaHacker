#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void WelcomeFormInit(FormType *frmP) {
}

static void WelcomeFormSetUsername() {
	FieldType *fldUsername;
	MemHandle usernameH;
	Char *username;
	
	fldUsername = GetObjectPtr(WelcomeUsernameField);
	usernameH = FldGetTextHandle(fldUsername);
	if(usernameH) {
		username = MemHandleLock(usernameH);
		StrCopy(g_prefs.username, username);
	}
}

static Boolean WelcomeFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case WelcomeLogonButton:
			WelcomeFormSetUsername();
			FrmGotoForm(MainForm);
			handled = true;
			break;
	}

	return handled;
}

Boolean WelcomeFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return WelcomeFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            WelcomeFormInit(frmP);
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