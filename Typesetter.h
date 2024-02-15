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
		virtual Size get_size();
		virtual double width();
		virtual double height();
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
		~WrappedText();
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

struct ResumeInfo {
	string name;
	vector<string> links;
};

class ResumeTypesetter : public Typesetter {
	public:
		ResumeTypesetter(Document *, ResumeInfo);
		~ResumeTypesetter();
		void write();
	private:
		class ResumeElement : public Drawable {
			public:
				ResumeElement(ResumeTypesetter&);
			private:
				ResumeTypesetter& typesetter;
		};
		
		class ResumeHeader : public ResumeElement {
			public:
				ResumeHeader(ResumeTypesetter&, double);
				virtual ~ResumeHeader();
				void draw(Point);
			private:
				UnwrappedText *name;
				vector<UnwrappedText*> links;
		};

		/*class ResumeSection : public Drawable {
			public ResumeSection(ResumeTypesetter&, do
		};*/

		ResumeInfo info;
		map<string,Font*> fonts;
		double margin;
		ResumeHeader *header;
};
