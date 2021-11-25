#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include "smtp.h"
#include "json_message_parsing.h"

#define MAX_NUMBER_LEN 64
#define TEMPLATE_LENGTH 4096

static char payload_text[TEMPLATE_LENGTH] = {0};

void prepare_email_template(CURL *curl, const struct observable_message msg, char* incoming_value)
{
  if (curl == NULL) {
    return;
  }
  snprintf(payload_text,
    TEMPLATE_LENGTH,
    "To: %s\r\n"
    "From: %s %s\r\n"
    "Subject: \'%s\' threshold has been reached on topic %s\r\n"
    "\r\n" /* empty line to divide headers from body, see RFC5322 */
    "MQTT service has received an event trigger on topic %s.\r\n"
    "In \'%s\', value has been received: \'%s\', which %s \'%s\'.\r\n",
    msg.receiver_mail,
    msg.sender.email, msg.sender.username,
    msg.contents.key, msg.topic_name,
    msg.topic_name,
    msg.contents.key, incoming_value, operator_to_words(msg.comparison_op), msg.contents.value
  );
}

struct upload_status {
  size_t bytes_read;
};
 
static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;
  size_t room = size * nmemb;
 
  if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }
 
  data = &payload_text[upload_ctx->bytes_read];
 
  if (data) {
    size_t len = strlen(data);
    if(room < len)
      len = room;
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;
 
    return len;
  }
 
  return 0;
}

int send_prepared_mail(CURL* curl, const char* smtp_server, int port, const char *from_email, const char* from_email_pwd, char *to_email)
{
  if (curl == NULL) {
    return CURLE_FAILED_INIT;
  }
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx = { 0 };

  char smtp_url[256];
  if (strstr(smtp_server, "smtp://") != NULL) { // Contains "smtp://" in "smtp://smtp.gmail.com:587"
    snprintf(smtp_url, 256, "%s:%d", smtp_server, port);
  }
  else {
    snprintf(smtp_url, 256, "smtp://%s:%d", smtp_server, port);
  }

  curl_easy_setopt(curl, CURLOPT_URL, smtp_url);
  curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

  curl_easy_setopt(curl, CURLOPT_USERNAME, from_email);
  curl_easy_setopt(curl, CURLOPT_PASSWORD, from_email_pwd);

  curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from_email);

  recipients = curl_slist_append(recipients, to_email);
  curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

  curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
  curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
  curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

  /* Send the message */
  res = curl_easy_perform(curl);

  /* Check for errors */
  if(res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));

  curl_slist_free_all(recipients);
  
  return (int)res;
}