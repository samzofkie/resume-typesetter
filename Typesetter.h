#include <vector>
#include <string>
#include <map>
#include <pango/pangocairo.h>
using namespace std;

class Font {
	public:
		Font(int, string);
		~Font();
		PangoFontDescription *get_description();
	private:	
		PangoFontDescription *description;
};

struct Point {
	double x, y;
};

struct Size {
	double width, height;
};

class Sized {
	public:
		Size get_size();
		double width();
		double height();
	protected:
		Size size;
};

class Drawable : public Sized {
	public:
		virtual void draw(Point) = 0;
};

class DrawableText : public Drawable {
	public:
		DrawableText(cairo_t *, Font *, string);
	protected:
		cairo_t *cr;
		Font *font;
		string str;	
};

class UnwrappedText : public DrawableText {
	public:
		UnwrappedText(cairo_t *, Font *, string);
		virtual ~UnwrappedText();
		void draw(Point);
	private:
		PangoLayout *layout;
};

class WrappedText : public DrawableText {
	public:
		WrappedText(cairo_t *, Font *, string, double max_width,
							  double line_spacing = 0);
		virtual ~WrappedText();
		void draw(Point);
	private:
		double max_width, line_spacing;
		std::vector<UnwrappedText*> lines;

		string longest_substring_that_fits(string);
		string rest_of_string(string, size_t);
};

class Document : public Sized {
	public:
		Document(string, Size size = {8.5 * 72, 11 * 72});
		virtual ~Document();
	private:
		string name;
		cairo_surface_t *surface;
	public:
		cairo_t *cr;
};

class Typesetter {
	public:
		Typesetter(Document *);
		virtual void write() = 0;
	
	protected:
		Document *document;
		cairo_t *cr;
};

struct Bullet {
	string text;
	vector<string> subbullets = {};
};

struct ProjectDescription {
	string name, summary;
	vector<Bullet> bullets;
};

struct SkillCategory {
	string name;
	vector<string> skills;
};

struct ResumeInfo {
	string name;
	vector<string> links = {};
	string school, degree, school_date;
	vector<ProjectDescription> projects = {};
	vector<SkillCategory> skill_categories = {};
};

class ResumeTypesetter : public Typesetter {
	public:
		ResumeTypesetter(Document *, ResumeInfo);
		~ResumeTypesetter();
		void write();

	private:
		class Element : public Drawable {
			public:
				Element(ResumeTypesetter&);
			protected:
				ResumeTypesetter& typesetter;
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

		class EducationSection : public Section {
			public:
				EducationSection(ResumeTypesetter&);
				virtual ~EducationSection();
				void draw(Point);
			private:
				UnwrappedText *school, *degree, *date;
		};

		class BulletList : public Element {
			public:
				BulletList(ResumeTypesetter&, vector<Bullet>);
				virtual ~BulletList();
				void draw(Point);
			private:
				struct BulletText {
					WrappedText *bullet_text;
					vector<WrappedText*> subbullet_texts;
				};
				UnwrappedText *bullet, *subbullet;
				double bullet_spacing;
				vector<BulletText*> bullet_texts;	
		};

		class Project : public Element {
			public:
				Project(ResumeTypesetter&, ProjectDescription);
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
