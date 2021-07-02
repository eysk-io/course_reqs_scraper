// This is the webcrawler header file
// A web crawler will be able to:
// - write to the output
// - crawl down one layer
#include <tidy.h>
#include <tidybuffio.h>

#define BUFFER_SIZE 2048;

// define the Crawler struct that holds data about the web traversal
#define CrawlerTag
typedef struct CrawlerTag {
  char * url;
  char ** parsedUrls;
} Crawler;

// 1 = success
// 0 = failure
int writeContent(char ** content);

// get the website content with libcurl
int getContent(Crawler crawler);

// return an array of links
// take page content in HTML
void parseNode(TidyNode node, char ** output);
