#include <tidy.h>
#include <tidybuffio.h>

#ifndef __INDEX_PAGE_SCRAPER__
#define __INDEX_PAGE_SCRAPER__

#define BUFFER_SIZE 2048;
#define MAX_LINKS 1000
#define MAX_URL_LEN 512
#define COURSE_CODE_LINK "courses.cfm?page=name&code="

typedef struct index_page_scraper {
  char* url;
  char** parsed_urls;
} index_page_scraper_t;

size_t subject_index_buffer_callback(
  char* buffer,
  size_t size,
  size_t num_members,
  TidyBuffer * tidy_buffer
);
void parse_node_for_href(TidyNode node, char** output);
int get_all_urls_on_page(index_page_scraper_t index_page_scraper);
void get_course_page_urls(index_page_scraper_t index_page_scraper, size_t* num_urls);

#endif
