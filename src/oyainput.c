#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <signal.h>
#include <pwd.h>

#include "common.h"
#include "oyainput.h"
#include "functions.h"
#include "config.h"
#include "oyastate.h"

#define MSG_PAUSED  "\royainput:    paused."
#define MSG_RESTART "\royainput: restarted."



static int do_terminate = 0;
static char devpath[BUFSIZE+1] = {};
static int fdo = 0;
static int paused = 0;
static int imtype = 0; // (0: none, 1:fcitx, 2:ibus, 3:uim)
static __u16 on_keycode = 0;
static __u16 off_keycode = 0;
static char keyboardname[BUFSIZE+1] = {};

int get_kbdevie_output() {
	return fdo;
}

void set_keyboardname(char* name) {
	strncpy(keyboardname, name, BUFSIZE);
}

const char* get_keyboardname() {
	return keyboardname;
}

int get_imtype() {
	return imtype;
}

void set_imtype(char* imname) {
	if(strcasecmp(imname, "fcitx")==0){
		imtype = 1;
	} else if(strcasecmp(imname, "ibus")==0){
		imtype = 2;
	} else if(strcasecmp(imname, "uim")==0){
		imtype = 3;
	} else if (strcasecmp(imname, "auto")==0){
		//nothing to do.
	} else {
		imtype = 0;
	}
}

void set_imtype_default() {
	char GTK_IM_MODULE[BUFSIZE] = {};
	char QT_IM_MODULE[BUFSIZE] = {};
	char XMODIFIERS[BUFSIZE] = {};
	if (getenv("GTK_IM_MODULE")) {
		strncpy(GTK_IM_MODULE, getenv("GTK_IM_MODULE"), BUFSIZE);
	}
	if (getenv("QT_IM_MODULE")) {
		strncpy(QT_IM_MODULE, getenv("QT_IM_MODULE"), BUFSIZE);
	}
	if (getenv("XMODIFIERS")) {
		strncpy(XMODIFIERS, getenv("XMODIFIERS"), BUFSIZE);
	}
	if (strncasecmp(GTK_IM_MODULE, "fcitx", BUFSIZE) == 0 ||
		strncasecmp(QT_IM_MODULE, "fcitx", BUFSIZE) == 0 ||
		strncasecmp(XMODIFIERS, "@im=fcitx", BUFSIZE) == 0
	) {
		printf("IM auto-detect: fcitx\n");
		imtype = 1;
	} else if(strncasecmp(GTK_IM_MODULE, "ibus",BUFSIZE) == 0 ||
		strncasecmp(QT_IM_MODULE, "ibus",BUFSIZE) == 0 ||
		strncasecmp(XMODIFIERS, "@im=ibus",BUFSIZE) == 0) {
		printf("IM auto-detect: ibus\n");
		imtype = 2;
	} else if(strncasecmp(GTK_IM_MODULE, "uim",BUFSIZE) == 0 ||
		strncasecmp(QT_IM_MODULE, "uim",BUFSIZE) == 0 ||
		strncasecmp(XMODIFIERS, "@im=uim",BUFSIZE) == 0) {
		printf("IM auto-detect: uim\n");
		imtype = 3;
	}
}

void set_onkey(__u16 kc) {
	on_keycode = kc;
}

void set_offkey(__u16 kc){
	off_keycode = kc;
}

void set_inputdevice_path(char* new_devpath) {
	printf("keyboard device: %s\n", new_devpath);
	strncpy(devpath, new_devpath, BUFSIZE-1);
}

void on_sigterm(int signal) {
	UNUSED_VARIABLE(signal);
	printf("\royainput terminated.\n");
	do_terminate = 1;
}

void on_sigstop(int signal) {
	UNUSED_VARIABLE(signal);
	printf(MSG_PAUSED);
	paused = 1;
}

void on_sigrestart(int signal) {
	UNUSED_VARIABLE(signal);
	printf(MSG_RESTART);
	paused = 0;
}

void on_sigtoggle(int signal) {
	UNUSED_VARIABLE(signal);
	if (paused) {
		printf(MSG_RESTART);
		paused = 0;
	} else {
		printf(MSG_PAUSED);
		paused = 1;
	}
}


void set_signal_handler() {
	sigset_t mask;
	sigemptyset(&mask);
	signal(SIGTERM, on_sigterm);
	signal(SIGINT, on_sigterm);
	signal(SIGRTMIN, on_sigstop);
	signal(SIGRTMIN+1, on_sigrestart);
	signal(SIGRTMIN+2, on_sigtoggle);
	sigaddset(&mask, SIGTERM);
	sigaddset(&mask, SIGINT);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void close_app(int fdi, int fdo) {
	ioctl(fdi, EVIOCGRAB, 0); // release hook of /dev/input/event0-9
	close(fdi);
	close(fdo);
	close_oya_state();
}

void create_user_input() {
	// global value: fdo
	fdo = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fdo == -1) {
		die("error: Failed to open uinput device event file! %s", "/dev/uinput");
	}
	if(ioctl(fdo, UI_SET_EVBIT, EV_SYN) < 0) die("error: ioctl");
	if(ioctl(fdo, UI_SET_EVBIT, EV_KEY) < 0) die("error: ioctl");
	if(ioctl(fdo, UI_SET_EVBIT, EV_MSC) < 0) die("error: ioctl");
	for(int i = 0; i < KEY_MAX; ++i)
		if(ioctl(fdo, UI_SET_KEYBIT, i) < 0) die("error: ioctl");

	struct uinput_user_dev uidev;
	memset(&uidev, 0, sizeof(uidev));
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "oyainput");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor  = 0x1;
	uidev.id.product = 0x1;
	uidev.id.version = 1;

	if(write(fdo, &uidev, sizeof(uidev)) < 0) die("error: write");
	if(ioctl(fdo, UI_DEV_CREATE) < 0) die("error: ioctl");

}

int main(int argc, char *argv[]) {

	// set console bufferring mode off.
	setvbuf(stdout, (char*)NULL, _IONBF, 0);

	// check duplicate running
	if (exist_previous()) {
		die("error: oyainput is already running!");
	}

	uid_t euid = geteuid();
	if (euid != 0) {
		die("error: Only su(root) can run this program.");
	}

	// initialize oyayubi state by default values.
	oyayubi_state_init();


	char user_name[BUFSIZE+1] = {};
	if (getenv("USER")) {
		strncpy(user_name, getenv("USER"), BUFSIZE);
	}
	if (argc >= 2) {
		strncpy(user_name, argv[1], BUFSIZE);
	}
	struct passwd *pw = getpwnam(user_name);
	if (pw==NULL) {
		die("error: Invalid user name", user_name);
	}

	set_imtype_default();

	char confpath[BUFSIZE+1] = {};
	strncpy(confpath, pw->pw_dir, BUFSIZE);
	strcat(confpath, "/.oyainputconf");

	// load config file.
	if (! exist_file(confpath)) {
		save_config(confpath);
	}

	printf("Load config: %s\n", confpath);
	if(! load_config(confpath)) {
		die("error: Cannot load config file!\n");
	}

	// try search keyboard input device event file form /dev/input/event0-9
	KBDDEVINFO devs[5];
	int devcnt;
	int usedevno = -1;
	find_kbdevent_info(devs, &devcnt, 5);
	if (devcnt == 0) {
		die("error: Cannot find keyboard device.");
	} else if (devcnt >= 2) {
		printf("multiple keyboard is detected.\n");
		if (strlen(get_keyboardname()) > 0) {
			for (int i = 0;i < devcnt; i++) {
				if (strncmp(devs[i].name, get_keyboardname(), strlen(get_keyboardname())) == 0) {
					usedevno = i;
					printf("use keyboard '%s'\n", devs[usedevno].name);
					break;
				}
			}
			if (usedevno < 0) {
				printf("keyboardname '%s' not found\n\n", get_keyboardname());
			}
		}
		if (usedevno < 0) {
			for (int i = 0;i < devcnt; i++) {
				printf("%d: %s\n", i, devs[i].name);
			}
			printf("Enter keyboard number:");
			scanf("%d", &usedevno);
		}
		if (usedevno < 0 || usedevno > devcnt - 1) {
			usedevno = 0;
		}
	} else {
		usedevno = 0;
	}
	strcpy(devpath, INPUT_EVENT_PATH);
	strcat(devpath, "event");
	strncat(devpath, devs[usedevno].devno, 2);

	if (get_imtype() == 1 &&
		0 != system("type fcitx-remote > /dev/null")) {
		die("error: fcitx is not installed!");
	} else if (get_imtype() == 2 &&
		0 != system("type ibus > /dev/null")) {
		die("error: ibus is not installed!");
	}

	create_infotables();

	int fdi = open(devpath, O_RDONLY);
	if (fdi == -1) {
		die("error: Failed to open keyboard device event file! %s", devpath);
	}
	sleep(1); // DO NOT DELETE. need to intialize ioctl
	ioctl(fdi, EVIOCGRAB, 1); // start hook keyboard device

	create_user_input();
	set_signal_handler();

	// set userid to current user(this program must be run by root privilege)
	setuid(pw->pw_uid);
	seteuid(euid);

	fd_set rfds;
	//struct timeval tv;
	struct timespec tv;
	int retval;
	Boolean ctrl_pressed  = FALSE;
	Boolean shift_pressed = FALSE;
	Boolean alt_pressed   = FALSE;
	struct input_event ie;
	OYAYUBI_EVENT oe;

	do_terminate = 0;
	printf("oyainput running...(CTRL+C to exit)\n");

	sigset_t sigset;
	sigemptyset(&sigset);

	Boolean ime_on = is_imeon(pw->pw_dir);

	__u16 pressing_key = 0;

	while(!do_terminate){

		FD_ZERO(&rfds);
		FD_SET(fdi, &rfds);

		tv.tv_sec  = 0;
		tv.tv_nsec = 1000000;

		retval = pselect(fdi+1, &rfds, NULL, NULL, &tv, &sigset);
		if(retval == -1) {
			continue;
		}
		if (retval == 0) {
			update_event_timer();
			continue;
		}

		memset(&ie, 0, sizeof(ie));
		if(read(fdi, &ie, sizeof(ie)) != sizeof(ie)) {
			close_app(fdi, fdo);
			exit( EXIT_FAILURE );
		}

		switch(ie.code) {
		case KEY_LEFTCTRL:
		case KEY_RIGHTCTRL:
		case KEY_CAPSLOCK:
			if (ie.value == 1) {
				ctrl_pressed = TRUE;
				on_otherkey_down(ie.code);
			} else if (ie.value == 0) {
				ctrl_pressed = FALSE;
				if (pressing_key != 0) {
					send_event(EV_KEY, pressing_key, 0);
					send_event(EV_SYN, SYN_REPORT, 0);
					pressing_key = 0;
				}
			}
			write(fdo, &ie, sizeof(ie));
			break;
		case KEY_LEFTSHIFT:
		case KEY_RIGHTSHIFT:
			if (ie.value == 1) {
				shift_pressed = TRUE;
				on_otherkey_down(ie.code);
			} else if (ie.value == 0) {
				shift_pressed = FALSE;
				if (pressing_key != 0) {
					send_event(EV_KEY, pressing_key, 0);
					send_event(EV_SYN, SYN_REPORT, 0);
					pressing_key = 0;
				}
			}
			write(fdo, &ie, sizeof(ie));
			break;
		case KEY_LEFTALT:
		//case KEY_RIGHTALT:
			if (ie.value == 1) {
				alt_pressed = TRUE;
				on_otherkey_down(ie.code);
			} else if (ie.value == 0) {
				alt_pressed = FALSE;
				if (pressing_key != 0) {
					send_event(EV_KEY, pressing_key, 0);
					send_event(EV_SYN, SYN_REPORT, 0);
					pressing_key = 0;
				}
			}
			write(fdo, &ie, sizeof(ie));
			break;
		case KEY_PAUSE:
			if (ie.value == 1) {
				on_otherkey_down(ie.code);
				if (paused) {
					printf(MSG_RESTART);
					paused = 0;
				} else {
					printf(MSG_PAUSED);
					paused = 1;
				}
			}
			write(fdo, &ie, sizeof(ie));
			break;
		default:

			if ( ie.type != EV_KEY) {
				write(fdo, &ie, sizeof(ie));
				break;
			}

			if (ie.value == 1) {
				if (paused && on_keycode != 0 && ie.code == on_keycode) {
					printf(MSG_RESTART);
					paused = 0;
					write(fdo, &ie, sizeof(ie));
					break;
				}
				if (! paused && off_keycode != 0 && ie.code == off_keycode) {
					printf(MSG_PAUSED);
					paused = 1;
					write(fdo, &ie, sizeof(ie));
					break;
				}
			}

			if (paused) {
				write(fdo, &ie, sizeof(ie));
				break;
			}

			if (shift_pressed || ctrl_pressed || alt_pressed) {
				// CTRL DOWN -> SPACE DOWN -> CTRL UP -> SPACE UP
				if (ie.value == 1) {
					pressing_key = ie.code;
				} else if (ie.value == 0) {
					if (ie.code == pressing_key) {
						pressing_key = 0;
					}
				}
				write(fdo, &ie, sizeof(ie));
				break;
			}

			if (! is_acceptable(ie.code)) {
				if (ie.value == 1) {
					on_otherkey_down(ie.code);
				}
				write(fdo, &ie, sizeof(ie));
				break;
			}

			if (ie.value == 1 && is_state_first()) {
				// check on key down only
				ime_on = is_imeon(pw->pw_dir);
			}

			if (! ime_on) {
				write(fdo, &ie, sizeof(ie));
				break;
			}

			memset(&oe, 0, sizeof(oe));
			oe.eventType = ET_KEYDOWN;
			oe.isRepeat = 0;
			if (ie.value == 0) {
				oe.eventType = ET_KEYUP;
			} else if (ie.value == 1) {
			} else if (ie.value == 2) {
				oe.isRepeat = 1;
			}
			oe.keyCode = ie.code;
			handle_oyayubi_event(oe);
			break;
		}
	}

	close_app(fdi,fdo);
	return EXIT_SUCCESS;
}
