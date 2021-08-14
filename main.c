#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <curl/curl.h>
#include <mongoc/mongoc.h>
#include "index_page_scraper.h"
#include "subject_page_scraper.h"
#include "tpool.h"

void worker(void* arg) {
  subject_page_scraper_t* subject_page_scraper = (subject_page_scraper_t*) arg;
  update_courses(subject_page_scraper);
}

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

  const size_t num_threads = 4;
  tpool_t* tm = tpool_create(num_threads);

  subject_page_scraper_t* scrapers = malloc(sizeof(subject_page_scraper_t) * num_urls);
  for (size_t i = 0; i < num_urls; i++) {
    printf("Code Number %ld: %s\n", i + 1, subject_page_urls[i]); 
    subject_page_scraper_t* subject_page_scraper = scrapers + i;
    subject_page_scraper->url = subject_page_urls[i];
    subject_page_scraper->num_courses = 0;
    tpool_add_work(tm, worker, subject_page_scraper);
  }
  
  tpool_wait(tm);
  tpool_destroy(tm);
  
  size_t num_courses = 0;
  for (size_t i = 0; i < num_urls; i++) {
    subject_page_scraper_t* subject_page_scraper = scrapers + i;
    printf("Number of Courses for %s: %ld\n", subject_page_scraper->url, subject_page_scraper->num_courses);
    num_courses += subject_page_scraper->num_courses;
  }
  printf("Total Number of Courses: %ld\n", num_courses);

  free(subject_page_urls);
  free(scrapers);
  mongoc_cleanup();
}
