#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <tidy.h>
#include <tidybuffio.h>
#include <mongoc/mongoc.h>
#include "index_page_scraper.h"

size_t current_subject_index = 0;

size_t subject_index_buffer_callback(
  char* buffer,
  size_t size,
  size_t num_members,
  TidyBuffer* tidy_buffer) {
    size_t new_size = size * num_members;
    tidyBufAppend(tidy_buffer, buffer, new_size);
    return new_size;
};

void parse_node_for_href(TidyNode node, char** output) {
  TidyNode child;
  for (child = tidyGetChild(node); child != NULL; child = tidyGetNext(child)) {
    TidyAttr href_attr = tidyAttrGetById(child, TidyAttr_HREF);
    if (href_attr) {
      if (current_subject_index < MAX_SUBJECT_INFO_NUM) {
        if (
          strlen(tidyAttrValue(href_attr)) < MAX_URL_LEN &&
          strstr(tidyAttrValue(href_attr), COURSE_CODE_LINK)
        ) {
          strcpy(output[current_subject_index], tidyAttrValue(href_attr));
          current_subject_index++;
        }
      }
    }
    parse_node_for_href(child, output);
  }
}

int get_all_urls_on_page(index_page_scraper_t index_page_scraper) {
  if (index_page_scraper.url) {
    CURL* handle;
    handle = curl_easy_init();
    char err_buff[CURL_ERROR_SIZE];
    int res;
    TidyDoc parse_doc;
    TidyBuffer tidy_buffer = {0};

    if (handle) {
      curl_easy_setopt(handle, CURLOPT_URL, index_page_scraper.url); // set URL
      curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, subject_index_buffer_callback); // set output callback function
      curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, err_buff);

      parse_doc = tidyCreate();
      tidyBufInit(&tidy_buffer);
      tidyOptSetInt(parse_doc, TidyWrapLen, 2048); // set max length
      tidyOptSetBool(parse_doc, TidyForceOutput, yes); // force output

      curl_easy_setopt(handle, CURLOPT_WRITEDATA, &tidy_buffer); // identify buffer to store data in

      res = curl_easy_perform(handle); // execute request, return status code to res

      if (res == CURLE_OK) {
        printf("Parsed all course pages from: %s\n", index_page_scraper.url);

        tidyParseBuffer(parse_doc, &tidy_buffer);

        char** parsed_urls = (char**) malloc(MAX_SUBJECT_INFO_NUM * sizeof(char*));
        for (size_t i = 0; i < MAX_SUBJECT_INFO_NUM; i++) {
          index_page_scraper.subject_info[i].subject_url = (char *) malloc(MAX_URL_LEN * sizeof(char *));
          parsed_urls[i] = (char *) malloc(MAX_URL_LEN * sizeof(char *));
        }

        parse_node_for_href(tidyGetBody(parse_doc), parsed_urls); // parse results
        for (size_t i = 0; i < MAX_SUBJECT_INFO_NUM; i++) {
          index_page_scraper.subject_info[i].subject_url = parsed_urls[i];
        }
      } else {
        printf("Failed to parse course pages from: %s\n", index_page_scraper.url);
        return 0;
      }

      curl_easy_cleanup(handle);
      tidyBufFree(&tidy_buffer);
      tidyRelease(parse_doc);

      return 1;
    }
    return 0;
  }
  return 0;
}

void get_course_page_urls(index_page_scraper_t index_page_scraper, size_t* num_urls) {
  if (get_all_urls_on_page(index_page_scraper)) {
    char* url_first_part = "http://www.calendar.ubc.ca/vancouver/";
    for (size_t i = 1; i < current_subject_index; i++) {
      if (
        index_page_scraper.subject_info[i].subject_url &&
        !strcmp(index_page_scraper.subject_info[i - 1].subject_url, index_page_scraper.subject_info[i].subject_url)
      ) {
        if ((strlen(url_first_part) + strlen(index_page_scraper.subject_info[i].subject_url) - 1) > MAX_URL_LEN) {
          printf("max url length exceeded for %s%s", url_first_part, index_page_scraper.subject_info[i].subject_url);
          return;
        }
        char *course_page = (char *) malloc (strlen(url_first_part) + strlen(index_page_scraper.subject_info[i].subject_url) + 1);
        strcpy(course_page, url_first_part);
        strcat(course_page, index_page_scraper.subject_info[i].subject_url);
        strcpy(index_page_scraper.subject_info[i].subject_url, course_page);
        (*num_urls)++;
      }
    }
  }
}

void update_school(index_page_scraper_t* index_page_scraper, size_t num_urls) {
  bson_t*     document;
  bson_t      child;
  bson_t      child2;
  bson_t      child3;
  struct tm   schoolName = { 0 };
  char        buf[128];
  char        buf2[128];
  const       char *key;
  const       char *key2;
  size_t      keylen;
  size_t      keylen2;
  bson_t query = BSON_INITIALIZER;
  document = bson_new();

  BSON_APPEND_UTF8(document, "name", "UBC");
  BSON_APPEND_UTF8(&query, "name", "UBC");

  BSON_APPEND_ARRAY_BEGIN(document, "subjects", &child);
  for (size_t i = 1; i < (num_urls * 2); i+=2) {
    keylen = bson_uint32_to_string(i, &key, buf, sizeof buf);
    bson_append_document_begin(&child, key, (int) keylen, &child2);
    BSON_APPEND_UTF8(&child2, "url", index_page_scraper->subject_info[i].subject_url);

    BSON_APPEND_ARRAY_BEGIN(&child2, "codes", &child3);
    for (size_t j = 0; j < index_page_scraper->subject_info[i].num_courses; j++) {
      keylen2 = bson_uint32_to_string(j, &key2, buf2, sizeof buf2);
      bson_append_int32(&child3, key2, keylen2, index_page_scraper->subject_info[i].subject_codes[j]);
    }
    bson_append_array_end(&child2, &child3);
    bson_append_document_end(&child, &child2);
  }
  bson_append_array_end(document, &child);
  // char* str = bson_as_canonical_extended_json (document, NULL);
  // printf ("%s\n", str);
  // bson_free (str);

  mongoc_find_and_modify_opts_t *opts;
  bson_t reply;
  bson_error_t error;

  opts = mongoc_find_and_modify_opts_new();
  mongoc_find_and_modify_opts_set_update(opts, document);
  mongoc_find_and_modify_opts_set_flags(
    opts, 
    MONGOC_FIND_AND_MODIFY_UPSERT | MONGOC_FIND_AND_MODIFY_RETURN_NEW
  );
  mongoc_client_t *client = mongoc_client_new (getenv("MONGO_URI"));
  mongoc_collection_t *collection = mongoc_client_get_collection(client, "course_reqs_db", "schools");
  bool success = mongoc_collection_find_and_modify_with_opts(collection, &query, opts, &reply, &error);
  if (success) {
      char *str;
      str = bson_as_canonical_extended_json(&reply, NULL);
      printf("%s\n\n", str);
      bson_free(str);
  } else {
      fprintf (stderr, "Got error: \"%s\" on line %d\n", error.message, __LINE__);
  }

  bson_destroy (&reply);
  bson_destroy(document);
  mongoc_find_and_modify_opts_destroy (opts);
  mongoc_collection_destroy(collection);
  mongoc_client_destroy(client);
}
