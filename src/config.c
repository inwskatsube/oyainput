#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/uinput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include "common.h"
#include "oyainput.h"
#include "oyastate.h"
#include "config.h"
#include "functions.h"


KEYNAMECODE keyNameCodeTable[] = {
	{"ESC", KEY_ESC},
	{"1", KEY_1},
	{"2", KEY_2},
	{"3", KEY_3},
	{"4", KEY_4},
	{"5", KEY_5},
	{"6", KEY_6},
	{"7", KEY_7},
	{"8", KEY_8},
	{"9", KEY_9},
	{"0", KEY_0},
	{"MINUS", KEY_MINUS},
	{"EQUAL", KEY_EQUAL},
	{"BACKSPACE", KEY_BACKSPACE},
	{"TAB", KEY_TAB},
	{"Q", KEY_Q},
	{"W", KEY_W},
	{"E", KEY_E},
	{"R", KEY_R},
	{"T", KEY_T},
	{"Y", KEY_Y},
	{"U", KEY_U},
	{"I", KEY_I},
	{"O", KEY_O},
	{"P", KEY_P},
	{"LEFTBRACE", KEY_LEFTBRACE},
	{"RIGHTBRACE", KEY_RIGHTBRACE},
	{"ENTER", KEY_ENTER},
	{"LEFTCTRL", KEY_LEFTCTRL},
	{"A", KEY_A},
	{"S", KEY_S},
	{"D", KEY_D},
	{"F", KEY_F},
	{"G", KEY_G},
	{"H", KEY_H},
	{"J", KEY_J},
	{"K", KEY_K},
	{"L", KEY_L},
	{"SEMICOLON", KEY_SEMICOLON},
	{"APOSTROPHE", KEY_APOSTROPHE},
	{"GRAVE", KEY_GRAVE},
	{"LEFTSHIFT", KEY_LEFTSHIFT},
	{"BACKSLASH", KEY_BACKSLASH},
	{"Z", KEY_Z},
	{"X", KEY_X},
	{"C", KEY_C},
	{"V", KEY_V},
	{"B", KEY_B},
	{"N", KEY_N},
	{"M", KEY_M},
	{"COMMA", KEY_COMMA},
	{"DOT", KEY_DOT},
	{"SLASH", KEY_SLASH},
	{"RIGHTSHIFT", KEY_RIGHTSHIFT},
	{"KPASTERISK", KEY_KPASTERISK},
	{"LEFTALT", KEY_LEFTALT},
	{"SPACE", KEY_SPACE},
	{"CAPSLOCK", KEY_CAPSLOCK},
	{"F1", KEY_F1},
	{"F2", KEY_F2},
	{"F3", KEY_F3},
	{"F4", KEY_F4},
	{"F5", KEY_F5},
	{"F6", KEY_F6},
	{"F7", KEY_F7},
	{"F8", KEY_F8},
	{"F9", KEY_F9},
	{"F10", KEY_F10},
	{"NUMLOCK", KEY_NUMLOCK},
	{"SCROLLLOCK", KEY_SCROLLLOCK},
	{"KP7", KEY_KP7},
	{"KP8", KEY_KP8},
	{"KP9", KEY_KP9},
	{"KPMINUS", KEY_KPMINUS},
	{"KP4", KEY_KP4},
	{"KP5", KEY_KP5},
	{"KP6", KEY_KP6},
	{"KPPLUS", KEY_KPPLUS},
	{"KP1", KEY_KP1},
	{"KP2", KEY_KP2},
	{"KP3", KEY_KP3},
	{"KP0", KEY_KP0},
	{"KPDOT", KEY_KPDOT},
	{"ZENKAKUHANKAKU", KEY_ZENKAKUHANKAKU},
	{"102ND", KEY_102ND},
	{"F11", KEY_F11},
	{"F12", KEY_F12},
	{"RO", KEY_RO},
	{"KATAKANA", KEY_KATAKANA},
	{"HIRAGANA", KEY_HIRAGANA},
	{"HENKAN", KEY_HENKAN},
	{"KATAKANAHIRAGANA", KEY_KATAKANAHIRAGANA},
	{"MUHENKAN", KEY_MUHENKAN},
	{"KPJPCOMMA", KEY_KPJPCOMMA},
	{"KPENTER", KEY_KPENTER},
	{"RIGHTCTRL", KEY_RIGHTCTRL},
	{"KPSLASH", KEY_KPSLASH},
	{"SYSRQ", KEY_SYSRQ},
	{"RIGHTALT", KEY_RIGHTALT},
	{"LINEFEED", KEY_LINEFEED},
	{"HOME", KEY_HOME},
	{"UP", KEY_UP},
	{"PAGEUP", KEY_PAGEUP},
	{"LEFT", KEY_LEFT},
	{"RIGHT", KEY_RIGHT},
	{"END", KEY_END},
	{"DOWN", KEY_DOWN},
	{"PAGEDOWN", KEY_PAGEDOWN},
	{"INSERT", KEY_INSERT},
	{"DELETE", KEY_DELETE},
	{"MUTE", KEY_MUTE},
	{"VOLUMEDOWN", KEY_VOLUMEDOWN},
	{"VOLUMEUP", KEY_VOLUMEUP},
	{"POWER", KEY_POWER},
	{"PAUSE", KEY_PAUSE},
	{"YEN", KEY_YEN},
	{"LEFTMETA", KEY_LEFTMETA},
	{"RIGHTMETA", KEY_RIGHTMETA},
	{"COMPOSE", KEY_COMPOSE},
	{"STOP", KEY_STOP},
	{"PROPS", KEY_PROPS},
	{"UNDO", KEY_UNDO},
	{"FRONT", KEY_FRONT},
	{"COPY", KEY_COPY},
	{"OPEN", KEY_OPEN},
	{"PASTE", KEY_PASTE},
	{"FIND", KEY_FIND},
	{"CUT", KEY_CUT},
	{"HELP", KEY_HELP},
	{"MENU", KEY_MENU},
	{"CALC", KEY_CALC},
	{"SETUP", KEY_SETUP},
	{"SLEEP", KEY_SLEEP},
	{"WAKEUP", KEY_WAKEUP},
	{"XFER", KEY_XFER},
	{"SCREENLOCK", KEY_SCREENLOCK},
	{"SCROLLUP", KEY_SCROLLUP},
	{"SCROLLDOWN", KEY_SCROLLDOWN},
	{"KPLEFTPAREN", KEY_KPLEFTPAREN},
	{"KPRIGHTPAREN", KEY_KPRIGHTPAREN},
	{"F13", KEY_F13},
	{"F14", KEY_F14},
	{"F15", KEY_F15},
	{"F16", KEY_F16},
	{"F17", KEY_F17},
	{"F18", KEY_F18},
	{"F19", KEY_F19},
	{"F20", KEY_F20},
	{"F21", KEY_F21},
	{"F22", KEY_F22},
	{"F23", KEY_F23},
	{"F24", KEY_F24}
};

MOJINAMECODE mojiNameCodeTable[] = {
	{"NONE",1},
	{"A",1},
	{"I",2},
	{"U",3},
	{"E",4},
	{"O",5},
	{"KA",6},
	{"KI",7},
	{"KU",8},
	{"KE",9},
	{"KO",10},
	{"SA",11},
	{"SI",12},
	{"SU",13},
	{"SE",14},
	{"SO",15},
	{"TA",16},
	{"TI",17},
	{"TU",18},
	{"TE",19},
	{"TO",20},
	{"NA",21},
	{"NI",22},
	{"NU",23},
	{"NE",24},
	{"NO",25},
	{"HA",26},
	{"HI",27},
	{"HU",28},
	{"HE",29},
	{"HO",30},
	{"MA",31},
	{"MI",32},
	{"MU",33},
	{"ME",34},
	{"MO",35},
	{"YA",36},
	{"YI",37},
	{"YU",38},
	{"YE",39},
	{"YO",40},
	{"RA",41},
	{"RI",42},
	{"RU",43},
	{"RE",44},
	{"RO",45},
	{"GA",46},
	{"GI",47},
	{"GU",48},
	{"GE",49},
	{"GO",50},
	{"ZA",51},
	{"ZI",52},
	{"ZU",53},
	{"ZE",54},
	{"ZO",55},
	{"DA",56},
	{"DI",57},
	{"DU",58},
	{"DE",59},
	{"DO",60},
	{"BA",61},
	{"BI",62},
	{"BU",63},
	{"BE",64},
	{"BO",65},
	{"PA",66},
	{"PI",67},
	{"PU",68},
	{"PE",69},
	{"PO",70},
	{"WA",71},
	{"WI",72},
	{"WU",73},
	{"WE",74},
	{"WO",75},
	{"XA",76},
	{"XI",77},
	{"XU",78},
	{"XE",79},
	{"XO",80},
	{"XYA",81},
	{"XYI",82},
	{"XYU",83},
	{"XYE",84},
	{"XYO",85},
	{"XTU",86},
	{"NN",87},
	{"VU",88},
	{"DEL",89},
	{"BS",90},
	{"QUESTION",91},
	{"SLASH",92},
	{"TILDE",93},
	{"LKAGI",94l},
	{"RKAGI",95},
	{"LBRACKET",96},
	{"RBRACKET",97},
	{"LPAREN",98},
	{"RPAREN",99},
	{"0",100},
	{"1",101},
	{"2",102},
	{"3",103},
	{"4",104},
	{"5",105},
	{"6",106},
	{"7",107},
	{"8",108},
	{"9",109},
	{"MINUS",110},
	{"PERIOD",111},
	{"COMMA",112},
	{"KUTEN",113},
	{"KUTOUTEN",114},
	{"NAKAGURO",115},
	{"DAKUTEN",116},
	{"HANDAKUTEN",117}
};


Boolean load_config(char* path)
{
	FILE *fp;
	char line[128];
	char *str;
	char key[128];
	char value[128];
	__u16 kc;
	int mc;
	char keyname[128];
	char mojiname[128];

	if ((fp = fopen(path, "r")) == NULL) {
		return FALSE;
	}

	while ((str = fgets(line, 128, fp)) != NULL) {
		key[0] = 0;
		value[0] = 0;

		trim(line);
		if(strcmp(line,"\n")==0) continue;
		if(strncmp(line,"#",1)==0) continue;

		sscanf(line, "%[^=]=%s", key, value);
		//printf("key = %s, value = %s \n", key, value);
		if (strcasecmp(key,"DEVFILENO")==0) {
			if (strlen(value) > 0 ) {
				char devpath[120] = INPUT_EVENT_PATH;
				strcat(devpath, "event");
				strcat(devpath, value);
				set_inputdevice_path(devpath);
			}
		} else if (strcasecmp(key, "LOYAKEY")==0) {
			kc = keyname_to_code(value);
			if (kc != 0) {
				printf("LOYAKEY: %s\n", value);
				set_left_oyakey(kc);
			}
		} else if (strcasecmp(key, "ROYAKEY")==0) {
			kc = keyname_to_code(value);
			if (kc != 0) {
				printf("ROYAKEY: %s\n", value);
				set_right_oyakey(kc);
			}
		} else if (strcasecmp(key, "ONKEY")==0) {
			kc = keyname_to_code(value);
			if (kc != 0) {
				printf("ONKEY: %s\n", value);
				set_onkey(kc);
			}
		} else if (strcasecmp(key, "OFFKEY")==0) {
			kc = keyname_to_code(value);
			if (kc != 0) {
				printf("OFFKEY: %s\n", value);
				set_offkey(kc);
			}
		} else if (strcasecmp(key, "IM")==0) {
			printf("IM: %s\n", value);
			set_imtype(value);
		}else if (strcasecmp(key, "CHARTIME")==0) {
			printf("CHARTIME: %s\n", value);
			set_char_time(atoi(value));
		}else if (strcasecmp(key, "OYATIME")==0) {
			printf("OYATIME: %s\n", value);
			set_oya_time(atoi(value));
		}else if (strcasecmp(key, "NICOLATIME")==0) {
			printf("NICOLATIME: %s\n", value);
			set_nicola_time(atoi(value));
		} else if (strcasecmp(key, "KEYADD")==0) {
			sscanf(value, "%[^:]:%s", keyname, mojiname);
			kc = keyname_to_code(keyname);
			mc = mojiname_to_code(mojiname);
			if (kc != 0) {
				printf("KEYADD: %s=%s\n", keyname, mojiname);
				add_key_moji(kc, mc);
			}
		} else if (strcasecmp(key, "LKEYADD")==0) {
			sscanf(value, "%[^:]:%s", keyname, mojiname);
			kc = keyname_to_code(keyname);
			mc = mojiname_to_code(mojiname);
			if (kc != 0) {
				printf("LKEYADD: %s=%s\n", keyname, mojiname);
				add_left_key_moji(kc, mc);
			}

		} else if (strcasecmp(key, "RKEYADD")==0) {
			sscanf(value, "%[^:]:%s", keyname, mojiname);
			kc = keyname_to_code(keyname);
			mc = mojiname_to_code(mojiname);
			if (kc != 0) {
				printf("RKEYADD: %s=%s\n", keyname, mojiname);
				add_right_key_moji(kc, mc);
			}
		}
	}
	fclose(fp);
	return TRUE;
}

Boolean save_config(char *path)
{
	FILE *fp;
	if ((fp = fopen(path, "w")) == NULL) {
		return FALSE;
	}
	fprintf(fp, "# キーボードのイベントデバイスファイルの番号。\n");
	fprintf(fp, "# 複数キーボードがある場合に指定可能。\n");
	fprintf(fp, "# (cat /proc/bus/input/devices で確認可能）\n");
	fprintf(fp, "#DEVFILENO=\n");
	fprintf(fp, "\n");
	fprintf(fp, "# 左親指キー　(スペースキー＝SPACE, 無変換キー＝MUHENKAN)\n");
	fprintf(fp, "LOYAKEY=SPACE\n");
	fprintf(fp, "\n");
	fprintf(fp, "# 右親指キー　(変換キー＝HENKAN,ひらがなカタカナキー＝KATAKANAHIRAGANA)\n");
	fprintf(fp, "ROYAKEY=HENKAN\n");
	fprintf(fp, "\n");
	fprintf(fp, "# ON状態とOFF状態を連携させるIM(fcitx/ibus/auto/none)を指定。\n");
	fprintf(fp, "#IM=auto\n");
	fprintf(fp, "\n");
	fprintf(fp, "# 日本語 ON/OFFキー\n");
	fprintf(fp, "#ONKEY=RIGHTALT\n");
	fprintf(fp, "#OFFKEY=RIGHTALT\n");
	fprintf(fp, "\n");
	fprintf(fp, "# 親文字->親指同時打鍵検出許容期間(ms)\n");
	fprintf(fp, "#CHARTIME=200\n");
	fprintf(fp, "\n");
	fprintf(fp, "# 親指->文字同時打鍵検出許容期間(ms)\n");
	fprintf(fp, "#OYATIME=200\n");
	fprintf(fp, "\n");
	fprintf(fp, "# 親指単独打鍵みなし期間(ms)\n");
	fprintf(fp, "#NICOLATIME=50\n");
	fprintf(fp, "\n");

	fclose(fp);

	chmod(path, 0666);
	return TRUE;
}


__u16 keyname_to_code(char* key_name){
	int len = sizeof(keyNameCodeTable) / sizeof(KEYNAMECODE);
	KEYNAMECODE* p = keyNameCodeTable;
	for(int i = 0; i < len; i++, p++) {
		if (strcmp(key_name, p->name)==0) {
			return p->value;
		}
	}
	if(key_name != NULL && *key_name != 0) {
		printf("Unknown key name, %s\n", key_name);
	}
	return 0;
}

int mojiname_to_code(char* moji_name){
	int len = sizeof(mojiNameCodeTable) / sizeof(MOJINAMECODE);
	MOJINAMECODE* p = mojiNameCodeTable;
	for(int i = 0; i < len; i++, p++) {
		if (strcmp(moji_name, p->name)==0) {
			return p->value;
		}
	}
	if(moji_name != NULL && *moji_name != 0) {
		printf("Unknown moji name, %s\n", moji_name);
	}
	return 0;
}
