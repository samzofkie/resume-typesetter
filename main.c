#include <cairo-pdf.h>
#include "resume.h"

//extern double doc_width, doc_height;
//extern cairo_surface_t *surface;
//extern cairo_t *cr;

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
  //Bullet("Whats up w all that shit that happens when that thing goes down u dont even know whatll happen when n all that shitll b crazy n shit will go nuts");
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
