#include <math.h>
#include <stdbool.h>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>

// TODO
// * linked list
// * refactor line wrap
// * section / bullet func
// * beautify all (const members of struct)


enum Alignment {Left, Right, Center};

enum TextStyleType {Title, Subtitle, SectionTitle,
                    ProjectTitle, Normal12, Bullet};

typedef struct {
  const char *font_string;
  int font_size;
  enum Alignment alignment;
  bool section_line;
  double cursor_increment;
  bool newline;
} TextStyle;

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
    NULL,
    {
      {"Cantarell Bold", 26, Center, false, 15, true}, // Title
      {"Cantarell", 12, Center, false, 15, true},      // Subtitle
      {"Cantarell Bold", 18, Left, true, 2, true},     // SectionTitle
      {"Cantarell", 14, Left, false, 2, true},         // ProjectTitle
      {"Cantarell", 12, Right, false, 2, true},        // Normal12
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


void write_long_line(Text *t)
{
/*
  if (w > max_width) {
    int i = 0, prev = 0;
    char short_str[strlen(t.text)];
    memset(short_str, '\0', sizeof(short_str));
    do {
      prev = i;
      
      for (; t.text[i] != ' '; i++) {}
      strncpy(short_str, t.text, i);
      i++;  
      
      Text short_text = { short_str, t.style };
      layout = create_layout_get_size(doc, short_text, &w, &h);
    
    } while (w < max_width);
    
    // Draw first line
    memset(short_str, '\0', sizeof(short_str));
    strncpy(short_str, t.text, prev);
    Text first_line = {short_str, t.style};
    draw_text(doc, first_line);

    // Draw rest
    char rest[strlen(t.text)];
    memset(rest, '\0', sizeof(rest));
    strncpy(rest, t.text + prev, strlen(t.text) - prev);
    Text rest_text = {rest, t.style};
    draw_text(doc, rest_text);

    return;
  }
    */
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
    cairo_line_to(doc.cr, doc.width - x, line_y);
    cairo_stroke(doc.cr); 
  }
  
  cairo_move_to(doc.cr, x, y);
  pango_cairo_show_layout(doc.cr, layout);
  g_object_unref(layout);

  if (style.newline)
    doc.cursor += layout_height + style.cursor_increment;
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
    free(prev);
  }
  cairo_destroy(doc.cr);
  cairo_surface_destroy(doc.surface); 
}


int main (int argc, char **argv)
{
  init_doc();

  add("Sam Zofkie", Title);
  add("samzofkie@gmail.com  •  github.com/samzofkie  •  samzofkie.com  •  17735510259  •  Chicago, IL", Subtitle);

  for (Text *p=doc.head; p; p=p->next)
    write(p);
  
  /*Text lines[] = {
    {"Sam Zofkie", title},
    {"samzofkie@gmail.com  •  github.com/samzofkie  •  samzofkie.com  •  17735510259  •  Chicago, IL", subtitle},
    {"Education", section_title},
    {"Reed College, B.A. Computer Science", (TextStyle){"Cantarell", 12, Left, false, 2, false} },
    {"2016-2020", left_12},
    {"Projects", section_title},
    {"YouTube Game", project_title},
    {"• Created web app to display randomly chosen YouTube videos using Flask and React.", bullet},
    {"• Designed gibberish search word algorithm and implemented crawler in Python to populate SQLite database with video URLs using Python.", bullet},
    {"• Researched and implemented JavaScript CSS technique to hide HTML iframes to improve UI responsiveness.", bullet},
    {"X11 PulseAudio DAW", project_title},
    {"Automated Linux From Scratch", project_title},
    {"Skills", section_title},
    {"Proficient: C, C++, JavaScript, React, Git, Unix", subtitle},
    {"Familiar: Python, Bash, Docker, SQL, AWS, HTML, CSS", subtitle}
  }; */ 
  
  cleanup_doc();

  return 0;
}
