#ifndef RESUME_H
#define RESUME_H

#include <pango/pangocairo.h>

extern const double doc_width, doc_height, margin;
extern double cursor;
extern cairo_surface_t *surface;
extern cairo_t *cr;

typedef struct {
  PangoLayout *layout;
  double width, height;
} TextLayout;

TextLayout new_layout(const char *, const char *, int);

void draw_and_free_layout(double, double, PangoLayout *);

void Center(TextLayout, double *, double *);
void Right(TextLayout, double *, double *);
void Indent(TextLayout, double *, double *);
void SectionLine(TextLayout, double *, double *);

void Render(const char *, 
           const char *, 
           int, 
           void(*)(TextLayout, double *, double *),  
           int,
           double line_increment);

void Title(const char *);
void Subtitle(const char *);
void SectionTitle(const char *);
void SplitLine(const char *, const char *);
void SubSectionTitle(const char *);

int index_of_next_space(const char *, int);
int longest_str_that_fits_in(char *, double);
void Bullet(const char *);

void Center12Point(const char *);

#endif
