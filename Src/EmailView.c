#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

static void EmailViewFormFreePointers() {
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(EmailViewFromField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(EmailViewSubjectField)));
	FreeCharPointer(FldGetTextPtr(GetObjectPtr(EmailViewMessageField)));
}

static void EmailViewFormInit(FormType *frmP) {
	FieldType *fldFrom, *fldSubject, *fldMessage;
	Char *from, *subject, *message;
	MemHandle recH;
	emailDBRecord *recP;

	
	fldFrom = GetObjectPtr(EmailViewFromField);
	fldSubject = GetObjectPtr(EmailViewSubjectField);
	fldMessage = GetObjectPtr(EmailViewMessageField);
	
	from = MemPtrNew(25);
	subject = MemPtrNew(128);
	message = MemPtrNew(256);
	
	recH = DmQueryRecord(gEmailDB, g_emailRecNum);
	recP = MemHandleLock(recH);

	StrCopy(from, recP->from);
	StrCopy(subject, recP->subject);
	StrCopy(message, recP->message);

	MemHandleUnlock(recH);
	
	FldSetTextPtr(fldFrom, from);
	FldSetTextPtr(fldSubject, subject);
	FldSetTextPtr(fldMessage, message);
	
	FldRecalculateField(fldFrom, true);
	FldRecalculateField(fldSubject, true);
	FldRecalculateField(fldMessage, true);
}

static Boolean EmailViewFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;
	
	switch(command) {
		case EmailViewOkButton:
			FrmGotoForm(MainForm);
			handled = true;
			break;
		case EmailViewDeleteButton:
			if(FrmAlert(DeleteEmailAlert) == 0) {
				DmRemoveRecord(gEmailDB, g_emailRecNum);
				FrmGotoForm(MainForm);
			}
			handled = true;
			break;
	}
	return handled;
}

Boolean EmailViewFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return EmailViewFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            EmailViewFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	EmailViewFormFreePointers();
        	break;

        default:
            break;
        }
    
    return handled;
}
