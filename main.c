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
  printf("Started...\n");
  
  size_t num_threads;
  if (argc != 2) {
    num_threads = 1;
  } else {
    num_threads = strtol(argv[1], NULL, 10);
  }
  printf("Number of threads: %ld\n", num_threads);

  mongoc_init();
  
  index_page_scraper_t index_page_scraper = {
    "http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name",
    (subject_info_t*) malloc(MAX_SUBJECT_INFO_NUM * (sizeof(subject_info_t)))
  };

  size_t num_urls = 0;
  get_course_page_urls(index_page_scraper, &num_urls);

  size_t num_course_codes = 0;
  char** subject_page_urls = malloc(num_urls * sizeof(char*));
  for (size_t i = 1; i < (num_urls * 2); i+=2) {
    subject_page_urls[num_course_codes] = index_page_scraper.subject_info[i].subject_url;
    num_course_codes++;
  }

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

  update_school(subject_page_urls, num_urls);

  free(subject_page_urls);
  free(scrapers);
  mongoc_cleanup();
}
