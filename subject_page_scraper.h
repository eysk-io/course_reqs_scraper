#include <tidy.h>
#include <tidybuffio.h>

#ifndef __SUBJECT_PAGE_SCRAPER__
#define __SUBJECT_PAGE_SCRAPER__

#define PREREQUISITES_TAG "<em>Prerequisite:</em>"
#define COREQUISITES_TAG "<em>Corequisite:</em>"
#define EQUIVALENCIES_TAG "<em>Equivalencies:</em>"
#define EQUIVALENCY_TAG "<em>Equivalency:</em>"

typedef struct course {
    char* subject;
    int code;
    char* title;
    char* description;
    char* school;
    int credits;
    struct Requisites* pre_requisites;
    struct Requisites* co_requisites;
} course_t;

typedef struct subject_page_scraper {
  char * url;
  course_t* courses;
} subject_page_scraper_t;

size_t course_subject_buffer_callback(
    char* buffer, 
    size_t size, 
    size_t num_members, 
    TidyBuffer* tidy_buffer
);
void parse_node_for_descriptions(
    TidyDoc doc, 
    TidyBuffer* tidy_buffer, 
    TidyNode node, 
    course_t* courses
);

char* split(char* str, const char* delim);
char* trim_white_space(char* str);
void get_requisites(
    char** course_description, 
    char** prerequisites, 
    char** corequisites, 
    char** equivalency
);

void update_each_course(TidyBuffer* tidy_buffer, size_t* num_courses);
void update_courses(subject_page_scraper_t subject_page_scraper, size_t* num_courses);

#endif
