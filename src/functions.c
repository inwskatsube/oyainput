
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <sys/stat.h>
#include <errno.h>

#include "common.h"
#include "oyainput.h"
#include "functions.h"

// ２重起動チェック
Boolean exist_previous(){
	const int BUF = 120;
    FILE* pipe;
    char buff[BUF+1];
    memset(buff, 0, BUF+1);
	char splchr[] = " ";
	char *tok;
	int pid_cnt = 0;
    
	pipe = popen("pidof oyainput", "r");
	if (pipe == NULL) {
		return FALSE;
	}
	fgets(buff, BUF, pipe);
	tok = strtok(buff, splchr);
	
	while(tok != NULL) {
		pid_cnt++;
		tok = strtok(NULL, splchr);
	}
	pclose(pipe);
	return (pid_cnt >= 2);
}


Boolean send_event(int type, int code, int value) {
	struct input_event ie;
	memset(&ie, 0, sizeof(ie));

	gettimeofday(&ie.time, NULL);
	ie.type = type;
	ie.code = code;
	ie.value = value;
	
	int fdo = get_kbdevie_output();
	if (write(fdo, &ie, sizeof(ie)) < 0) {
		return FALSE;
	}
	return TRUE;
}

void output_char(unsigned int code) {
    send_event(EV_KEY, code, 1);
    send_event(EV_KEY, code, 0);
    send_event(EV_SYN, SYN_REPORT, 0);
}

void put_romaji(ROMAJI_INFO ri) {
	if (ri.outCode1 == KEY_LEFTSHIFT) {
		send_event(EV_KEY, KEY_LEFTSHIFT, 1);

		if (ri.outCode2 != 0 ) {
			output_char(ri.outCode2);
		}
		if (ri.outCode3 != 0 ) {
			output_char(ri.outCode3);
		}

		send_event(EV_KEY, KEY_LEFTSHIFT, 0);
		send_event(EV_SYN, SYN_REPORT, 0);
	}
	else {
		if (ri.outCode1 != 0 ) {
			output_char(ri.outCode1);
		}
		if (ri.outCode2 != 0 ) {
			output_char(ri.outCode2);
		}
		if (ri.outCode3 != 0 ) {
			output_char(ri.outCode3);
		}
	}

}

void put_loyakey(__u16 leftOyaKeyCode) {
	output_char(leftOyaKeyCode);
}

void put_royakey(__u16 rightOyaKeyCode) {
	output_char(rightOyaKeyCode);
}


Boolean is_imeon(){
	if (get_imtype() != 1) {
		return TRUE;
	}

	int BUF = 2;
    FILE* pipe;
    char buff[BUF];
    buff[0] = '\0';
    buff[1] = '\0';
    
	uid_t uid = getuid();
	uid_t euid = getegid();

	seteuid(uid);
	pipe = popen("fcitx-remote", "r");
	if (pipe == NULL) {
		seteuid(euid);
		return FALSE;
	}
	fgets(buff, BUF, pipe);
	pclose(pipe);
	seteuid(euid);

	int n = atoi(buff);
	switch(n){
		case 0 : return FALSE; break;
		case 1 : return FALSE; break;
		case 2 : return TRUE; break;
		default: return FALSE;
	}
    return FALSE;
}

/*
void set_ime(Boolean on_flg) {
	uid_t uid = getuid();
	uid_t euid = getegid();
	seteuid(uid);
	if (on_flg) {
		system("fcitx-remote -c");
	} else {
		system("fcitx-remote -o");
	}
	seteuid(euid);
}
*/


int trim(char *s) {
    int i;
    int count = 0;
    if ( s == NULL ) {
        return -1;
    }
    i = strlen(s);

    while ( --i >= 0 && s[i] == ' ' ) count++;
    s[i+1] = '\0';

    i = 0;
    while ( s[i] != '\0' && s[i] == ' ' ) i++;
    strcpy(s, &s[i]);
    return i + count;
}

Boolean exist_file(char *path)
{
	struct stat st;
	if(stat(path, &st) != 0){
		return FALSE;
	}
	mode_t m = st.st_mode;
	if(S_ISDIR(m)){
		return FALSE;
	}
	return TRUE;
}

Boolean find_kbdevent_path(char *path) {
	char cmd[] = GREP_CMD " -E 'Handlers|EV=' /proc/bus/input/devices | "\
		GREP_CMD " -B1 'EV=1[02]001[3Ff]' | "\
		GREP_CMD " -Eo 'event[0-9]+'";
    FILE* pipe;
	char buff[BUFSIZE+1] = {};
	char *sp;
    
	pipe = popen(cmd, "r");
	if (pipe == NULL) {
		return FALSE;
	}
	fgets(buff, BUFSIZE, pipe);
	pclose(pipe);
	
    sp = strstr(buff,"event");
    if ((sp - buff) + 5 >= BUFSIZE) {
		return FALSE;
	}

	//char devch = sp[5];
	strcpy(path, INPUT_EVENT_PATH);
	strcat(path, "event");
	strncat(path, sp+5, 1);

	return TRUE;
}

