#include <tidy.h>
#include <tidybuffio.h>

#define BUFFER_SIZE 2048;

#define ScraperTag
typedef struct ScraperTag {
  char * url;
  char ** parsed_urls;
} Scraper;

void parse_node(TidyNode node, char ** output);

int get_all_urls_on_page(Scraper scraper);

void get_course_page_urls(Scraper scraper, int *num_urls);
