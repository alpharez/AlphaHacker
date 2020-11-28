#ifndef ALPHAHACKER_H_
#define ALPHAHACKER_H_

// Internal Structures

typedef struct AlphaHackerPreferenceType
{
	Char username[16];
	Char password[16];
	Int32 credits;
	Int16 numCracked;
	Int16 ram;
	Int16 cpu;
	Int16 os;
	Int16 scanner;
	Int16 cracker;
	Int16 ice;
	Int16 score;
	Boolean gameover;
	UInt16 key;
	UInt16 source;
	UInt16 payroll;
	UInt16 ufo;
} AlphaHackerPreferenceType;

typedef struct {
	Int16 network;
	Int16 host;
	Int16 level;		
	Char name[16];
	Char passcode[9];
	Int16 timeout;
	Char motd[128];
	Char clue[64];
	UInt16 balance;
	Boolean hasICE;
	Boolean hasGovtICE;
	Boolean hasAdmin;
	Boolean MOTDChanged;
	Boolean FundsTaken;
	Boolean hasSourceCode;
	Boolean hasPayrollData;
	Boolean hasUFOData;
	Boolean hasClue;
	Boolean wantsSourceCode;
	Boolean wantsPayrollData;
	Boolean wantsUFOData;
} netDBRecord;

typedef struct {
	Char from[26];
	Char subject[32];
	Char message[512];
} emailDBRecord;

typedef struct {
	Int16 status;
	Char address[9];
	Char name[16];
	Char passcode[9];
} nslookupDBRecord;

typedef struct {
	Int16 level;
	Char name[16];
	Char passcode[9];
	Int16 timeout;
	Char motd[128];
	Char clue[64];
	UInt16 balance;
	Boolean hasICE;
	Boolean hasGovtICE;
	Boolean hasAdmin;
	Boolean MOTDChanged;
	Boolean FundsTaken;
	Boolean hasSourceCode;
	Boolean hasPayrollData;
	Boolean hasUFOData;
	Boolean hasClue;
	Boolean wantsSourceCode;
	Boolean wantsPayrollData;
	Boolean wantsUFOData;
} hostRecord;

// Global variables

#define NUMHOSTS				47			// Number of hosts on the net
#define NUMEMAILS				2
#define NUMTOTEMAILS			12
#define NUMPASSWORDS			50

extern AlphaHackerPreferenceType g_prefs;
extern UInt16 g_timer;						// timer for eventloop
extern UInt16 g_tickspersec;
extern Boolean g_cracking;					// are we cracking?
extern Boolean g_scanning;					// are we scanning?
extern DmOpenRef gNetDB;					// network database
extern DmOpenRef gEmailDB;					// email database
extern DmOpenRef gNSLookupDB;				// hosts file database
extern Int16 g_server;						// current server
extern hostRecord g_hosts[NUMHOSTS];		// hosts records
extern emailDBRecord g_emails[NUMTOTEMAILS];	// email records
extern Int16 g_crackpos;					// currect crack position
extern Int16 g_crackposmax;
extern Int16 g_scanpos;
extern Char **g_scanList;
extern UInt16 g_NSLookupTopRecord;
extern UInt16 g_EmailTopRecord;
extern UInt16 g_emailRecNum;
extern UInt16 g_hostRecNum;
extern UInt16 g_timeout;					// time left until detected
extern Char registered;
extern Char junk[35];
extern UInt16 g_gameovertime;
extern UInt16 g_hostaddr[NUMHOSTS];
extern Char g_passwords[NUMPASSWORDS][9];
extern UInt16 g_servertimeindex;
extern UInt16 g_servertimeout;
extern Boolean g_downloading;
extern UInt16 g_downloadindex;
extern UInt16 g_databaseobject;
extern UInt32 g_SupportedDepths;
extern UInt16 g_shutdownitems;
extern Boolean g_shuttingdown;

// Function declarations for MainForm
void * GetObjectPtr(UInt16 objectID);
void FreeCharPointer(Char *ptr);
void MainFormSetCredits(Int32 credits);
void MainFormIncCredits(Int32 credits);
void MainFormDecCredits(Int32 credits);
void MainFormLoadHostsTable();
void MainFormLoadEmailTable();
//void MainFormShowGameOver();

// Function declarations for UserForm
Boolean UserFormHandleEvent(EventType * eventP);
static void UserFormInit(FormType *frmP);

// Function declarations for SWInvForm
Boolean SWInvFormHandleEvent(EventType * eventP);
static void SWInvFormInit(FormType *frmP);

// Function declarations for CompStoreForm
Boolean CompStoreFormHandleEvent(EventType * eventP);
static void CompStoreFormInit(FormType *frmP);

// Function declarations for BlackHatForm
Boolean BlackHatFormHandleEvent(EventType * eventP);
static void BlackHatFormInit(FormType *frmP);

// Function declarations for EmailViewForm
Boolean EmailViewFormHandleEvent(EventType * eventP);
static void EmailViewFormInit(FormType *frmP);

// Function declarations for CrackForm
Boolean CrackFormHandleEvent(EventType * eventP);
static void CrackFormInit(FormType *frmP);

// Function declarations for ScanForm
Boolean ScanFormHandleEvent(EventType * eventP);
static void ScanFormInit(FormType *frmP);

// Function declarations for ICEForm
Boolean ICEFormHandleEvent(EventType * eventP);
static void ICEFormInit(FormType *frmP);

// Function declarations for ConnectForm
Boolean ConnectFormHandleEvent(EventType * eventP);
static void ConnectFormInit(FormType *frmP);

// Function declarations for ServerForm
Boolean ServerFormHandleEvent(EventType * eventP);
static void ServerFormInit(FormType *frmP);

// Function declarations for Accounts Payable Form
Boolean AccountsPayableFormHandleEvent(EventType * eventP);

// Function declarations for NSLookupForm
Boolean NSLookupFormHandleEvent(EventType * eventP);

// Function declarations for AdministrationForm
Boolean AdministrationFormHandleEvent(EventType * eventP);

// Function declarations for HostForm
Boolean HostFormHandleEvent(EventType * eventP);

// Function declarations for MOTDForm
Boolean MOTDFormHandleEvent(EventType * eventP);

// Function declarations for WelcomeForm
Boolean WelcomeFormHandleEvent(EventType * eventP);

// Function declarations for DatabaseForm
Boolean DatabaseFormHandleEvent(EventType * eventP);

// Function declarations for RegisterForm
Boolean RegisterFormHandleEvent(EventType * eventP);
Char EvalKey(UInt16 key,Char *data);
Char *regCode(Char *user, Boolean nameOnly);
void PrintRegistered();
Char RegisterFormIsRegistered();

// Function declarations for GameOverForm
Boolean GameOverFormHandleEvent(EventType * eventP);

// Function declarations for CheatForm
Boolean CheatFormHandleEvent(EventType * eventP);

// Function declarations for ShutdownForm
Boolean ShutdownFormHandleEvent(EventType * eventP);

// Function declarations for AlphaHackerDB
Err AHDBGetNetworkDatabase();
Err AHDBGetEmailDatabase();
Err AHDBGetNSLookupDatabase();
Int16 AHDBFindHostByName(Char *hostname);
Int16 AHDBFindHostByAddr(Char *address);
void AHDBInitNetworkDatabase();
void AHDBInitEmailDatabase();
void AHDBInitHostRecords();
void AHDBInitEmailRecords();
void AHDBAddEmailToDatabase(UInt16 email);

// Internal Constants

#define appFileCreator			'IP70'
#define appName					"AlphaHacker"
#define appVersionNum			0x01
#define appPrefID				0x00
#define appPrefVersionNum		0x01
#define netDBName				"AHNetDB-IP70"
#define netDBType				'DATA'
#define emailDBName				"AHEmailDB-IP70"
#define emailDBType				'DATA'
#define nslookupDBName			"AHNSLookupDB-IP70"
#define nslookupDBType			'DATA'

// REGCODE MACROS

#define MIN(x,y) ((x)<(y)?(x):(y))
// RPN String : key c 1 << i 2 / >> ^
#define ALGORITHM(key,i,c) (key ^= ((c << 1) >> (i/2)))

// Score Constants

#define SCORE_TRANSFER			25
#define SCORE_CHANGE_MOTD		50
#define SCORE_SHUTDOWN			50
#define SCORE_SOURCE_CODE		100
#define SCORE_PAYROLL			150
#define SCORE_UFO				1000
#define SCORE_UPLOAD_SOURCE		200
#define SCORE_UPLOAD_PAYROLL	250
#define SCORE_UPLOAD_UFO		2000

// Rank Constants

#define LAMER					0
#define SCRIPTKIDDIE			200
#define CRACKER					450
#define HACKER					700
#define ELITE_CRACKER			1200
#define ELITE_HACKER			2000
#define SUPER_CODER				5000
#define KING_GURU				8000

// Database Constants

#define NOSUCHRECORD			-1

// Hosts Table Columns

#define NUMHOSTTABLECOLS		4
#define STATUSCOL				0
#define ADDRESSCOL				1
#define NAMECOL					2
#define PASSWORDCOL				3

// Email Table Columns

#define NUMEMAILTABLECOLS		2
#define FROMCOL					0
#define SUBJECTCOL				1

// CPU
#define CPU200MHZ				100
#define CPU300MHZ				101
#define CPU400MHZ				102
#define CPU500MHZ				103
#define CPU650MHZ				104
#define CPU900MHZ				105
#define CPU1GHZ					106
#define CPU2GHZ					107
#define CPU35GHZ				108

// RAM
#define RAM32MB					200
#define RAM64MB					201
#define RAM128MB				202
#define RAM384MB				203
#define RAM512MB				204
#define RAM750MB				205
#define RAM1GB					206
#define RAM2GB					207
#define RAM5GB					208

// OS
#define OSZDOS					300
#define OSWINBLOWS				301
#define OSBLACKHAT1				302
#define OSBLACKHAT2				303
#define OSBLACKHATPRO			304
#define OSKORNIX				305

// SCANNER
#define SCANNERSNOOP			400
#define SCANNERSNIFF2			401
#define SCANNERSUPERSNIFF		402

// CRACKER
#define CRACKERPASSCRACK		500
#define CRACKERL337CRACK		501
#define CRACKERSUPERNOVA		502
#define CRACKERDESCRACK			503
#define CRACKERQUANTA			504

// ICE
#define ICETECHICE				600
#define ICEGOVTICE				601

#endif /* ALPHAHACKER_H_ */
