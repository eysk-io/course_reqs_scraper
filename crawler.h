#include <tidy.h>
#include <tidybuffio.h>

#define BUFFER_SIZE 2048;

#define CrawlerTag
typedef struct CrawlerTag {
  char * url;
  char ** parsed_urls;
} Crawler;

int parse_node(TidyNode node, char ** output);

int get_all_urls_on_page(Crawler crawler);

void get_course_page_urls(Crawler crawler, int *num_urls);
