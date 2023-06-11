#ifndef RESUME_H
#define RESUME_H

void init_cairo();
void cleanup_cairo();

void Title(const char *);
void Subtitle(const char *);
void SectionTitle(const char *);
void SplitLine(const char *, const char *);
void SubSectionTitle(const char *);
void Bullet(const char *);
void Center12Point(const char *);

#endif
