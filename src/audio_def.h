#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(11295)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(3840)},  //XA_GameOver
	//WIIA.XA
	{XA_WiiA, XA_LENGTH(16800)}, //Dream of peace
	{XA_WiiA, XA_LENGTH(12300)}, //Diagraphephobia
	//WIIB.XA
	{XA_WiiB, XA_LENGTH(11800)}, //Post Mortal
	{XA_WiiB, XA_LENGTH(16700)}, //Plaything
	//WIIC.XA
	{XA_WiiC, XA_LENGTH(13400)},
	{XA_WiiC, XA_LENGTH(800)},
	//CUT.XA
	{XA_Cut, XA_LENGTH(600)},
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\WIIA.XA;1",
	"\\MUSIC\\WIIB.XA;1",
	"\\MUSIC\\WIIC.XA;1",
	"\\MUSIC\\CUT.XA;1",
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	//WIIA.XA
	{"dreamofpeace", true},
	{"diagraphephobia", true},
	//WIIB.XA
	{"postmortal", true},
	{"plaything", true},
	//WIIC.XA
	{"systemfailure", true},
	{"banan", false},
	//CUT.XA
	{"gottem", false},
	
	{NULL, false}
};
