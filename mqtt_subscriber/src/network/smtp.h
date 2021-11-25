#ifndef _SMTP_SENDING_H

#include "json_message_parsing.h"
#include "../datatypes.h"

void prepare_email_template(CURL *curl, const struct observable_message msg, char* incoming_value);
int send_prepared_mail(CURL* curl, const char* smtp_server, int port, const char *from_email, const char* from_email_pwd, char *to_email);

#endif
