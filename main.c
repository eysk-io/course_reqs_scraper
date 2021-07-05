#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "crawler.c"

int main( int argc, char ** argv ) {
  printf("Started...\n");
  Crawler crawler = {
    "http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name",
    (char **) malloc(MAX_LINKS * (sizeof(char *)))
  };

  int num_urls;
  get_course_page_urls(crawler, &num_urls);

  int num_course_codes = 0;
  for (int i = 1; i < (num_urls * 2); i+=2) {
    num_course_codes++;
    printf("course %d: %s\n", num_course_codes, crawler.parsedUrls[i]);
  }
}
