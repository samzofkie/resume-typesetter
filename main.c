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

  cleanup_cairo();

  return 0;
}
