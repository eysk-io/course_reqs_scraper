#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "scrapers/subject_index_scraper.c"
#include "scrapers/course_subject_scraper.c"

char** subject_page_urls;

int main( int argc, char ** argv ) {
  printf("Started...\n");
  SubjectIndexScraper subject_index_scraper = {
    "http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name",
    (char **) malloc(MAX_LINKS * (sizeof(char *)))
  };

  int num_urls;
  get_course_page_urls(subject_index_scraper, &num_urls);

  int num_course_codes = 0;
  subject_page_urls = malloc(num_urls * sizeof(char*));
  for (int i = 1; i < (num_urls * 2); i+=2) {
    subject_page_urls[num_course_codes] = subject_index_scraper.parsed_urls[i];
    num_course_codes++;
  }

  for (int i = 0; i < num_urls; i++) {
    printf("course %d: %s\n", i + 1, subject_page_urls[i]); 
  }

  // TODO: Change this to get courses from each url
  CourseSubjectScraper course_subject_scraper;
  course_subject_scraper.url = subject_page_urls[0];
  int num_courses = 0;
  get_courses(course_subject_scraper, &num_courses);

  free(subject_page_urls);
}
