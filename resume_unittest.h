#ifndef RESUME_UNITTEST_H
#define RESUME_UNITTEST_H

extern const double doc_width, doc_height;

void init_cairo();
void cleanup_cairo();

int index_of_first_space(const char*);
int length_longest_str_that_fits(const char *, double);

#endif
