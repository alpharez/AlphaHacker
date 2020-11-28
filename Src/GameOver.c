#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

UInt16 g_gameovertime;

static void GameOverFormInit(FormType *frmP) {

}

Boolean GameOverFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    //FieldType *fldP;

    switch (eventP->eType) 
        {
        case frmOpenEvent:
        	g_gameovertime = 0;
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            GameOverFormInit(frmP);
            handled = true;
            break;
            
		case nilEvent:
			if(g_gameovertime == 30) FrmGotoForm(MainForm);
			g_gameovertime++;
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