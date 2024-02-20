#include <vector>
#include <string>
#include <map>
#include <pango/pangocairo.h>
using namespace std;

/* An object representing a font-- an int size, and a string naming the font
	family. This object exists to make use of a destructor that calls a 
	Pango function to free the font. */
class Font {
	public:
		Font(int, string);
		~Font();
		PangoFontDescription *get_description();
		int size();
	private:
		int _size;
		PangoFontDescription *description;
};

struct Point {
	double x, y;
};

struct Size {
	double width, height;
};

/* This is an abstract class for anything that has a 2D size that might be
  nice for someone else to ask for, such as a Text object, a Document, or
	any of the sub-classes of the actual ResumeTypesetter class (which usually
	correspond to sections, like "experience" or "education". */
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

/* DrawableText had a draw function, a Font representing the font to be used,
  and a string member "str", representing the actual text to be rendered. */
class DrawableText : public Drawable {
	public:
		DrawableText(cairo_t *, Font *, string);
	protected:
		cairo_t *cr;
		Font *font;
		string str;	
};

/* UnwrappedText is simple-- it just draws the text at the Point it's draw
	method is pased. */
class UnwrappedText : public DrawableText {
	public:
		UnwrappedText(cairo_t *, Font *, string);
		virtual ~UnwrappedText();
		void draw(Point);
	private:
		PangoLayout *layout;
};

/* WrappedText is a bit more complex-- it takes a max_width double, and in it's
	constructor, uses private methods to calculate the longest UnwrappedText 
	that will fit within it's max_width, and so on, breaking up it's str string
	into a vector of UnwrappedText, none of which exceeds max_width. It's draw()
	method draws each UnwrappedText on top of one another, and it's height() and
	width() methods are overriden to accurately represent the final block of text. 
*/
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

/* The Document object mainly contains a name that the final pdf is saved to,
	 and since it is Sized, the height and width of it. */
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

/* This abstract class respresent an object that draws lots of things on a
	Document that it's passed. */
class Typesetter {
	public:
		Typesetter(Document&);
		virtual void write() = 0;
	
	protected:
		Document& document;
		cairo_t *cr;
};

/* These four structs are just for separating the data ultimately written to
	 resume in a logical structure that makes sense to the ResumeTypesetter. */
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
