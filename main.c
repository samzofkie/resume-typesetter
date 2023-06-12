//#include <cairo-pdf.h>
#include "resume.h"


int main (int argc, char **argv) { 
  init_cairo();

  Title("Sam Zofkie");
  Subtitle("samzofkie@gmail.com  •  github.com/samzofkie  •  samzofkie.com");
  SectionTitle("Education");
  SplitLine("Reed College, B.A. Computer Science", "2016-2020");
  SectionTitle("Projects");
  SubSectionTitle("Automated Linux From Scratch");
  SubSectionTitle("Youtube Game");
  Bullet("Created web app to display randomly chosen YouTube videos using Flask and React.");
  Bullet("Designed gibberish search word algorithm and implemented crawler in Python to populate SQLite database with video URLs using Python.");
  Bullet("Researched and implemented JavaScript CSS technique to hide HTML iframes to improve UI responsiveness.");
  SubSectionTitle("X11 PulseAudio DAW");
  SectionTitle("Skills");
  Center12Point("Proficient: C, C++, JavaScript, React, Git, Unix");
  Center12Point("Familiar: Python, Bash, Docker, SQL, AWS, HTML, CSS");
   
  cleanup_cairo();

  return 0;
}
