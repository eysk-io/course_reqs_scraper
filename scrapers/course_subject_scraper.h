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
