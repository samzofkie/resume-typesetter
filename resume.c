#include <pango/pangocairo.h>
#include <cairo-pdf.h>
#include "resume.h"

// TODO
// * section meta function
// * component function arr of functions
// * links

const double doc_width = 8.5*72,
             doc_height = 11*72,
             margin = 25;
double cursor = margin;
cairo_surface_t *surface;
cairo_t *cr;


void init_cairo() {
  surface = cairo_pdf_surface_create("resume.pdf", doc_width, doc_height);
  cr = cairo_create(surface);
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint(cr);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width(cr, 0.5);
}


void cleanup_cairo() {
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}


typedef struct {
  PangoLayout *layout;
  double width, height;
} TextLayout;


TextLayout new_textlayout(const char *str, const char *font_str, int font_size) {
  PangoLayout *layout = pango_cairo_create_layout(cr);
  PangoFontDescription *font_desc = pango_font_description_from_string(font_str);
  
  pango_layout_set_text(layout, str, -1); 
  pango_font_description_set_size(font_desc, font_size * PANGO_SCALE);
  pango_layout_set_font_description(layout, font_desc);
  pango_font_description_free(font_desc);

  int w, h;
  pango_layout_get_size(layout, &w, &h);
   
  return (TextLayout){layout, (double)w / PANGO_SCALE, (double)h / PANGO_SCALE};
}


void draw_and_free_layout(double x, double y, PangoLayout *l) {
  cairo_move_to(cr, x, y);
  pango_cairo_show_layout(cr, l);
  g_object_unref(l);
}


void Center(TextLayout l, double *x, double *y) { *x = (doc_width - l.width) / 2; }
void Right(TextLayout l, double *x, double *y) { *x = doc_width - margin - l.width; }
void Indent(TextLayout l, double *x, double *y) { *x += margin; }

void SectionLine(TextLayout l, double *x, double *y) {
  double line_y = *y + l.height/2 + 2;
  cairo_move_to(cr, *x + l.width, line_y);
  cairo_line_to(cr, doc_width - margin, line_y);
  cairo_stroke(cr);
}


void Render(const char *str, 
           const char *font_str, 
           int font_size, 
           void (*format_fns)(TextLayout, double*, double*),  
           int num_fns,
           double line_increment) {
  double x = margin, y = cursor;
  TextLayout tl = new_textlayout(str, font_str, font_size);
  if (num_fns) //for (int i=0; i<num_fns; i++)
    (*format_fns)(tl, &x, &y);
  draw_and_free_layout(x, y, tl.layout);
  if (line_increment > 0)
    cursor += tl.height + line_increment;
}


void Title(const char *str) { Render(str, "Cantarell Bold", 26, &Center, 1, 15); }
void Subtitle(const char *str) { Render(str, "Cantarell", 12, &Center, 1, 15); }
void SectionTitle(const char *str) { Render(str, "Cantarell Bold", 18, &SectionLine, 1, 2); }
void SplitLine(const char *left, const char *right) {
  Render(left, "Cantarell", 12, NULL, 0, -1);
  Render(right, "Cantarell", 12, &Right, 1, 2);
}
void SubSectionTitle(const char *str) { Render(str, "Cantarell Bold", 12, NULL, 0, 2); }


int index_of_first_space(const char *str) {
  int i = 0;
  while (str[i] != ' ' && str[i] != 0) i++;
  return i;  
}


int str_fits_in(const char *str, int str_len,
                const char *font_str, int font_size, double width) 
{
  char *slice = strdup(str);
  slice[str_len] = '\0';  
  TextLayout test = new_textlayout(slice, font_str, font_size); 
  free(slice);
  g_object_unref(test.layout);
  return test.width < width;
}


int length_longest_str_that_fits(const char *str, double max_width) {
  int i = 0, prev; 
  while (i < strlen(str)) {
    i += index_of_first_space(str + i);
    if(!str_fits_in(str, i, "Cantarell", 9, max_width))
      return prev;
    prev = i;
    if (str[i] == ' ')
      i++;
  }
  return i;
}


void Bullet(const char *sentence) {
  TextLayout tl = new_textlayout("•", "Cantarell Bold", 9);
  draw_and_free_layout(margin + margin - tl.width - 5, cursor, tl.layout);
  
  int i = 0, j = 0;
  double max_width = doc_width - margin * 4;
  char *slice;
  while (i < strlen(sentence)) {
    j += length_longest_str_that_fits(sentence + i, max_width);
    slice = strdup(sentence);
    slice[j] = 0;
    if (slice[i] == ' ')
      i++;
    Render(slice + i, "Cantarell", 9, &Indent, 1, 2);
    i = j;
  }
  free(slice);
}


void Center12Point(const char *str) { Render(str, "Cantarell", 12, &Center, 1, 2); }


void Project(int nlines, ...) {
  va_list valist;
  va_start(valist, nlines);
  SubSectionTitle(va_arg(valist, const char *));
  for (int i=1; i<nlines; i++)
    Bullet(va_arg(valist, const char *));
  va_end(valist);
}

