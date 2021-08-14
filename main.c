#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <curl/curl.h>
#include <mongoc/mongoc.h>
#include "index_page_scraper.h"
#include "subject_page_scraper.h"
#include "tpool.h"

int main(int argc, char** argv) {
  mongoc_init();
  
  printf("Started...\n");
  index_page_scraper_t index_page_scraper = {
    "http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name",
    (char**) malloc(MAX_LINKS * (sizeof(char*)))
  };

  size_t num_urls = 0;
  get_course_page_urls(index_page_scraper, &num_urls);

  size_t num_course_codes = 0;
  char** subject_page_urls = malloc(num_urls * sizeof(char*));
  for (size_t i = 1; i < (num_urls * 2); i+=2) {
    subject_page_urls[num_course_codes] = index_page_scraper.parsed_urls[i];
    num_course_codes++;
  }

  size_t num_courses = 0;
  for (size_t i = 0; i < num_urls; i++) {
    printf("code number %ld: %s\n", i + 1, subject_page_urls[i]); 
    subject_page_scraper_t subject_page_scraper;
    subject_page_scraper.url = subject_page_urls[i];
    update_courses(subject_page_scraper, &num_courses);
  }

  // Getting courses by individual course codes:
  // There are 260 course codes
  // There are 8958 individual courses 
  // subject_page_urls[73] is CPSC

  // size_t num_courses = 0;
  // subject_page_scraper_t subject_page_scraper;
  // subject_page_scraper.url = subject_page_urls[63];
  // update_courses(subject_page_scraper, &num_courses, client, collection);
  
  printf("num_courses: %ld\n", num_courses);

  free(subject_page_urls);
  mongoc_cleanup();
}
