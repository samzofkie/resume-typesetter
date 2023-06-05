#include <math.h>
#include <stdbool.h>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>

/*
typedef struct {
  const char *font_string;
  int font_size;
  enum Alignment alignment;
  bool section_line;
  double cursor_increment;
  bool newline;
} TextStyle;

    {
      {"Cantarell Bold", 26, Center, false, 15, true}, // Title
      {"Cantarell", 12, Center, false, 15, true},      // Subtitle
      {"Cantarell Bold", 18, Left, true, 2, true},     // SectionTitle
      {"Cantarell", 14, Left, false, 2, true},         // ProjectTitle
      {"Cantarell", 12, Left, false, 2, true},         // Normal12
      {"Cantarell", 12, Left, false, 2, false},        // NoNewline12
      {"Cantarell", 12, Right, false, 2, true},        // Right12
      {"Cantarell", 10, Left, false, 1, true}          // Bullet
    }
};


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


void Center(double *x, TextLayout l) {
  *x = (doc_width - l.width) / 2;
}

void Right(double *x, TextLayout l) {
  *x = doc_width - margin - l.width;
}

void SectionLine(double x, double y, TextLayout l) {
  double line_y = y + l.height/2 + 2;
  cairo_move_to(cr, x + l.width, line_y);
  cairo_line_to(cr, doc_width - margin, line_y);
  cairo_stroke(cr);
}


void Title(const char *str) {
  double x = margin, y = cursor;
  TextLayout text_layout = new_layout(str, "Cantarell Bold", 26);

  Center(&x, text_layout);
  
  draw_and_free_layout(x, y, text_layout.layout);
  
  cursor += text_layout.height + 15;
};

void Subtitle(const char *str) {
  double x = margin, y = cursor;
  TextLayout text_layout = new_layout(str, "Cantarell", 12);

  Center(&x, text_layout);

  draw_and_free_layout(x, y, text_layout.layout);

  cursor += text_layout.height + 15;
}

void SectionTitle(const char *str) {
  double x = margin, y = cursor;
  TextLayout text_layout = new_layout(str, "Cantarell Bold", 18);

  SectionLine(x, y, text_layout);

  draw_and_free_layout(x, y, text_layout.layout);

  cursor += text_layout.height + 2;
}

void SplitLine(const char *left, const char *right) {
  double x = margin, y = cursor;
  TextLayout text_layout = new_layout(left, "Cantarell", 12);

  draw_and_free_layout(x, y, text_layout.layout);

  x = margin, y = cursor;
  text_layout = new_layout(right, "Cantarell", 12);

  Right(&x, text_layout);

  draw_and_free_layout(x, y, text_layout.layout);
  
  cursor += text_layout.height + 2;
}

void SubSectionTitle(const char *str);
void Bullet(const char *str);


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
  /*
  add("Youtube Game", ProjectTitle);
  add("• Created web app to display randomly chosen YouTube videos using Flask and React.", Bullet);
  add("• Designed gibberish search word algorithm and implemented crawler in Python to populate SQLite database with video URLs using Python.", Bullet);
  add("• Researched and implemented JavaScript CSS technique to hide HTML iframes to improve UI responsiveness.", Bullet);
  add("X11 PulseAudio DAW", ProjectTitle);
  add("Automated Linux From Scratch", ProjectTitle);
  add("Skills", SectionTitle);
  add("Proficient: C, C++, JavaScript, React, Git, Unix", Normal12);
  add("Familiar: Python, Bash, Docker, SQL, AWS, HTML, CSS", Normal12);
  */

  cairo_destroy(cr);
  cairo_surface_destroy(surface); 

  return 0;
}
