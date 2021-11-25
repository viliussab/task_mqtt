#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "sqlite3_logging.h"
#include <unistd.h>

#define LOG_FILE "/log/mqtt_subscriber.db"

#define PRINT_SQLITE_ERROR(MESSAGE, db) \
        fprintf(stderr, "sqlite3 error: %s\n", sqlite3_errmsg(db)); \
        fprintf(stderr, "%s\n", MESSAGE);

static int create_db(sqlite3* db)
{
    int rc = 0;
    rc = sqlite3_exec(db,
         "create table if not exists Message(" 
         "message TEXT, "
         "topic_name TEXT, "
         "datetime INTEGER"
         ")"
         ,
         NULL, // callback argument. If callback is needed replace NULL
         0,
         NULL);
    if (rc) {
        PRINT_SQLITE_ERROR("Failed to create the database table, aborting...", db);
        sqlite3_close(db);
    }
    return rc;
}

sqlite3* sqlite3_create_db()
{
    sqlite3 *db = NULL;
    int rc = 0;

    rc = sqlite3_open(LOG_FILE, &db);
    if (rc) {
        PRINT_SQLITE_ERROR("Failed while opening the database", NULL);
        sqlite3_close(db);
        return NULL;
    }
    rc = create_db(db);
    if (rc) {
        return NULL;
    }
    return db;
}

int save_message_to_db(sqlite3* db, const char* message, const char* topic)
{
    int rc = 0;

    struct sqlite3_stmt* statement;
    int message_idx;
    int topic_idx;
    int datetime_idx;

    char* sql = "insert into Message(message, topic_name, datetime) values (@message, @topic_name, @datetime)";

    rc = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);

    if (rc) {
        PRINT_SQLITE_ERROR("Failed to prepare statement", db)
        return rc;
    }

    message_idx = sqlite3_bind_parameter_index(statement, "@message");
    topic_idx = sqlite3_bind_parameter_index(statement, "@topic_name");
    datetime_idx = sqlite3_bind_parameter_index(statement, "@datetime");

    int unixtime = (int)time(NULL);

    sqlite3_bind_text(statement,    message_idx,  message,        strlen(message),        NULL);
    sqlite3_bind_text(statement,    topic_idx,    topic,          strlen(topic),          NULL);
    sqlite3_bind_int (statement,    datetime_idx, unixtime);
    
    rc = sqlite3_step(statement);
    if (rc != SQLITE_DONE) {
        PRINT_SQLITE_ERROR("The query was not executed properly", db);
    }
    rc = sqlite3_finalize(statement);
    if (rc) {
        PRINT_SQLITE_ERROR("SQL query statement deallocation error", db);
    }

    return rc;
}