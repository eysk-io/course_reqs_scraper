#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <mongoc/mongoc.h>
#include "scrapers/subject_index_scraper.c"
#include "scrapers/course_subject_scraper.c"

int main(int argc, char** argv) {
  mongoc_init ();
  
  printf("Started...\n");
  SubjectIndexScraper subject_index_scraper = {
    "http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name",
    (char **) malloc(MAX_LINKS * (sizeof(char *)))
  };

  int num_urls = 0;
  get_course_page_urls(subject_index_scraper, &num_urls);

  int num_course_codes = 0;
  char** subject_page_urls = malloc(num_urls * sizeof(char*));
  for (int i = 1; i < (num_urls * 2); i+=2) {
    subject_page_urls[num_course_codes] = subject_index_scraper.parsed_urls[i];
    num_course_codes++;
  }

  int num_courses = 0;
  for (int i = 0; i < num_urls; i++) {
    printf("code number %d: %s\n", i + 1, subject_page_urls[i]); 
    CourseSubjectScraper course_subject_scraper;
    course_subject_scraper.url = subject_page_urls[i];
    update_courses(course_subject_scraper, &num_courses);
  }

  // Getting courses by individual course codes:
  // There are 260 course codes
  // There are 8958 individual courses 
  // subject_page_urls[73] is CPSC

  // int num_courses = 0;
  // CourseSubjectScraper course_subject_scraper;
  // course_subject_scraper.url = subject_page_urls[63];
  // update_courses(course_subject_scraper, &num_courses, client, collection);
  
  printf("num_courses: %d\n", num_courses);

  free(subject_page_urls);
  mongoc_cleanup();
}
