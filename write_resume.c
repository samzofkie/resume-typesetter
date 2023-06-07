#include <math.h>
#include <stdbool.h>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>

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

static const double doc_width = 8.5*72,
                    doc_height = 11*72,
                    margin = 25;
static double cursor = margin;
static cairo_surface_t *surface;
static cairo_t *cr;


typedef struct {
  PangoLayout *layout;
  double width, height;
} TextLayout;


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

void Bullet(const char *str) {

}

void Center12Point(const char *str) {
  Render(str, "Cantarell", 12, &Center, 1, 2);
}

int main (int argc, char **argv) {
  surface = cairo_pdf_surface_create("resume.pdf", doc_width, doc_height);
  cr = cairo_create(surface);
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint(cr);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width(cr, 0.5);

  Title("Sam Zofkie");
  Subtitle("samzofkie@gmail.com  •  github.com/samzofkie  •  samzofkie.com");
  SectionTitle("Education");
  SplitLine("Reed College, B.A. Computer Science", "2016-2020");
  SectionTitle("Projects");
  SubSectionTitle("Automated Linux From Scratch");
  SubSectionTitle("Youtube Game");
  SubSectionTitle("X11 PulseAudio DAW");
  SectionTitle("Skills");
  Center12Point("Proficient: C, C++, JavaScript, React, Git, Unix");
  Center12Point("Familiar: Python, Bash, Docker, SQL, AWS, HTML, CSS");
  
  /*
  add("• Created web app to display randomly chosen YouTube videos using Flask and React.", Bullet);
  add("• Designed gibberish search word algorithm and implemented crawler in Python to populate SQLite database with video URLs using Python.", Bullet);
  add("• Researched and implemented JavaScript CSS technique to hide HTML iframes to improve UI responsiveness.", Bullet);
  */

  cairo_destroy(cr);
  cairo_surface_destroy(surface); 

  return 0;
}
