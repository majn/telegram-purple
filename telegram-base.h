#ifndef __TELEGRAM_BASE_H__
#define __TELEGRAM_BASE_H__
void read_state_file (struct tgl_state *TLS);
void read_auth_file (struct tgl_state *TLS);
void write_auth_file (struct tgl_state *TLS);
void write_state_file (struct tgl_state *TLS);

void telegram_login (struct tgl_state *TLS);
PurpleConversation *chat_show (PurpleConnection *gc, int id);
int chat_add_message (struct tgl_state *TLS, struct tgl_message *M);
void chat_add_all_users (PurpleConversation *pc, struct tgl_chat *chat);
#endif
