#include <tidy.h>
#include <tidybuffio.h>

#define BUFFER_SIZE 2048;

#define ScraperTag
typedef struct ScraperTag {
  char * url;
  char ** parsed_urls;
} SubjectIndexScraper;

void parse_node_for_href(TidyNode node, char ** output);

int get_all_urls_on_page(SubjectIndexScraper subject_index_scraper);

void get_course_page_urls(SubjectIndexScraper subject_index_scraper, int *num_urls);
