#include <PalmOS.h>

#include "AlphaHacker.h"
#include "AlphaHackerRsc.h"

// Global variables

AlphaHackerPreferenceType g_prefs;
UInt16 g_timer = 10;
UInt16 g_tickspersec;
Boolean g_cracking = false;
Boolean g_scanning = false;
DmOpenRef gNetDB;
DmOpenRef gEmailDB;
DmOpenRef gNSLookupDB;
Int16 g_server;
hostRecord g_hosts[NUMHOSTS];
emailDBRecord g_emails[NUMTOTEMAILS];
Int16 g_crackpos;
Int16 g_crackposmax;
Int16 g_scanpos;
Char **g_scanList = 0;
UInt16 g_NSLookupTopRecord = 0;
UInt16 g_EmailTopRecord = 0;
UInt16 g_emailRecNum;
UInt16 g_hostRecNum;
UInt16 g_timeout;
Char registered = 0;
Char junk[35];
UInt16 g_hostaddr[NUMHOSTS];
UInt16 g_servertimeindex;
UInt16 g_servertimeout;
Boolean g_downloading;
UInt16 g_downloadindex;
UInt16 g_databaseobject;
UInt32 g_SupportedDepths;
UInt16 g_shutdownitems;
Boolean g_shuttingdown;

// Internal Constants

// Define the minimum OS version we support
#define ourMinVersion    sysMakeROMVersion(3,0,0,sysROMStageDevelopment,0)
#define kPalmOS20Version sysMakeROMVersion(2,0,0,sysROMStageDevelopment,0)

// Internal Functions

void * GetObjectPtr(UInt16 objectID)
{
    FormType *frmP;

    frmP = FrmGetActiveForm();
    return FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, objectID));
}

void FreeCharPointer(Char *ptr) {
	if(ptr) MemPtrFree(ptr);
}

static Boolean SeekHostRecord (UInt16 *index, Int16 offset, Int16 direction)
{
	UInt16 category = 0;
    DmSeekRecordInCategory(gNSLookupDB, index, offset, direction, category);
    if (DmGetLastErr()) return (false);
 
    return (true);
}

static Boolean SeekEmailRecord (UInt16 *index, Int16 offset, Int16 direction)
{
	UInt16 category = 0;
    DmSeekRecordInCategory(gEmailDB, index, offset, direction, category);
    if (DmGetLastErr()) return (false);
 
    return (true);
}

#pragma mark ----------

static UInt16 MainFormNumberOfRows(TableType *table) {
	UInt16 rows, rowsInTable;
	UInt16 tableHeight;
	//FontID curFont;
	RectangleType r;
	
	rowsInTable = TblGetNumberOfRows(table);
	TblGetBounds(table, &r);
	tableHeight = r.extent.y;
	//curFont = FntSetFont(gHostsFont);
	rows = tableHeight / FntLineHeight();
	//FntSetFont(curFont);
	if(rows <= rowsInTable)
		return(rows);
	else
		return(rowsInTable);
}

void MainFormSetCredits(Int32 credits) {
	FormType *frmP;
	FieldType *fldP;
	Char *str;
	
	g_prefs.credits = credits;
	frmP = FrmGetFormPtr(MainForm);
	fldP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, MainCreditsField));
	str = FldGetTextPtr(fldP);
	if(str == NULL) // make new pointer
		str=MemPtrNew(20);
	StrIToA(str, g_prefs.credits);
	FldSetTextPtr(fldP, str);
	FldDrawField(fldP);
}

void MainFormDecCredits(Int32 credits) {
	FormType *frmP;
	FieldType *fldP;
	Char *str;
	
	g_prefs.credits -= credits;
	frmP = FrmGetFormPtr(MainForm);
	fldP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, MainCreditsField));
	str = FldGetTextPtr(fldP);
	if(str == NULL) // make new pointer
		str=MemPtrNew(20);
	StrIToA(str, g_prefs.credits);
	FldSetTextPtr(fldP, str);
	FldDrawField(fldP);
}

void MainFormIncCredits(Int32 credits) {
	FormType *frmP;
	FieldType *fldP;
	Char *str;
	
	g_prefs.credits += credits;
	frmP = FrmGetFormPtr(MainForm);
	fldP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, MainCreditsField));
	str = FldGetTextPtr(fldP);
	if(str == NULL) // make new pointer
		str=MemPtrNew(20);
	StrIToA(str, g_prefs.credits);
	FldSetTextPtr(fldP, str);
	FldDrawField(fldP);
}

static void MainFormDrawStatusItem(void *table, Int16 row, Int16 column, RectangleType *bounds) {
	MemHandle bitmapH;
	UInt16 recordNum;
	MemHandle recH;
	nslookupDBRecord *recP;
	
	recordNum = TblGetRowID(table, row);
	recH = DmQueryRecord(gNSLookupDB, recordNum);
	recP = MemHandleLock(recH);
	switch(recP->status) {
		case 0: // nothing
			bitmapH = DmGetResource(bitmapRsc, HostStatusNothingBitmapFamily);
			if(bitmapH) {
				BitmapType *bitmap;
				bitmap = (BitmapPtr)MemHandleLock(bitmapH);
				WinDrawBitmap(bitmap, bounds->topLeft.x, bounds->topLeft.y);
				MemHandleUnlock(bitmapH);
				DmReleaseResource(bitmapH);
			}
			break;
		case 1: // cracked
			bitmapH = DmGetResource(bitmapRsc, HostStatusCrackedBitmapFamily);
			if(bitmapH) {
				BitmapType *bitmap;
				bitmap = (BitmapPtr)MemHandleLock(bitmapH);
				WinDrawBitmap(bitmap, bounds->topLeft.x, bounds->topLeft.y);
				MemHandleUnlock(bitmapH);
				DmReleaseResource(bitmapH);
			}
			break;
		case 2: // shutdown
			bitmapH = DmGetResource(bitmapRsc, HostStatusShutDownBitmapFamily);
			if(bitmapH) {
				BitmapType *bitmap;
				bitmap = (BitmapPtr)MemHandleLock(bitmapH);
				WinDrawBitmap(bitmap, bounds->topLeft.x, bounds->topLeft.y);
				MemHandleUnlock(bitmapH);
				DmReleaseResource(bitmapH);
			}
			break;
		case 3: // ice
			bitmapH = DmGetResource(bitmapRsc, HostStatusICEBitmapFamily);
			if(bitmapH) {
				BitmapType *bitmap;
				bitmap = (BitmapPtr)MemHandleLock(bitmapH);
				WinDrawBitmap(bitmap, bounds->topLeft.x, bounds->topLeft.y);
				MemHandleUnlock(bitmapH);
				DmReleaseResource(bitmapH);
			}
			break;
		default: 
			bitmapH = DmGetResource(bitmapRsc, HostStatusNothingBitmapFamily);
			if(bitmapH) {
				BitmapType *bitmap;
				bitmap = (BitmapPtr)MemHandleLock(bitmapH);
				WinDrawBitmap(bitmap, bounds->topLeft.x, bounds->topLeft.y);
				MemHandleUnlock(bitmapH);
				DmReleaseResource(bitmapH);
			}
			break;
	}
	MemHandleUnlock(recH);
}

static void MainFormDrawAddressItem(void *table, Int16 row, Int16 column, RectangleType *bounds) {
	UInt16 recordNum;
	MemHandle recH;
	nslookupDBRecord *recP;
	FontID originalFont = FntSetFont(stdFont);
	
	recordNum = TblGetRowID(table, row);
	recH = DmQueryRecord(gNSLookupDB, recordNum);
	recP = MemHandleLock(recH);
	WinDrawChars(recP->address, StrLen(recP->address), bounds->topLeft.x+1, bounds->topLeft.y);
	MemHandleUnlock(recH);
	FntSetFont(originalFont);
}

static void MainFormDrawNameItem(void *table, Int16 row, Int16 column, RectangleType *bounds) {
	UInt16 recordNum;
	MemHandle recH;
	nslookupDBRecord *recP;
	FontID originalFont = FntSetFont(stdFont);
	
	recordNum = TblGetRowID(table, row);
	recH = DmQueryRecord(gNSLookupDB, recordNum);
	recP = MemHandleLock(recH);
	WinDrawChars(recP->name, StrLen(recP->name), bounds->topLeft.x+1, bounds->topLeft.y);
	MemHandleUnlock(recH);
	FntSetFont(originalFont);
}

static void MainFormDrawPasswordItem(void *table, Int16 row, Int16 column, RectangleType *bounds) {
	UInt16 recordNum;
	MemHandle recH;
	nslookupDBRecord *recP;
	FontID originalFont = FntSetFont(stdFont);
	
	recordNum = TblGetRowID(table, row);
	recH = DmQueryRecord(gNSLookupDB, recordNum);
	recP = MemHandleLock(recH);
	WinDrawChars(recP->passcode, StrLen(recP->passcode), bounds->topLeft.x+1, bounds->topLeft.y);
	MemHandleUnlock(recH);
	FntSetFont(originalFont);
}

static void MainFormDrawEmailFromItem(void *table, Int16 row, Int16 column, RectangleType *bounds) {
	UInt16 recordNum;
	MemHandle recH;
	emailDBRecord *recP;
	FontID originalFont = FntSetFont(stdFont);
	
	recordNum = TblGetRowID(table, row);
	recH = DmQueryRecord(gEmailDB, recordNum);
	recP = MemHandleLock(recH);
	WinDrawChars(recP->from, StrLen(recP->from), bounds->topLeft.x+1, bounds->topLeft.y);
	MemHandleUnlock(recH);
	FntSetFont(originalFont);
}

static void MainFormDrawEmailSubjectItem(void *table, Int16 row, Int16 column, RectangleType *bounds) {
	UInt16 recordNum;
	MemHandle recH;
	emailDBRecord *recP;
	FontID originalFont = FntSetFont(stdFont);
	
	recordNum = TblGetRowID(table, row);
	recH = DmQueryRecord(gEmailDB, recordNum);
	recP = MemHandleLock(recH);
	WinDrawChars(recP->subject, StrLen(recP->subject), bounds->topLeft.x+1, bounds->topLeft.y);
	MemHandleUnlock(recH);
	FntSetFont(originalFont);
}

static void MainFormHostsScroll(WinDirectionType direction, UInt16 units, Boolean byLine) {
    TableType  *table;
    UInt16  rowsInPage;
    UInt16  newTopVisibleRecord;
 
    table = GetObjectPtr(MainNSLookupTable);
    rowsInPage = MainFormNumberOfRows(table) - 1;
    newTopVisibleRecord = g_NSLookupTopRecord;

    // Scroll the table down.
    if (direction == winDown)
    {
        // Scroll down by line units.
        if (byLine)
        {
            // Scroll down by the requested number of lines
            if (! SeekHostRecord(&newTopVisibleRecord, units,
                             dmSeekForward))
            {
                // Tried to scroll past bottom; go to the last record.
                newTopVisibleRecord = dmMaxRecordIndex;
                SeekHostRecord(&newTopVisibleRecord, 1, dmSeekBackward);
            }
        }
        // Scroll in page units.
        else
        {
            // Try scrolling down by the requested number of pages.
            if (! SeekHostRecord(&newTopVisibleRecord, units * rowsInPage,
                             dmSeekForward))
            {
                // Hit bottom; try going backwards one page from the last
                // record.
                newTopVisibleRecord = dmMaxRecordIndex;
                if (! SeekHostRecord(&newTopVisibleRecord, rowsInPage,
                                 dmSeekBackward))
                {
                    // Not enough records to fill one page; go to the
                    // first record.
                    newTopVisibleRecord = 0;
                    SeekHostRecord(&newTopVisibleRecord, 0, dmSeekForward);
                }
            }
        }
    }
    // Scroll the table up.
    else
    {
        // Scroll up by line units.
        if (byLine)
        {
            // Scroll up by the requested number of lines
            if (! SeekHostRecord(&newTopVisibleRecord, units,
                             dmSeekBackward))
            {
                // Tried to scroll past top; go to the first record.
                newTopVisibleRecord = 0;
                SeekHostRecord(&newTopVisibleRecord, 0, dmSeekForward);
            }
        }
        // Scroll in page units.
        else
        {
            // Try scrolling up by the requested number of pages.
            if (! SeekHostRecord(&newTopVisibleRecord, units * rowsInPage,
                             dmSeekBackward))
            {
                // Hit top; go to the first record.
                newTopVisibleRecord = 0;
                SeekHostRecord(&newTopVisibleRecord, 0, dmSeekForward);
            }
        }
    }

    // Redraw the table, but only if there has been a change.
    if (g_NSLookupTopRecord != newTopVisibleRecord)
    {
        g_NSLookupTopRecord = newTopVisibleRecord;
        MainFormLoadHostsTable();
        TblRedrawTable(table);
    }
}

static void MainFormEmailScroll(WinDirectionType direction, UInt16 units, Boolean byLine) {
    TableType  *table;
    UInt16  rowsInPage;
    UInt16  newTopVisibleRecord;
 
    table = GetObjectPtr(MainEmailTable);
    rowsInPage = MainFormNumberOfRows(table) - 1;
    newTopVisibleRecord = g_EmailTopRecord;

    // Scroll the table down.
    if (direction == winDown)
    {
        // Scroll down by line units.
        if (byLine)
        {
            // Scroll down by the requested number of lines
            if (! SeekEmailRecord(&newTopVisibleRecord, units,
                             dmSeekForward))
            {
                // Tried to scroll past bottom; go to the last record.
                newTopVisibleRecord = dmMaxRecordIndex;
                SeekEmailRecord(&newTopVisibleRecord, 1, dmSeekBackward);
            }
        }
        // Scroll in page units.
        else
        {
            // Try scrolling down by the requested number of pages.
            if (! SeekEmailRecord(&newTopVisibleRecord, units * rowsInPage,
                             dmSeekForward))
            {
                // Hit bottom; try going backwards one page from the last
                // record.
                newTopVisibleRecord = dmMaxRecordIndex;
                if (! SeekEmailRecord(&newTopVisibleRecord, rowsInPage,
                                 dmSeekBackward))
                {
                    // Not enough records to fill one page; go to the
                    // first record.
                    newTopVisibleRecord = 0;
                    SeekEmailRecord(&newTopVisibleRecord, 0, dmSeekForward);
                }
            }
        }
    }
    // Scroll the table up.
    else
    {
        // Scroll up by line units.
        if (byLine)
        {
            // Scroll up by the requested number of lines
            if (! SeekEmailRecord(&newTopVisibleRecord, units,
                             dmSeekBackward))
            {
                // Tried to scroll past top; go to the first record.
                newTopVisibleRecord = 0;
                SeekEmailRecord(&newTopVisibleRecord, 0, dmSeekForward);
            }
        }
        // Scroll in page units.
        else
        {
            // Try scrolling up by the requested number of pages.
            if (! SeekEmailRecord(&newTopVisibleRecord, units * rowsInPage,
                             dmSeekBackward))
            {
                // Hit top; go to the first record.
                newTopVisibleRecord = 0;
                SeekEmailRecord(&newTopVisibleRecord, 0, dmSeekForward);
            }
        }
    }

    // Redraw the table, but only if there has been a change.
    if (g_EmailTopRecord != newTopVisibleRecord)
    {
        g_EmailTopRecord = newTopVisibleRecord;
        MainFormLoadEmailTable();
        TblRedrawTable(table);
    }
}

static void MainFormUpdateNSLookupScrollers() {
	UInt16 upIndex;
	UInt16 downIndex;
	Boolean scrollableUp;
	Boolean scrollableDown;
	FormType *frmP;
	Int16 row;
	TableType *table;
	UInt16 recordNum;
	
	scrollableUp = g_NSLookupTopRecord > 0;
	
	table = GetObjectPtr(MainNSLookupTable);
	row = TblGetLastUsableRow(table);
	if(row != -1) recordNum = TblGetRowID(table, row);
	
	scrollableDown = SeekHostRecord(&recordNum, 1, dmSeekForward);
	
	frmP = FrmGetActiveForm();
	upIndex = FrmGetObjectIndex(frmP, MainScrollUpRepeating);
	downIndex = FrmGetObjectIndex(frmP, MainScrollDownRepeating);
	
	FrmUpdateScrollers(frmP, upIndex, downIndex, scrollableUp, scrollableDown);
}

static void MainFormUpdateEmailScrollers() {
	UInt16 upIndex;
	UInt16 downIndex;
	Boolean scrollableUp;
	Boolean scrollableDown;
	FormType *frmP;
	Int16 row;
	TableType *table;
	UInt16 recordNum;
	
	scrollableUp = g_EmailTopRecord > 0;
	
	table = GetObjectPtr(MainEmailTable);
	row = TblGetLastUsableRow(table);
	if(row != -1) recordNum = TblGetRowID(table, row);
	
	scrollableDown = SeekEmailRecord(&recordNum, 1, dmSeekForward);
	
	frmP = FrmGetActiveForm();
	upIndex = FrmGetObjectIndex(frmP, MainEmailScrollUpRepeating);
	downIndex = FrmGetObjectIndex(frmP, MainEmailScrollDownRepeating);
	
	FrmUpdateScrollers(frmP, upIndex, downIndex, scrollableUp, scrollableDown);
}

void MainFormLoadHostsTable() {
	UInt16 row, numRows, visibleRows;
	//UInt16 lineHeight;
	UInt16 recordNum;
	//FontID curFont;
	TableType *table;
	
	table = GetObjectPtr(MainNSLookupTable);
	TblUnhighlightSelection(table);
	
	visibleRows = MainFormNumberOfRows(table);
	recordNum = g_NSLookupTopRecord;
	if(! SeekHostRecord(&recordNum, visibleRows - 1, dmSeekForward)) {
		// At least one line has no record.  Fix it.
		g_NSLookupTopRecord = dmMaxRecordIndex;
		if(!SeekHostRecord(&g_NSLookupTopRecord, visibleRows - 1, dmSeekBackward)) {
			// Not enough records to fill one page
			g_NSLookupTopRecord = 0;
			SeekHostRecord(&g_NSLookupTopRecord, 0, dmSeekForward);
		}
	}
	recordNum = g_NSLookupTopRecord;
	for(row=0; row < visibleRows; row++) {
		if(!SeekHostRecord(&recordNum, 0, dmSeekForward)) break;
		//Make the row usable
		TblSetRowUsable(table, row, true);
		// Mark row invalid
		TblMarkRowInvalid(table, row);
		// Store the record number as the row ID
		TblSetRowID(table, row, recordNum);
		
		recordNum++;
	}
	
	// Hide items that don't have any data
	numRows = TblGetNumberOfRows(table);
	while (row < numRows) {
		TblSetRowUsable(table, row, false);
		row++;
	}
	MainFormUpdateNSLookupScrollers();
}

void MainFormLoadEmailTable() {
	UInt16 row, numRows, visibleRows;
	//UInt16 lineHeight;
	UInt16 recordNum;
	//FontID curFont;
	TableType *table;
	
	table = GetObjectPtr(MainEmailTable);
	TblUnhighlightSelection(table);
	
	visibleRows = MainFormNumberOfRows(table);
	recordNum = g_EmailTopRecord;
	if(! SeekEmailRecord(&recordNum, visibleRows - 1, dmSeekForward)) {
		// At least one line has no record.  Fix it.
		g_EmailTopRecord = dmMaxRecordIndex;
		if(!SeekEmailRecord(&g_EmailTopRecord, visibleRows - 1, dmSeekBackward)) {
			// Not enough records to fill one page
			g_EmailTopRecord = 0;
			SeekEmailRecord(&g_EmailTopRecord, 0, dmSeekForward);
		}
	}
	recordNum = g_EmailTopRecord;
	for(row=0; row < visibleRows; row++) {
		if(!SeekEmailRecord(&recordNum, 0, dmSeekForward)) break;
		//Make the row usable
		TblSetRowUsable(table, row, true);
		// Mark row invalid
		TblMarkRowInvalid(table, row);
		// Store the record number as the row ID
		TblSetRowID(table, row, recordNum);
		
		recordNum++;
	}
	
	// Hide items that don't have any data
	numRows = TblGetNumberOfRows(table);
	while (row < numRows) {
		TblSetRowUsable(table, row, false);
		row++;
	}
	MainFormUpdateEmailScrollers();
}


static void MainFormSetupHostsTable() {
	TableType *table;
	Int16 row, col, numRows;
	
	table = GetObjectPtr(MainNSLookupTable);
	numRows = TblGetNumberOfRows(table);
	
	for(row=0; row < numRows; row++) {
		TblSetItemStyle(table, row, STATUSCOL, customTableItem);
		TblSetItemInt(table, row, STATUSCOL, 0);
		TblSetItemStyle(table, row, ADDRESSCOL, customTableItem);
		TblSetItemStyle(table, row, NAMECOL, customTableItem);
		TblSetItemStyle(table, row, PASSWORDCOL, customTableItem); 
		TblSetRowUsable(table, row, false);
	}
	
	for(col=0; col < NUMHOSTTABLECOLS; col++) {
		TblSetColumnUsable(table, col, true);
	}
	MainFormLoadHostsTable();	
	TblSetCustomDrawProcedure(table, STATUSCOL, MainFormDrawStatusItem);
	TblSetCustomDrawProcedure(table, ADDRESSCOL, MainFormDrawAddressItem);
	TblSetCustomDrawProcedure(table, NAMECOL, MainFormDrawNameItem);
	TblSetCustomDrawProcedure(table, PASSWORDCOL, MainFormDrawPasswordItem);
	
	TblDrawTable(table);
}

static void MainFormSetupEmailTable() {
	TableType *table;
	Int16 row, col, numRows;
	
	table = GetObjectPtr(MainEmailTable);
	numRows = TblGetNumberOfRows(table);
	
	for(row=0; row < numRows; row++) {
		TblSetItemStyle(table, row, FROMCOL, customTableItem);
		TblSetItemStyle(table, row, SUBJECTCOL, customTableItem); 
		TblSetRowUsable(table, row, false);
	}
	
	for(col=0; col < NUMEMAILTABLECOLS; col++) {
		TblSetColumnUsable(table, col, true);
	}
	MainFormLoadEmailTable();
	
	TblSetCustomDrawProcedure(table, FROMCOL, MainFormDrawEmailFromItem);
	TblSetCustomDrawProcedure(table, SUBJECTCOL, MainFormDrawEmailSubjectItem);
	
	TblDrawTable(table);
}

static void MainFormDrawGadget(FormType *frmP, UInt16 gadgetID) {
	RectangleType bounds;
	MemHandle bitmapH;
	BitmapType *bitmapP;
	UInt16 gadgetIndex = FrmGetObjectIndex(frmP, gadgetID);
	
	FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
	if(g_prefs.gameover) return;
	switch(gadgetID) {
		case MainConnectGadget:
			bitmapH = DmGetResource(bitmapRsc, 1400);
			bitmapP = MemHandleLock(bitmapH);
			WinDrawBitmap(bitmapP, bounds.topLeft.x, bounds.topLeft.y);
			MemHandleUnlock(bitmapH);
			break;
		case MainNSLookupGadget:
			bitmapH = DmGetResource(bitmapRsc, 1800);
			bitmapP = MemHandleLock(bitmapH);
			WinDrawBitmap(bitmapP, bounds.topLeft.x, bounds.topLeft.y);
			MemHandleUnlock(bitmapH);
			break;
		case MainCrackerGadget:
			if(g_prefs.cracker >= CRACKERPASSCRACK) {
				bitmapH = DmGetResource(bitmapRsc, 1200);
				bitmapP = MemHandleLock(bitmapH);
				WinDrawBitmap(bitmapP, bounds.topLeft.x, bounds.topLeft.y);
				MemHandleUnlock(bitmapH);
			}
			break;
		case MainScanGadget:
			if(g_prefs.scanner >= SCANNERSNOOP) {
				bitmapH = DmGetResource(bitmapRsc, 1500);
				bitmapP = MemHandleLock(bitmapH);
				WinDrawBitmap(bitmapP, bounds.topLeft.x, bounds.topLeft.y);
				MemHandleUnlock(bitmapH);
			}
			break;
		case MainICEBreakerGadget:
			if(g_prefs.ice >= ICETECHICE) {
				bitmapH = DmGetResource(bitmapRsc, 1600);
				bitmapP = MemHandleLock(bitmapH);
				WinDrawBitmap(bitmapP, bounds.topLeft.x, bounds.topLeft.y);
				MemHandleUnlock(bitmapH);
			}
			break;
		case MainCompStoreGadget:
			bitmapH = DmGetResource(bitmapRsc, 1900);
			bitmapP = MemHandleLock(bitmapH);
			WinDrawBitmap(bitmapP, bounds.topLeft.x, bounds.topLeft.y);
			MemHandleUnlock(bitmapH);
			break;
		case MainBlackHatGadget:
			bitmapH = DmGetResource(bitmapRsc, 2000);
			bitmapP = MemHandleLock(bitmapH);
			WinDrawBitmap(bitmapP, bounds.topLeft.x, bounds.topLeft.y);
			MemHandleUnlock(bitmapH);
			break;

	}
}

static void MainFormDrawGadgets() {
	FormType *frmP;
	
	frmP = FrmGetActiveForm();
	MainFormDrawGadget(frmP, MainConnectGadget);
	MainFormDrawGadget(frmP, MainNSLookupGadget);
	MainFormDrawGadget(frmP, MainCrackerGadget);
	MainFormDrawGadget(frmP, MainScanGadget);
	MainFormDrawGadget(frmP, MainICEBreakerGadget);
	MainFormDrawGadget(frmP, MainCompStoreGadget);
	MainFormDrawGadget(frmP, MainBlackHatGadget);
}

static void MainFormDrawBorders() {
	RectangleType border1, border2;
	RGBColorType greenRGB;
	IndexedColorType greenIndexed, oldIndexed;
	CustomPatternType bpattern = { 0xAA, 0x55, 0xAA, 0x55,
								   0xAA, 0x55, 0xAA, 0x55 };
	
	border1.topLeft.x = 25;
	border1.topLeft.y = 17;
	border1.extent.x = 110;
	border1.extent.y = 8;
	
	border2.topLeft.x = 25;
	border2.topLeft.y = 89;
	border2.extent.x = 110;
	border2.extent.y = 8;
	
	if(0x80 & g_SupportedDepths) {
		greenRGB.r = 50; greenRGB.g = 200; greenRGB.b = 50;
		greenIndexed = WinRGBToIndex(&greenRGB);
		oldIndexed = WinSetForeColor(greenIndexed);
		WinSetPattern(&bpattern);
		WinFillRectangle(&border1, 1);
		WinFillRectangle(&border2, 1);
		WinSetForeColor(oldIndexed);
	} else {
		WinSetPattern(&bpattern);
		WinFillRectangle(&border1, 1);
		WinFillRectangle(&border2, 1);
	}
}

static void MainFormInit(FormType *frmP) {
	WinScreenMode(winScreenModeGetSupportedDepths, NULL, NULL, &g_SupportedDepths, NULL);
	MainFormSetCredits(g_prefs.credits);
	MainFormSetupHostsTable();
	MainFormSetupEmailTable();
	
	// is username set?
	if(!StrCompare(g_prefs.username, "")) {
		// no get username
		FrmGotoForm(WelcomeForm);
	}
	MainFormDrawGadgets();
	MainFormDrawBorders();
	//FrmDrawForm(frmP);
}

static Boolean MainFormDoRepeatCommand(UInt16 command) {
	Boolean handled = false;
	switch(command) {
		case MainScrollUpRepeating:
			MainFormHostsScroll(winUp, 1, false);
			break;
		case MainScrollDownRepeating:
			MainFormHostsScroll(winDown, 1, false);
			break;
		case MainEmailScrollUpRepeating:
			MainFormEmailScroll(winUp, 1, false);
			break;
		case MainEmailScrollDownRepeating:
			MainFormEmailScroll(winDown, 1, false);
			break;
	}
	return handled;
}

static Boolean MainFormDoPenCommand(UInt16 x, UInt16 y) {
	Boolean handled = false;
	FormType *frmP;
	UInt16 gadgetIndex;
	RectangleType bounds;
	
   	if(g_prefs.gameover) return handled;
	frmP = FrmGetActiveForm();
	gadgetIndex = FrmGetObjectIndex(frmP, MainConnectGadget);
	FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
	if(RctPtInRectangle(x, y, &bounds)) {
		FrmGotoForm(ConnectForm);
		handled = true;
	}
	gadgetIndex = FrmGetObjectIndex(frmP, MainNSLookupGadget);
	FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
	if(RctPtInRectangle(x, y, &bounds)) {
		FrmGotoForm(NSLookupForm);
		handled = true;
	}
	gadgetIndex = FrmGetObjectIndex(frmP, MainCrackerGadget);
	FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
	if((RctPtInRectangle(x, y, &bounds)) && (g_prefs.cracker >= CRACKERPASSCRACK)) {
		FrmGotoForm(CrackForm);
		handled = true;
	}
	gadgetIndex = FrmGetObjectIndex(frmP, MainScanGadget);
	FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
	if((RctPtInRectangle(x, y, &bounds)) && (g_prefs.scanner >= SCANNERSNOOP)) {
		FrmGotoForm(ScanForm);
		handled = true;
	}
	gadgetIndex = FrmGetObjectIndex(frmP, MainICEBreakerGadget);
	FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
	if((RctPtInRectangle(x, y, &bounds)) && (g_prefs.ice >= ICETECHICE)) {
		FrmGotoForm(ICEBreakerForm);
		handled = true;
	}
	gadgetIndex = FrmGetObjectIndex(frmP, MainCompStoreGadget);
	FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
	if(RctPtInRectangle(x, y, &bounds)) {
		FrmGotoForm(CompStoreForm);
		handled = true;
	}
	gadgetIndex = FrmGetObjectIndex(frmP, MainBlackHatGadget);
	FrmGetObjectBounds(frmP, gadgetIndex, &bounds);
	if(RctPtInRectangle(x, y, &bounds)) {
		FrmGotoForm(BlackHatForm);
		handled = true;
	}
	return handled;
}

static Boolean MainFormDoTableCommand(UInt16 tableID, Int16 row, Int16 column) {
	Boolean handled = false;
	TableType *table;
	
	if(tableID == MainEmailTable) {
		table = GetObjectPtr(tableID);
		g_emailRecNum = TblGetRowID(table, row);
		FrmGotoForm(EmailViewForm);
	}
	if(tableID == MainNSLookupTable) {
		table = GetObjectPtr(tableID);
		g_hostRecNum = TblGetRowID(table, row);
		FrmGotoForm(HostForm);
	}
	return handled;
}

static void MainFormResetGame() {
	FormType *frmP;
	Err err;

	if(FrmAlert(ResetGameAlert) == 0) {
		// Close Databases
		err = DmCloseDatabase(gNetDB);
		err = DmCloseDatabase(gNSLookupDB);
		err = DmCloseDatabase(gEmailDB);
		// Delete Databases
		err = DmDeleteDatabase(0, DmFindDatabase(0, netDBName));
		err = DmDeleteDatabase(0, DmFindDatabase(0, nslookupDBName));
		err = DmDeleteDatabase(0, DmFindDatabase(0, emailDBName));
		gNetDB = NULL;
		gNSLookupDB = NULL;
		gEmailDB = NULL;
		// Make New Databases
	    AHDBGetNetworkDatabase();
	    AHDBGetEmailDatabase();
	    AHDBGetNSLookupDatabase();
		// Reset User profile
		g_prefs.credits = 0;
		g_prefs.numCracked = 0;
		g_prefs.ram = RAM32MB;
		g_prefs.cpu = CPU200MHZ;
		g_prefs.os = OSZDOS;
		g_prefs.scanner = 0;
		g_prefs.cracker = 0;
		g_prefs.ice = 0;
		g_prefs.score = 0;
		g_prefs.gameover = false;
		g_prefs.source = 0;
		g_prefs.payroll = 0;
		g_prefs.ufo = 0;
		MainFormSetCredits(g_prefs.credits);
		g_NSLookupTopRecord = 0;
		g_EmailTopRecord = 0;
		MainFormSetupHostsTable();
		MainFormSetupEmailTable();
	
		// is username set?
		if(!StrCompare(g_prefs.username, "")) {
			// no get username
			FrmGotoForm(WelcomeForm);
		}
	}
	frmP = FrmGetActiveForm();
	FrmDrawForm(frmP);
	MainFormDrawGadgets();
	MainFormDrawBorders();
}

static Boolean MainFormDoCommand(UInt16 command)
{
    Boolean handled = false;
    FormType *frmP;

    switch (command) {
	    case OptionsAboutAlphaHacker:
	        MenuEraseStatus(0);
    	    frmP = FrmInitForm(AboutForm);
        	FrmDoDialog(frmP);                    
	        FrmDeleteForm(frmP);
	        MainFormDrawGadgets();
	        handled = true;
    	    break;
    	case OptionsRegister:
    		MenuEraseStatus(0);
    		FrmGotoForm(RegisterForm);
    		handled = true;
    		break;
    	case OptionsCheat:
    		MenuEraseStatus(0);
    		FrmGotoForm(CheatForm);
    		handled = true;
    		break;
		case GameUserInfo:
			MenuEraseStatus(0);
			FrmGotoForm(UserForm);
			handled = true;
			break;
		case GameSWInventory:
			MenuEraseStatus(0);
			FrmGotoForm(SWInventoryForm);
			handled = true;
			break;
		case GameResetGame:
			MenuEraseStatus(0);
			MainFormResetGame();
			handled = true;
			break;
    }
    return handled;
}

static Boolean MainFormHandleEvent(EventType * eventP) {
    Boolean handled = false;
    FormType *frmP;
    FieldType *fldP;

    switch (eventP->eType) 
        {
        case menuEvent:
            return MainFormDoCommand(eventP->data.menu.itemID);
            
        case ctlRepeatEvent:
        	return MainFormDoRepeatCommand(eventP->data.ctlEnter.controlID);
        
        case tblSelectEvent:
        	return MainFormDoTableCommand(eventP->data.tblSelect.tableID,
        		eventP->data.tblSelect.row, eventP->data.tblSelect.column);
        
        case penDownEvent:
           	return MainFormDoPenCommand(eventP->screenX, eventP->screenY);

        case frmOpenEvent:
            frmP = FrmGetActiveForm();
            FrmDrawForm(frmP);
            MainFormInit(frmP);
            handled = true;
            break;
            
        case frmUpdateEvent:
            /* To do any custom drawing here, first call
             * FrmDrawForm(), then do your drawing, and
             * then set handled to true. */
            break;
            
        case frmCloseEvent:
        	fldP = GetObjectPtr(MainCreditsField);
        	if(FldGetTextPtr(fldP) != NULL) MemPtrFree(FldGetTextPtr(fldP));
        	break;

        default:
            break;
        }
    
    return handled;
}

#pragma mark ----------

static Boolean AppHandleEvent(EventType * eventP)
{
    UInt16 formId;
    FormType * frmP;

    if (eventP->eType == frmLoadEvent)
    {
        // Load the form resource.
        formId = eventP->data.frmLoad.formID;
        frmP = FrmInitForm(formId);
        FrmSetActiveForm(frmP);

        // Set the event handler for the form.
        switch (formId)
        {
        case MainForm:
            FrmSetEventHandler(frmP, MainFormHandleEvent);
            break;
        case UserForm:
        	FrmSetEventHandler(frmP, UserFormHandleEvent);
        	break;
        case SWInventoryForm:
        	FrmSetEventHandler(frmP, SWInvFormHandleEvent);
        	break;
        case CompStoreForm:
        	FrmSetEventHandler(frmP, CompStoreFormHandleEvent);
        	break;
        case BlackHatForm:
        	FrmSetEventHandler(frmP, BlackHatFormHandleEvent);
        	break;
        case EmailViewForm:
        	FrmSetEventHandler(frmP, EmailViewFormHandleEvent);
        	break;
        case CrackForm:
        	FrmSetEventHandler(frmP, CrackFormHandleEvent);
        	break;
        case ScanForm:
        	FrmSetEventHandler(frmP, ScanFormHandleEvent);
        	break;
        case ICEBreakerForm:
        	FrmSetEventHandler(frmP, ICEFormHandleEvent);
        	break;
        case ConnectForm:
        	FrmSetEventHandler(frmP, ConnectFormHandleEvent);
        	break;
        case ServerForm:
        	FrmSetEventHandler(frmP, ServerFormHandleEvent);
        	break;
        case NSLookupForm:
        	FrmSetEventHandler(frmP, NSLookupFormHandleEvent);
        	break;
        case AccountsPayableForm:
        	FrmSetEventHandler(frmP, AccountsPayableFormHandleEvent);
        	break;
        case AdministrationForm:
        	FrmSetEventHandler(frmP, AdministrationFormHandleEvent);
        	break;
        case HostForm:
        	FrmSetEventHandler(frmP, HostFormHandleEvent);
        	break;
        case MOTDForm:
        	FrmSetEventHandler(frmP, MOTDFormHandleEvent);
        	break;
        case WelcomeForm:
        	FrmSetEventHandler(frmP, WelcomeFormHandleEvent);
        	break;
        case DatabaseForm:
        	FrmSetEventHandler(frmP, DatabaseFormHandleEvent);
        	break;
        case RegisterForm:
        	FrmSetEventHandler(frmP, RegisterFormHandleEvent);
        	break;
        case GameOverForm:
        	FrmSetEventHandler(frmP, GameOverFormHandleEvent);
        	break;
        case CheatForm:
        	FrmSetEventHandler(frmP, CheatFormHandleEvent);
        	break;
        case ShutdownForm:
        	FrmSetEventHandler(frmP, ShutdownFormHandleEvent);
        	break;
        default:
            break;
        }
        return true;
    }
    return false;
}

static void AppEventLoop(void)
{
    UInt16 error;
    EventType event;

    do {
        /* change timeout if you need periodic nilEvents */
        EvtGetEvent(&event, g_timer);

        if (! SysHandleEvent(&event))
        {
            if (! MenuHandleEvent(0, &event, &error))
            {
                if (! AppHandleEvent(&event))
                {
                    FrmDispatchEvent(&event);
                }
            }
        }
    } while (event.eType != appStopEvent);
}

static Err AppStart(void)
{
    UInt16 prefsSize;

    /* Read the saved preferences / saved-state information. */
    prefsSize = sizeof(AlphaHackerPreferenceType);
    if (PrefGetAppPreferences(
        appFileCreator, appPrefID, &g_prefs, &prefsSize, true) == noPreferenceFound) {
			// must be a new user
			g_prefs.credits = 0;
			g_prefs.numCracked = 0;
			g_prefs.ram = RAM32MB;
			g_prefs.cpu = CPU200MHZ;
			g_prefs.os = OSZDOS;
			g_prefs.scanner = 0;
			g_prefs.cracker = 0;
			g_prefs.ice = 0;
			g_prefs.score = 0;
			g_prefs.gameover = false;
			g_prefs.source = 0;
			g_prefs.payroll = 0;
			g_prefs.ufo = 0;
			StrCopy(g_prefs.username, "");
    }
    SysRandom(TimGetTicks()); // Seed Random Number Generator
    AHDBGetNetworkDatabase();
    AHDBGetEmailDatabase();
    AHDBGetNSLookupDatabase();
   	registered = RegisterFormIsRegistered();
   	//registered = '\1';
   	g_tickspersec = SysTicksPerSecond();
   	g_timer = g_tickspersec / 10;
    return errNone;
}

static void AppStop(void)
{
    /* Write the saved preferences / saved-state information.  This
     * data will be saved during a HotSync backup. */
    PrefSetAppPreferences(
        appFileCreator, appPrefID, appPrefVersionNum, 
        &g_prefs, sizeof(AlphaHackerPreferenceType), true);
        
    /* Close all the open forms. */
    FrmCloseAllForms();
    
    // Close databases
    DmCloseDatabase(gNetDB);
    DmCloseDatabase(gEmailDB);
    DmCloseDatabase(gNSLookupDB);
}

/* all code from here to end of file should use no global variables */
#pragma warn_a5_access on

static Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags)
{
    UInt32 romVersion;

    /* See if we're on in minimum required version of the ROM or later. */
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
    if (romVersion < requiredVersion)
    {
        if ((launchFlags & 
            (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
            (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp))
        {
            FrmAlert (RomIncompatibleAlert);

            /* Palm OS versions before 2.0 will continuously relaunch this
             * app unless we switch to another safe one. */
            if (romVersion < kPalmOS20Version)
            {
                AppLaunchWithCommand(
                    sysFileCDefaultApp, 
                    sysAppLaunchCmdNormalLaunch, NULL);
            }
        }

        return sysErrRomIncompatible;
    }

    return errNone;
}

static UInt32 AlphaHackerPalmMain(
    UInt16 cmd, 
    MemPtr /*cmdPBP*/, 
    UInt16 launchFlags)
{
    Err error;

    error = RomVersionCompatible (ourMinVersion, launchFlags);
    if (error) return (error);

    switch (cmd)
    {
    case sysAppLaunchCmdNormalLaunch:
        error = AppStart();
        if (error) 
            return error;

        /* start application by opening the main form
         * and then entering the main event loop */
        FrmGotoForm(MainForm);
        AppEventLoop();
        
        AppStop();
        break;

    default:
        break;
    }

    return errNone;
}

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
    return AlphaHackerPalmMain(cmd, cmdPBP, launchFlags);
}

/* turn a5 warning off to prevent it being set off by C++
 * static initializer code generation */
#pragma warn_a5_access reset
