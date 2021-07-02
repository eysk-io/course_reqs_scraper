#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "crawler.c"

int main( int argc, char ** argv ) {
  printf("Started...\n");
  Crawler crawler = {
    "",
    "http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name",
    (char **) malloc(MAX_LINKS * (sizeof(char *)))
  };
  getCoursePages(crawler);
  writeContent(crawler.parsedUrls);
}
