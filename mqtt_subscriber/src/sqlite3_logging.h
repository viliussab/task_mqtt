#ifndef MESSAGE_LOGGING_H
#define MESSAGE_LOGGING_H

int sqlite3_messaging_ctx_create();
void sqlite3_messaging_ctx_close();

int save_message_to_db(const char* message, const char* topic);

#endif