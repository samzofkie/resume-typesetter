### My C++ Resume Typesetter

Using it just might look a little something like this:
```
#include "Typesetter.h"
...
Document resume("resume.pdf");

ResumeInfo info;
info.name = "Hugh Babineaux";

ProjectDescription project1 = {
  "Stealing Stuff", // Project title
  "Yeah I stole never got caught", // Project description
  {
    {"A bullet point", {
        "A sub bullet",
        "Another sub bullet"
    }}
  }
};

info.projects = {project1};

ResumeTypesetter typesetter(document, info);
typesetter.write();
```

I have been compiling with the one-liner shell script `Make` because this is a tiny project. All you'll need is binaries of cairo, pango, and pangocairo.

### Future Ideas
- Abstract element representing sequence of Drawable objects, that are drawn back to back on top of one another.
- More robust text handling (Markdown or HTML like syntax for bold, underline, ect. and paragraphs with nice indentation).
- Read in ResumeInfo from a web form or some sort of .txt file with a primitive syntax.
- Read the resume info from a web form or a .txt file with some sort of very primitive syntax.
- Wrangle cairo-pdf stuff to create proper links to websites.
- Unit-testing, linting, ect.
