#ifndef RESUME_TYPESETTER_INCLUDE_RESUME_TYPESETTER_H_
#define RESUME_TYPESETTER_INCLUDE_RESUME_TYPESETTER_H_

#include <string>
#include <vector>
#include <map>
#include "typesetter.h"
#include "document.h"
#include "resume-info.h"
#include "drawable.h"
#include "font.h"
#include "text.h"
#include "point.h"
#include <pango/pangocairo.h>
using namespace std;

/* ResumeTypesetter is initialized with a ResumeInfo object. */
class ResumeTypesetter : public Typesetter {
	public:
		ResumeTypesetter(Document&, ResumeInfo);
		~ResumeTypesetter();
		void write();

	private:
		class Element : public Drawable {
			public:
				Element(ResumeTypesetter&);
			protected:
				cairo_t *cr;
				ResumeInfo info;
				map<string,Font*> fonts;
				double margin, padding, inner_width;
		};

		/* MaxWidthElement is just Element but with a max_width member that it's
			subclasses must be careful to avoid drawing outside of. It's a way for
			main ResumeTypesetter's logic to easily communicate the x-axis boundaries
			of a section. */
		class MaxWidthElement : public Element {
			public:
				MaxWidthElement(ResumeTypesetter&);
				MaxWidthElement(ResumeTypesetter&, double);
			protected:
				double max_width;
		};

		class Header : public Element {
			public:
				Header(ResumeTypesetter&);
				virtual ~Header();
				void draw(Point);
			private:
				UnwrappedText *name;
				vector<UnwrappedText*> links;
		};

		class Section : public Element {
			public:
				Section(ResumeTypesetter&, string);
				virtual ~Section();
				void draw(Point);
			protected:
				UnwrappedText *title;
		};

		/* Many nested classes inherit from Section so they can use Section::draw()
			in their own draw() method, instead of composition, in which case it
			would need to create and destroy a Section object. This is pretty arbitrary
			though and could easily change in the future. */
		class EducationSection : public Section {
			public:
				EducationSection(ResumeTypesetter&);
				virtual ~EducationSection();
				void draw(Point);
			private:
				UnwrappedText *school, *degree, *date;
		};

		class BulletList : public MaxWidthElement {
			public:
				BulletList(ResumeTypesetter&, double, vector<Bullet>);
				virtual ~BulletList();
				void draw(Point);
			private:
				struct BulletText {
					WrappedText *bullet_text;
					vector<WrappedText*> subbullet_texts;
				};
				double bullet_spacing;
				UnwrappedText *bullet, *subbullet;
				vector<BulletText*> bullet_texts;	
		};

		class Project : public MaxWidthElement {
			public:
				Project(ResumeTypesetter&, double, ProjectDescription);
				virtual ~Project();
				void draw(Point);
			private:
				UnwrappedText *name;
				WrappedText *summary;
				BulletList *bullets;	
		};

		class ExperienceSection : public Section {
			public:
				ExperienceSection(ResumeTypesetter&);
				virtual ~ExperienceSection();
				void draw(Point);
			private:
				vector<Project*> projects;
		};

		class SkillsSection : public Section {
			public:
				SkillsSection(ResumeTypesetter&);
				virtual ~SkillsSection();
				void draw(Point);
			private:
				struct SkillsTexts {
					UnwrappedText *category;
					WrappedText *skills_list;
				};
				vector<SkillsTexts*> skills;
		};

		ResumeInfo info;
		map<string,Font*> fonts;
		double margin, padding, inner_width;
		Header *header;
		EducationSection *education;
		ExperienceSection *experience;
		SkillsSection *skills;
};

#endif // RESUME_TYPESETTER_INCLUDE_RESUME_TYPESETTER_H_