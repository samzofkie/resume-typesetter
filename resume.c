#include <pango/pangocairo.h>
#include "resume.h"

// TODO
// * bullet function
// * section meta function

/*
void write_long_line(Text *t)
{
  PangoLayout *layout;
  double max_width = doc.width - doc.margin * 2;
  double w, h;
  int i = 0, prev;
  char first_line[strlen(t->text)];
  memset(first_line, '\0', strlen(t->text));

  do {
    prev = i;

    for (; t->text[i] != ' '; i++) ;

    strncpy(first_line, t->text, i);
    i++;

    Text *potential_line = alloc_text(first_line, t->style_type);
    layout = create_layout_get_size(potential_line, &w, &h);
    dealloc_text(potential_line);
  } while (w < max_width);

  Text *first_line_text = alloc_text(first_line, t->style_type);
  write(first_line_text);

  char remainder[strlen(t->text)];
  strcpy(remainder, first_line);
  Text *remainder_text = alloc_text(remainder, t->style_type);
  write(remainder_text);
}
*/

const double doc_width = 8.5*72,
             doc_height = 11*72,
             margin = 25;
double cursor = margin;
cairo_surface_t *surface;
cairo_t *cr;


TextLayout new_layout(const char *str, const char *font_str, int font_size) {
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


void Center(TextLayout l, double *x, double *y) {
  *x = (doc_width - l.width) / 2;
}


void Right(TextLayout l, double *x, double *y) {
  *x = doc_width - margin - l.width;
}


void Indent(TextLayout l, double *x, double *y) {
  *x += margin;
}


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
  TextLayout tl = new_layout(str, font_str, font_size);
  if (num_fns) //for (int i=0; i<num_fns; i++)
    (*format_fns)(tl, &x, &y);
  draw_and_free_layout(x, y, tl.layout);
  if (line_increment > 0)
    cursor += tl.height + line_increment;
}


void Title(const char *str) {
  Render(str, "Cantarell Bold", 26, &Center, 1, 15);
}

void Subtitle(const char *str) {
  Render(str, "Cantarell", 12, &Center, 1, 15);
}

void SectionTitle(const char *str) {
  Render(str, "Cantarell Bold", 18, &SectionLine, 1, 2);
}

void SplitLine(const char *left, const char *right) {
  Render(left, "Cantarell", 12, NULL, 0, -1);
  Render(right, "Cantarell", 12, &Right, 1, 2);
}

void SubSectionTitle(const char *str) {
  Render(str, "Cantarell Bold", 12, NULL, 0, 2);
}


int index_of_next_space(const char *str, int end) {
  if (end < 0 || end > strlen(str)-1)
    return -1;
  while (--end != 0)
    if (str[end] == ' ')
      break;
  return end;
}


int longest_str_that_fits_in(char *str, double width) {
  //printf("%s\n", str); 
  int i = strlen(str);
  double new_width;
  while (new_width = new_layout(str, "Cantarell", 9).width > width) {
    i = index_of_next_space(str, i);
    str[i] = 0;
  }
  return i;
}

void Bullet(const char *str) {
  // strlen("Whats up") is 8 
  double max_width = doc_width - margin * 4;

  for (int start=0, end=0; start < strlen(str); start += end) {
    end = longest_str_that_fits_in((char*)str+start, max_width);
    //char *test = (char*)malloc(end-start+1);
    //strncpy(test, str + start, end);
    //Render(test, "Cantarell", 9, &Indent, 1, 2);
    //free(test);
  } 
}

void Center12Point(const char *str) {
  Render(str, "Cantarell", 12, &Center, 1, 2);
}
