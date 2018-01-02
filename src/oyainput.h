/*
 * oyainput.h
 */

int get_kbdevie_output();
void set_keyboardname(char* value);
const char* get_keyboardname();
void set_imtype(char* imname);
int get_imtype();
void set_inputdevice_path(char * new_devpath);
void set_onkey(__u16 kc);
void set_offkey(__u16 kc);

