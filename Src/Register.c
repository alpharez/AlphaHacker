#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"
#include "DLServer.h"

static void RegisterFormInit(FormType *frmP) {
    char *code,name[dlkUserNameBufSize];

    DlkGetSyncInfo(NULL, NULL, NULL, name, NULL, NULL);
    if(!name[0]) {
        StrCopy(name, "Emulator");
    }

    code = (char *)regCode(name, 0);
    WinDrawChars(name, StrLen(name), 7, 52);
    WinDrawChars(code,StrLen(code),7,80);
    PrintRegistered();

}

Char EvalKey(UInt16 key,Char *data) {
    unsigned char c;
    UInt16 i,unlockKey = 0, length;	

    length = StrLen((char *)data);

    for (i=0;i<length;i++) {
        c = data[i];
        unlockKey = ALGORITHM(unlockKey, i, c);
    }
    unlockKey &= 0xFFFF;  /* Mask off bits outside range of 0..65535 */

    return (unlockKey == key);
}

Char RegisterFormIsRegistered() {
	Char *data;
	Char name[dlkUserNameBufSize];
	
	DlkGetSyncInfo(NULL, NULL, NULL, name, NULL, NULL);
	if(!name[0]) {
		StrCopy(name, "Emulator");
	}
	data = regCode(name, 1);
	return EvalKey(g_prefs.key, data);
}

static void RegisterFormDoRegister() {
	FormType *frmP;
	FieldType *fldP;
	Int16 fldIndex;
	MemHandle keyH;
	Char *keyString, name[dlkUserNameBufSize];
	Char *data;
	
	frmP = FrmGetActiveForm();
	fldIndex = FrmGetObjectIndex(frmP, RegisterKeyField);
	fldP = FrmGetObjectPtr(frmP, fldIndex);
	keyH = FldGetTextHandle(fldP);
	if(keyH) {
		keyString = MemHandleLock(keyH);
		g_prefs.key = StrAToI(keyString);
		DlkGetSyncInfo(NULL, NULL, NULL, name, NULL, NULL);
		if(!name[0]) {
			StrCopy(name, "Emulator");
		}
		data = regCode(name, 1);
		registered = EvalKey(g_prefs.key, data);
		PrintRegistered();
		MemHandleUnlock(keyH);
	}
}

Char *regCode(Char *user, Boolean nameOnly) {
    static char name[11];
    static char hex[] = "0123456789ABCDEF";
    int len;
    int c=0, n=0, idx=0, i;
    unsigned char checkSum = 0;

    len=StrLen((const char *)user);

    for(i=0;i<MIN(len,10);i++) {
	junk[c++] = hex[user[idx]/16];
	junk[c++] = hex[user[idx]%16];
	junk[c++] = ':';
	name[n++] = user[idx];
	checkSum += user[idx++];
	if(len > 10 && idx == 5)
	    idx = len - 5;
    }

    junk[c++] = hex[checkSum/16];
    junk[c++] = hex[checkSum%16];
    junk[c] = '\0';
    name[n] = '\0';

    if(nameOnly)
        return name;
    else
        return junk;
}


/*****************************************************************************
* void PrintRegistered()
* Prints "Registered" or "Unregistered" text at top of screen.
*****************************************************************************/
void PrintRegistered() {
    RectangleType rect;

    rect.topLeft.x = 40;
    rect.topLeft.y = 16;
    rect.extent.x = 100;
    rect.extent.y = 9;
    WinEraseRectangle(&rect, 0);

    if (registered) {
        WinDrawChars("Registered",10,60,16);
    } else {
        WinDrawChars("Unregistered",12,60,16);
    }
}

static Boolean RegisterFormDoButtonCommand(UInt16 command) {
	Boolean handled = false;

	switch(command) {
		case RegisterRegisterButton:
			RegisterFormDoRegister();
			handled = true;
			break;
		case RegisterDoneButton:
			FrmGotoForm(MainForm);
			handled = true;
			break;
	}

	return handled;
}

Boolean RegisterFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;

    switch (eventP->eType) 
        {
        case ctlSelectEvent:
        	return RegisterFormDoButtonCommand(eventP->data.ctlSelect.controlID);
        	break;

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            RegisterFormInit(frmP);
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