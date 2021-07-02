#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "crawler.c"

int main( int argc, char ** argv ) {
  printf("Started...\n");
  Crawler crawler = {
    "",
    argv[1],
    (char **) malloc(MAX_LINKS * (sizeof(char *)))
  };
  getContent(crawler);
  writeContent(crawler.parsedUrls);
}
