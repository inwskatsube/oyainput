/*
 * funtion.h
 */

void output_char(unsigned int code);
Boolean send_event(int type, int code, int value);
void put_loyakey(__u16 leftOyaKeyCode);
void put_royakey(__u16 rightOyaKeyCode);
Boolean is_imeon();
Boolean exist_previous();
void put_romaji(ROMAJI_INFO ri);
int trim(char *s);
Boolean exist_file(char *path);
Boolean find_kbdevent_info(KBDDEVINFO *devs, int* devcnt, int maxdevs);
