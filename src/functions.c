
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

#include "common.h"
#include "oyainput.h"
#include "functions.h"

static char cmd_buffer[BUFSIZE] = {};

// check duplicate process
Boolean exist_previous(){
	const int BUF = 120;
	char buff[BUF+1];
	memset(buff, 0, BUF+1);
	char splchr[] = " ";
	char *tok;
	int pid_cnt = 0;

	FILE* pipe = popen("pidof oyainput", "r");
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

		if (ri.outCode2 == KEY_LEFTSHIFT) {
			send_event(EV_KEY, KEY_LEFTSHIFT, 1);
			if (ri.outCode3 != 0 ) {
				output_char(ri.outCode3);
			}
			send_event(EV_KEY, KEY_LEFTSHIFT, 0);
			send_event(EV_SYN, SYN_REPORT, 0);
		} else {
			if (ri.outCode2 != 0 ) {
				output_char(ri.outCode2);
			}
			if (ri.outCode3 != 0 ) {
				output_char(ri.outCode3);
			}
		}
	}

}

void put_loyakey(__u16 leftOyaKeyCode) {
	output_char(leftOyaKeyCode);
}

void put_royakey(__u16 rightOyaKeyCode) {
	output_char(rightOyaKeyCode);
}

Boolean is_fcitx_on(char *home_dir) {
	uid_t uid = getuid();
	uid_t euid = getegid();

	char cmd[BUFSIZE];
	if (snprintf(cmd, BUFSIZE, "HOME=%s fcitx-remote", home_dir) > BUFSIZE) {
		die("error: Username is too long.");
	}

	seteuid(uid);
	FILE* pipe = popen(cmd, "r");
	if (pipe == NULL) {
		seteuid(euid);
		return FALSE;
	}
	memset(cmd_buffer, 0, BUFSIZE);
	fgets(cmd_buffer, BUFSIZE, pipe);
	pclose(pipe);
	seteuid(euid);

	int n = atoi(cmd_buffer);
	switch(n){
		case 0 : return FALSE; break;
		case 1 : return FALSE; break;
		case 2 : return TRUE; break;
		default: return FALSE;
	}
	return FALSE;
}

Boolean is_ibus_on() {
	uid_t uid = getuid();
	uid_t euid = getegid();

	seteuid(uid);
	FILE* pipe = popen("ibus engine", "r");
	if (pipe == NULL) {
		seteuid(euid);
		return FALSE;
	}
	memset(cmd_buffer, 0, BUFSIZE);
	fgets(cmd_buffer, BUFSIZE, pipe);
	pclose(pipe);
	seteuid(euid);

	if (strncmp(cmd_buffer, "xkb:", 4) == 0) {
		return FALSE;
	}
	if (strlen(cmd_buffer) > 0) {
		return TRUE;
	}
	return FALSE;
}

Boolean is_imeon(char *home_dir){
	int imtype = get_imtype();
	if (imtype == 1) {
		return is_fcitx_on(home_dir);
	}else if (imtype == 2) {
		return is_ibus_on();
	}
	return TRUE;
}

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

Boolean exist_file(char *path) {
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

Boolean find_kbdevent_info(KBDDEVINFO *devs, int* devcnt, int maxdevs) {
	char cmd[] = GREP_CMD " -E 'Name=|Handlers|EV=' /proc/bus/input/devices | "\
		GREP_CMD " -B2 -- 'EV=1[02]001[3Ff]' | " GREP_CMD " -v '^--$'";

	char buff1[BUFSIZE+1] = {};
	char buff2[BUFSIZE+1] = {};
	char buff3[BUFSIZE+1] = {};
	memset(devs, 0, sizeof(KBDDEVINFO) * maxdevs);
	KBDDEVINFO *devp = devs;

	char *sp;
	*devcnt = 0;

	FILE* pipe = popen(cmd, "r");
	if (pipe == NULL) {
		return FALSE;
	}
	do {
		memset(buff1, 0, BUFSIZE);
		memset(buff2, 0, BUFSIZE);
		memset(buff3, 0, BUFSIZE);
		if (! fgets(buff1, BUFSIZE, pipe)) break;
		if (! fgets(buff2, BUFSIZE, pipe)) break;
		if (! fgets(buff3, BUFSIZE, pipe)) break;

		sp = strstr(buff1, "Name=");
		if (! sp) break;
		strncpy(devp->name, sp+6, strlen(sp+6)-2); // 2 means double quote + line feed

		sp = strstr(buff2, "event");
		if (! sp) break;
		strncpy(devp->devno, sp + 5, 2);
		if (!isdigit(devp->devno[1])) {
			devp->devno[1] = 0;
		}
		*devcnt = (*devcnt) + 1;
		devp++;
	} while(devp - devs < maxdevs);

	pclose(pipe);
	return TRUE;
}
