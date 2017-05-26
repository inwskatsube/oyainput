/*
 * common.h
 */

#ifndef Boolean
#define Boolean int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define BUFSIZE 1024

// Moji definition
#define MOJI_UNDEF 0
#define MOJI_A 1
#define MOJI_I 2
#define MOJI_U 3
#define MOJI_E 4
#define MOJI_O 5
#define MOJI_KA 6
#define MOJI_KI 7
#define MOJI_KU 8
#define MOJI_KE 9
#define MOJI_KO 10
#define MOJI_SA 11
#define MOJI_SI 12
#define MOJI_SU 13
#define MOJI_SE 14
#define MOJI_SO 15
#define MOJI_TA 16
#define MOJI_TI 17
#define MOJI_TU 18
#define MOJI_TE 19
#define MOJI_TO 20
#define MOJI_NA 21
#define MOJI_NI 22
#define MOJI_NU 23
#define MOJI_NE 24
#define MOJI_NO 25
#define MOJI_HA 26
#define MOJI_HI 27
#define MOJI_HU 28
#define MOJI_HE 29
#define MOJI_HO 30
#define MOJI_MA 31
#define MOJI_MI 32
#define MOJI_MU 33
#define MOJI_ME 34
#define MOJI_MO 35
#define MOJI_YA 36
#define MOJI_YI 37
#define MOJI_YU 38
#define MOJI_YE 39
#define MOJI_YO 40
#define MOJI_RA 41
#define MOJI_RI 42
#define MOJI_RU 43
#define MOJI_RE 44
#define MOJI_RO 45
#define MOJI_GA 46
#define MOJI_GI 47
#define MOJI_GU 48
#define MOJI_GE 49
#define MOJI_GO 50
#define MOJI_ZA 51
#define MOJI_ZI 52
#define MOJI_ZU 53
#define MOJI_ZE 54
#define MOJI_ZO 55
#define MOJI_DA 56
#define MOJI_DI 57
#define MOJI_DU 58
#define MOJI_DE 59
#define MOJI_DO 60
#define MOJI_BA 61
#define MOJI_BI 62
#define MOJI_BU 63
#define MOJI_BE 64
#define MOJI_BO 65
#define MOJI_PA 66
#define MOJI_PI 67
#define MOJI_PU 68
#define MOJI_PE 69
#define MOJI_PO 70
#define MOJI_WA 71
#define MOJI_WI 72
#define MOJI_WU 73
#define MOJI_WE 74
#define MOJI_WO 75
#define MOJI_XA 76
#define MOJI_XI 77
#define MOJI_XU 78
#define MOJI_XE 79
#define MOJI_XO 80
#define MOJI_XYA 81
#define MOJI_XYI 82
#define MOJI_XYU 83
#define MOJI_XYE 84
#define MOJI_XYO 85
#define MOJI_XTU 86
#define MOJI_NN 87
#define MOJI_VU 88
#define MOJI_DEL 89
#define MOJI_BS 90
#define MOJI_QUESTION 91
#define MOJI_SLASH 92
#define MOJI_TILDE 93
#define MOJI_LKAGI 94
#define MOJI_RKAGI 95
#define MOJI_LBRACKET 96
#define MOJI_RBRACKET 97
#define MOJI_LPAREN 98
#define MOJI_RPAREN 99
#define MOJI_0 100
#define MOJI_1 101
#define MOJI_2 102
#define MOJI_3 103
#define MOJI_4 104
#define MOJI_5 105
#define MOJI_6 106
#define MOJI_7 107
#define MOJI_8 108
#define MOJI_9 109
#define MOJI_MINUS 110
#define MOJI_PERIOD 111
#define MOJI_COMMA 112
#define MOJI_KUTEN 113
#define MOJI_KUTOUTEN 114
#define MOJI_NAKAGURO 115
#define MOJI_DAKUTEN 116
#define MOJI_HANDAKUTEN 117
#define MOJI_LKAKKO 118
#define MOJI_RKAKKO 119
#define MOJI_MAX 119 // last index

// Oyayubi State
#define STATE_FIRST 1
#define STATE_MOJIHOLD 2
#define STATE_OYAHOLD 3
#define STATE_REPEAT 4
#define STATE_MOJIOYAHOLD 5

// Oyayubi Event Type
#define ET_KEYDOWN 1
#define ET_KEYUP 2
#define ET_TIMER 3

#define LEFTOYA 1
#define RIGHTOYA 2

#define IMTYPE_NONE 0
#define IMTYPE_FCITX 1
#define IMTYPE_IBUS 2
#define IMTYPE_UIM 3

typedef struct keymapinfo {
	__u16 keyCode;
	int moji;
} KEYMAP_INFO;

typedef struct romajiinfo {
	__u16 outCode1;
	__u16 outCode2;
	__u16 outCode3;
} ROMAJI_INFO;

typedef struct OyayubiEvent {
	int eventType;
	__u16 keyCode;
	int isRepeat;
} OYAYUBI_EVENT;

typedef struct KeyNameCode {
	char *name;
	__u16 value;
} KEYNAMECODE;

typedef struct MojiNameCode {
	char *name;
	int value;
} MOJINAMECODE;

typedef struct KbdDevInfo {
    char name[BUFSIZE];
    char devno[3];
} KBDDEVINFO;


// standard path
#define INPUT_EVENT_PATH  "/dev/input/"
#define GREP_CMD "/bin/grep"

#define die(str, args...) do { \
		perror(str); \
		exit(EXIT_FAILURE); \
	} while(0)

#define UNUSED_VARIABLE(x) (void)(x)
