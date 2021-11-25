#ifndef MESSAGE_LOGGING_H
#define MESSAGE_LOGGING_H

#include <sqlite3.h>

sqlite3* sqlite3_create_db();
int save_message_to_db(sqlite3* db, const char* message, const char* topic);

#endif