#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <curl/curl.h>
#include <tidy.h>
#include <tidybuffio.h>
#include "course_subject_scraper.h"

#define MAX_LINKS 1000
#define MAX_URL_LEN 512
#define MAX_REQS 10

char* PREREQUISITES_TAG = "<em>Prerequisite:</em>";
char* COREQUISITES_TAG = "<em>Corequisite:</em>";
char* EQUIVALENCIES_TAG = "<em>Equivalencies:</em>";

size_t course_subject_buffer_callback(
  char * buffer,
  size_t size,
  size_t num_members,
  TidyBuffer * tidy_buffer) {
    size_t new_size = size * num_members;
    tidyBufAppend(tidy_buffer, buffer, new_size);
    return new_size;
};

void parse_node_for_descriptions(TidyDoc doc, TidyBuffer* tidy_buffer, TidyNode node, struct Course* courses) {
    TidyNode child;
    for (child = tidyGetChild(node); child != NULL; child = tidyGetNext(child)) {
        if (
            tidyNodeIsDD(child) ||
            tidyNodeIsDT(child)
        ) {
            tidyNodeGetText(doc, child, tidy_buffer);
        }
        parse_node_for_descriptions(doc, tidy_buffer, child, courses);
    }
}

char *split(char *str, const char *delim) {
    char *p = strstr(str, delim);
    if (p == NULL) return NULL;
    *p = '\0';
    return p + strlen(delim);
}

char *trim_white_space(char *str) {
    char *end;
    while (isspace((unsigned char) *str)) {
        str++;
    }
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) {
        end--;
    }
    end[1] = '\0';
    return str;
}

void get_requisites(char** course_description, char** prerequisites, char** corequisites, char** equivalencies) {
    if (strstr(*course_description, PREREQUISITES_TAG)) {
        *prerequisites = split(*course_description, PREREQUISITES_TAG);
        *prerequisites = trim_white_space(*prerequisites);
        if (strstr(*prerequisites, COREQUISITES_TAG)) {
            *corequisites = split(*prerequisites, COREQUISITES_TAG);
            *corequisites = trim_white_space(*corequisites);
            if (strstr(*corequisites, EQUIVALENCIES_TAG)) {
                *equivalencies = split(*corequisites, EQUIVALENCIES_TAG);
                *equivalencies = trim_white_space(*equivalencies);
            }
        } else if (strstr(*prerequisites, EQUIVALENCIES_TAG)) {
            *equivalencies = split(*prerequisites, EQUIVALENCIES_TAG);
            *equivalencies = trim_white_space(*equivalencies);
        }
    } else if (strstr(*course_description, COREQUISITES_TAG)) {
        *corequisites = split(*course_description, COREQUISITES_TAG);
        *corequisites = trim_white_space(*corequisites);
        if (strstr(*corequisites, EQUIVALENCIES_TAG)) {
            *equivalencies = split(*corequisites, EQUIVALENCIES_TAG);
            *equivalencies = trim_white_space(*equivalencies);
        }
    } else if (strstr(*course_description, EQUIVALENCIES_TAG)) {
        *equivalencies = split(*course_description, EQUIVALENCIES_TAG);
        *equivalencies = trim_white_space(*equivalencies);
    }

    *course_description = trim_white_space(*course_description);
    if (strstr(*course_description, "<br />")) {
        split(*course_description, "<br />");
    }

    if (*prerequisites) {
        *prerequisites = trim_white_space(*prerequisites);
    }
    if (*corequisites) {
        *corequisites = trim_white_space(*corequisites);
    }
}

void get_each_course(TidyBuffer* tidy_buffer, int* num_courses) {
    char* tail = (char*) tidy_buffer->bp;
    while(strstr(tail, "</dd>")) {
        *num_courses = *num_courses + 1;
        char* all_courses = tail;
        char* course_subject = split(all_courses, "</a>");

        char* course_code_str = split(course_subject, " ");
        int course_code = atoi(course_code_str);

        char* course_num_credits_str = split(course_code_str, " (");

        char* course_title = split(course_num_credits_str, ")");
        course_title = split(course_title, "<b>");

        char* course_description = split(course_title, "</b></dt>");

        course_description = trim_white_space(course_description);
        course_description = split(course_description, "<dd>");

        tail = split(course_description, "</dd>");
        tail = trim_white_space(tail);

        char* prerequisites = '\0';
        char* corequisites = '\0';
        char* equivalencies = '\0';
        get_requisites(&course_description, &prerequisites, &corequisites, &equivalencies);

        printf("course_subject: %s\n", course_subject);
        printf("course_code: %d\n", course_code);
        printf("course_num_credits: %s\n", course_num_credits_str);
        printf("course_title: %s\n", course_title);
        printf("course_description: %s\n", course_description);
        if (prerequisites) {
            printf("prerequisites: %s\n", prerequisites);
        }
        if (corequisites) {
            printf("corequisites: %s\n", corequisites);
        }
        if (equivalencies) {
            printf("equivalencies: %s\n", equivalencies);
        }
        printf("\n");
    }
}

void get_courses(CourseSubjectScraper course_subject_scraper, int* num_courses) {
    CURL *handle;
    handle = curl_easy_init();
    char err_buff[CURL_ERROR_SIZE];
    int res;
    TidyDoc parse_doc;
    TidyBuffer tidy_buffer = {0};
    TidyBuffer description_tidy_buffer = {0};

    if (handle) {
        curl_easy_setopt(handle, CURLOPT_URL, course_subject_scraper.url); // set URL
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, course_subject_buffer_callback); // set output callback function
        curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, err_buff);

        parse_doc = tidyCreate();
        tidyBufInit(&tidy_buffer);
        tidyOptSetInt(parse_doc, TidyWrapLen, 2048); // set max length
        tidyOptSetBool(parse_doc, TidyForceOutput, yes); // force output

        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &tidy_buffer); // identify buffer to store data in

        res = curl_easy_perform(handle); // execute request, return status code to res

        if (res == CURLE_OK) {
            printf("Parsed all courses from: %s\n", course_subject_scraper.url);

            tidyParseBuffer(parse_doc, &tidy_buffer);
            course_subject_scraper.courses = malloc(sizeof(struct Courses*));

            tidyBufInit(&description_tidy_buffer);
            parse_node_for_descriptions(parse_doc, &description_tidy_buffer, tidyGetBody(parse_doc), course_subject_scraper.courses);

            get_each_course(&description_tidy_buffer, num_courses);
        } else {
            printf("Failed to parse courses from: %s\n", course_subject_scraper.url);
            return;
        }
        curl_easy_cleanup(handle);
        tidyBufFree(&tidy_buffer);
        tidyBufFree(&description_tidy_buffer);
        tidyRelease(parse_doc);
    }
}