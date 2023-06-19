#ifndef RESUME_H
#define RESUME_H

extern double cursor;

void init_cairo();
void cleanup_cairo();

void Title(const char *);
void Subtitle(const char *);
void SectionTitle(const char *);
void SplitLine(const char *, const char *);
void SubSectionTitle(const char *);
void Center12Point(const char *);
void Bullet(const char *);

void Project(const char *, int, ...);
void Experience(const char *, const char *, const char *, const char *);
void Job(const char *, const char *, const char *, const char *, int, ...);
void BulletSection(int, ...);

#endif
