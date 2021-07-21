#include <tidy.h>
#include <tidybuffio.h>

#define BUFFER_SIZE 2048;

struct Requisites {
    int score_of;
    void *courses;
    char *one_of;
    char *advanced_credit;
    char *metric;
};

struct Course {
    char* subject;
    int code;
    char* title;
    char* description;
    char* school;
    int credits;
    struct Requisites* pre_requisites;
    struct Requisites* co_requisites;
};

#define ScraperTag
typedef struct ScraperTag {
  char * url;
  struct Course* courses;
} CourseSubjectScraper;

size_t course_subject_buffer_callback(char * buffer, size_t size, size_t num_members, TidyBuffer * tidy_buffer);

void parse_node_for_descriptions(TidyDoc doc, TidyBuffer* tidy_buffer, TidyNode node, struct Course* courses);

char *split(char *str, const char *delim);

char *trim_white_space(char *str);

void get_requisites(char** course_description, char** prerequisites, char** corequisites, char** equivalency);

void get_each_course(TidyBuffer* tidy_buffer, int* num_courses);

void get_courses(CourseSubjectScraper course_subject_scraper, int* num_courses);
