#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <tidy.h>
#include <tidybuffio.h>
#include "crawler.h"
#include "io.c"

#define MAX_LINKS 1000
#define MAX_URL_LEN 512
#define COURSE_CODE_LINK "courses.cfm?page=name&code="
int currentIndex = 0;

size_t bufferCallback(
  char * buffer,
  size_t size,
  size_t nmemb,
  TidyBuffer * tidyBuffer) {
    size_t newSize = size * nmemb;
    tidyBufAppend(tidyBuffer, buffer, newSize);
    return newSize;
};

int writeContent(char ** output) {
  char* url_first_part = "http://www.calendar.ubc.ca/vancouver/";
  for (int i = 1; i < currentIndex; i++) {
    if (
      output[i] &&
      !strcmp(output[i - 1], output[i]) 
    ) {
      if ((strlen(url_first_part) + strlen(output[i]) - 1) > MAX_URL_LEN) {
        printf("max url length exceeded for %s%s", url_first_part, output[i]);
        return 0;
      }
      char *newOutput = malloc (strlen(url_first_part) + strlen(output[i]) + 1);
      strcpy(newOutput, url_first_part);
      strcat(newOutput, output[i]);
      printf("writing %d: %s\n", i, newOutput);
      openFileAndWrite(OUTPUT_PATH, newOutput);
      free(newOutput);
    }
  }
  return 1;
}

void parse(TidyNode node, char ** output) {
  TidyNode child;
  for (child = tidyGetChild(node); child != NULL; child = tidyGetNext(child)) {
    TidyAttr hrefAttr = tidyAttrGetById(child, TidyAttr_HREF);
    if (hrefAttr) {
      if (currentIndex < MAX_LINKS) {
        if (
          strlen(tidyAttrValue(hrefAttr)) < MAX_URL_LEN &&
          strstr(tidyAttrValue(hrefAttr), COURSE_CODE_LINK)
        ) {
          strcpy(output[currentIndex], tidyAttrValue(hrefAttr));
          currentIndex++;
        }
      }
      if (tidyAttrValue(hrefAttr)) printf("Url found: %s\n", tidyAttrValue(hrefAttr));
    }

    parse(child, output);
  }
}

int getContent(Crawler crawler) {
  if (crawler.url) {
    CURL *handle;
    handle = curl_easy_init();
    char errBuff[CURL_ERROR_SIZE];
    int res;
    TidyDoc parseDoc;
    TidyBuffer tidyBuffer = {0};

    if (handle) {
      curl_easy_setopt(handle, CURLOPT_URL, crawler.url); // set URL
      curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, bufferCallback); // set output callback function
      curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuff);

      parseDoc = tidyCreate();
      tidyBufInit(&tidyBuffer);
      tidyOptSetInt(parseDoc, TidyWrapLen, 2048); // set max length
      tidyOptSetBool(parseDoc, TidyForceOutput, yes); // force output

      curl_easy_setopt(handle, CURLOPT_WRITEDATA, &tidyBuffer); // identify buffer to store data in

      res = curl_easy_perform(handle); // execute request, return status code to res

      if (res == CURLE_OK) {
        printf("successful crawl of %s\n", crawler.url);

        tidyParseBuffer(parseDoc, &tidyBuffer);

        for (int i = 0; i < MAX_LINKS; i ++) {
          crawler.parsedUrls[i] = (char *) malloc(MAX_URL_LEN * sizeof(char *));
        }
        parse(tidyGetBody(parseDoc), crawler.parsedUrls); // parse results
        crawler.parsedUrls = crawler.parsedUrls;
      } else {
        printf("crawl failed for %s\n", crawler.url);
        return 0; // failure
      }

      curl_easy_cleanup(handle);
      tidyBufFree(&tidyBuffer);
      tidyRelease(parseDoc);

      return 1; // success

    }
    return 0; // failure

  }
  return 0; // failure

}
