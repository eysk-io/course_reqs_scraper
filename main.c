#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "scraper.c"

char** subject_page_urls;

int main( int argc, char ** argv ) {
  printf("Started...\n");
  Scraper scraper = {
    "http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name",
    (char **) malloc(MAX_LINKS * (sizeof(char *)))
  };

  int num_urls;
  get_course_page_urls(scraper, &num_urls);

  int num_course_codes = 0;
  subject_page_urls = malloc(num_urls * sizeof(char*));
  for (int i = 1; i < (num_urls * 2); i+=2) {
    subject_page_urls[num_course_codes] = scraper.parsed_urls[i];
    num_course_codes++;
  }

  for (int i = 0; i < num_urls; i++) {
    printf("course %d: %s\n", i + 1, subject_page_urls[i]);
  }

  free(subject_page_urls);
}
