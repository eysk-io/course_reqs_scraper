#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <tidy.h>
#include <tidybuffio.h>
#include "subject_index_scraper.h"

#define MAX_LINKS 1000
#define MAX_URL_LEN 512
#define COURSE_CODE_LINK "courses.cfm?page=name&code="
int current_index = 0;

size_t buffer_callback(
  char * buffer,
  size_t size,
  size_t num_members,
  TidyBuffer * tidy_buffer) {
    size_t new_size = size * num_members;
    tidyBufAppend(tidy_buffer, buffer, new_size);
    return new_size;
};

void parse_node_for_href(TidyNode node, char ** output) {
  TidyNode child;
  for (child = tidyGetChild(node); child != NULL; child = tidyGetNext(child)) {
    TidyAttr href_attr = tidyAttrGetById(child, TidyAttr_HREF);
    if (href_attr) {
      if (current_index < MAX_LINKS) {
        if (
          strlen(tidyAttrValue(href_attr)) < MAX_URL_LEN &&
          strstr(tidyAttrValue(href_attr), COURSE_CODE_LINK)
        ) {
          strcpy(output[current_index], tidyAttrValue(href_attr));
          current_index++;
        }
      }
    }
    parse_node_for_href(child, output);
  }
}

int get_all_urls_on_page(SubjectIndexScraper subject_index_scraper) {
  if (subject_index_scraper.url) {
    CURL *handle;
    handle = curl_easy_init();
    char err_buff[CURL_ERROR_SIZE];
    int res;
    TidyDoc parse_doc;
    TidyBuffer tidy_buffer = {0};

    if (handle) {
      curl_easy_setopt(handle, CURLOPT_URL, subject_index_scraper.url); // set URL
      curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, buffer_callback); // set output callback function
      curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, err_buff);

      parse_doc = tidyCreate();
      tidyBufInit(&tidy_buffer);
      tidyOptSetInt(parse_doc, TidyWrapLen, 2048); // set max length
      tidyOptSetBool(parse_doc, TidyForceOutput, yes); // force output

      curl_easy_setopt(handle, CURLOPT_WRITEDATA, &tidy_buffer); // identify buffer to store data in

      res = curl_easy_perform(handle); // execute request, return status code to res

      if (res == CURLE_OK) {
        printf("Parsed all course pages from: %s\n", subject_index_scraper.url);

        tidyParseBuffer(parse_doc, &tidy_buffer);

        for (int i = 0; i < MAX_LINKS; i ++) {
          subject_index_scraper.parsed_urls[i] = (char *) malloc(MAX_URL_LEN * sizeof(char *));
        }
        parse_node_for_href(tidyGetBody(parse_doc), subject_index_scraper.parsed_urls); // parse results
        subject_index_scraper.parsed_urls = subject_index_scraper.parsed_urls;
      } else {
        printf("Failed to parse course pages from: %s\n", subject_index_scraper.url);
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

void get_course_page_urls(SubjectIndexScraper subject_index_scraper, int *num_urls) {
  if (get_all_urls_on_page(subject_index_scraper)) {
    char* url_first_part = "http://www.calendar.ubc.ca/vancouver/";
    for (int i = 1; i < current_index; i++) {
      if (
        subject_index_scraper.parsed_urls[i] &&
        !strcmp(subject_index_scraper.parsed_urls[i - 1], subject_index_scraper.parsed_urls[i]) 
      ) {
        if ((strlen(url_first_part) + strlen(subject_index_scraper.parsed_urls[i]) - 1) > MAX_URL_LEN) {
          printf("max url length exceeded for %s%s", url_first_part, subject_index_scraper.parsed_urls[i]);
          return;
        }
        char *course_page = (char *) malloc (strlen(url_first_part) + strlen(subject_index_scraper.parsed_urls[i]) + 1);
        strcpy(course_page, url_first_part);
        strcat(course_page, subject_index_scraper.parsed_urls[i]);
        strcpy(subject_index_scraper.parsed_urls[i], course_page);
        (*num_urls)++;
      }
    }
  }
}
