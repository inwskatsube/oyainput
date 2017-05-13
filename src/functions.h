/*
 * funtion.h
 */

void output_char(unsigned int code);
Boolean send_event(int type, int code, int value);
void put_loyakey(__u16 leftOyaKeyCode);
void put_royakey(__u16 rightOyaKeyCode);
Boolean is_imeon();
Boolean find_kbdevent_path(char *path);
Boolean exist_previous();
void put_romaji(ROMAJI_INFO ri);
Boolean exist_file(char *path);
int trim(char *s);

