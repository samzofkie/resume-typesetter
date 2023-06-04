#include <math.h>
#include <stdbool.h>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>

// TODO
// * linked list
// * refactor line wrap
// * section / bullet func
// * beautify all (const members of struct)

/*
enum Alignment {Left, Right, Center};

typedef struct {
  const char *font_string;
  int font_size;
  enum Alignment alignment;
  bool section_line;
  double cursor_increment;
  bool newline;
} TextStyle;

enum TextStyleType {Title, Subtitle, SectionTitle,
                    ProjectTitle, Normal12, NoNewline12,
                    Right12, Bullet};

typedef struct _Text {
  char *text; 
  enum TextStyleType style_type;
  struct _Text *next;
} Text;

typedef struct {
  double width, height, margin; 
  cairo_surface_t *surface;
  cairo_t *cr;
  double cursor;
  Text *head;
  TextStyle styles[];
} Document;

static Document doc = { 
    8.5*72, // width 
    11*72,  // height
    25,     // margin
    NULL,   // surface
    NULL,   // cr
    0,      // cursor
    NULL,   // head
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

void init_doc(void)
{
  doc.cursor = doc.margin;

  doc.surface = cairo_pdf_surface_create("resume.pdf", doc.width, doc.height);
  doc.cr = cairo_create(doc.surface);

  cairo_set_source_rgb(doc.cr, 1.0, 1.0, 1.0);
  cairo_paint(doc.cr);
  cairo_set_source_rgb(doc.cr, 0, 0, 0);
  cairo_set_line_width(doc.cr, 0.5);
}


TextStyle *get_style(Text *t)
{
  return &doc.styles[t->style_type];
}


PangoLayout *create_layout_get_size(Text *t, double *w, double *h)
{
  PangoLayout *layout = pango_cairo_create_layout(doc.cr);
  PangoFontDescription *font_desc = pango_font_description_from_string(get_style(t)->font_string);
  
  pango_layout_set_text(layout, t->text, -1); 
  pango_font_description_set_size(font_desc, get_style(t)->font_size * PANGO_SCALE);
  pango_layout_set_font_description(layout, font_desc);
  pango_font_description_free(font_desc);

  int _w, _h;
  pango_layout_get_size(layout, &_w, &_h);
  *w = (double)_w / PANGO_SCALE;
  *h = (double)_h / PANGO_SCALE;

  return layout;
}


Text *alloc_text(const char *str, enum TextStyleType type)
{
  Text *new = (Text*)malloc(sizeof(Text));
  new->text = (char*)malloc(strlen(str)+1);
  strcpy(new->text, str);
  new->style_type = type;
  new->next = NULL;
  return new;
}


void *dealloc_text(Text *t)
{
  free(t->text);
  free(t);
}


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


void write(Text *t) {
  PangoLayout *layout;
  double layout_width, layout_height;
  layout = create_layout_get_size(t, &layout_width, &layout_height);
  double x = doc.margin, 
         y = doc.cursor;
  TextStyle style = *get_style(t);

  if (layout_width > doc.width - doc.margin * 2)
    return write_long_line(t);
  
  if (style.alignment == Center)
    x = (doc.width - layout_width) / 2;
  
  else if (style.alignment == Right)
    x = doc.width - doc.margin - layout_width;
   
  if (style.section_line) {
    double line_y = y + layout_height/2 + 2;
    cairo_move_to(doc.cr, x + layout_width, line_y);
    cairo_line_to(doc.cr, doc.width - doc.margin, line_y);
    cairo_stroke(doc.cr); 
  }
  
  cairo_move_to(doc.cr, x, y);
  pango_cairo_show_layout(doc.cr, layout);
  g_object_unref(layout);

  if (style.newline)
    doc.cursor += layout_height + style.cursor_increment;
}


void add(const char *str, enum TextStyleType type)
{
  Text *new = alloc_text(str, type);

  if (!doc.head) {
    doc.head = new;
    return;
  }

  Text *last_node = doc.head;
  while (last_node->next)
    last_node = last_node->next;
  last_node->next = new;
};


void cleanup_doc(void)
{
  Text *curr = doc.head, *prev = doc.head;
  while (curr) {
    prev = curr;
    curr = curr->next;
    dealloc_text(prev);
  }
  cairo_destroy(doc.cr);
  cairo_surface_destroy(doc.surface); 
}
*/


static const double doc_width = 8.5*72,
                    doc_height = 11*72,
                    margin = 25,
                    cursor = margin;
static cairo_surface_t *surface;
static cairo_t *cr;


void Title(const char *str) {
  PangoLayout *layout = pango_cairo_create_layout(cr);
  PangoFontDescription *font_desc = pango_font_description_from_string("Cantarell Bold");
  
  pango_layout_set_text(layout, str, -1); 
  pango_font_description_set_size(font_desc, 26 * PANGO_SCALE);
  pango_layout_set_font_description(layout, font_desc);
  pango_font_description_free(font_desc);

  int _w, _h;
  pango_layout_get_size(layout, &_w, &_h);
  double w = (double)_w / PANGO_SCALE;
  double h = (double)_h / PANGO_SCALE;

  double x = margin, 
         y = cursor;

  /*if (layout_width > doc_width - margin * 2)
    return write_long_line(t);
  
  if (style.alignment == Center)
    x = (doc.width - layout_width) / 2;
  
  else if (style.alignment == Right)
    x = doc.width - doc.margin - layout_width;
   
  if (style.section_line) {
    double line_y = y + layout_height/2 + 2;
    cairo_move_to(doc.cr, x + layout_width, line_y);
    cairo_line_to(doc.cr, doc.width - doc.margin, line_y);
    cairo_stroke(doc.cr); 
  }*/
  
  cairo_move_to(cr, x, y);
  pango_cairo_show_layout(cr, layout);
  g_object_unref(layout);

  /*if (style.newline)
    doc.cursor += layout_height + style.cursor_increment;*/

};

void Subtitle(const char *str);
void SectionTitle(const char *str);
void SplitLine(const char *left, const char *right);
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
  /*
  add("Sam Zofkie", Title);
  add("samzofkie@gmail.com  •  github.com/samzofkie  •  samzofkie.com", Subtitle);
  add("Education", SectionTitle);
  add("Reed College, B.A. Computer Science", NoNewline12);
  add("2016-2020", Right12);
  add("Projects", SectionTitle);
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
