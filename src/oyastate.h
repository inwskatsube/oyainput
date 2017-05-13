/*
 * oyastate.h
 */

void handle_oyayubi_event(struct OyayubiEvent ev);
Boolean is_acceptable(int keycode);
void oyayubi_state_init();
void create_infotables();
void close_oya_state();
void set_left_oyakey(__u16 kc);
void set_right_oyakey(__u16 kc);
void set_nicola_time(long nicola_time);
void add_key_moji(__u16 kc, int m);
void add_left_key_moji(__u16 kc, int m);
void add_right_key_moji(__u16 kc, int m);
