#include <vector>
#include <string>
#include <map>
#include <pango/pangocairo.h>
using namespace std;

const double DOC_WIDTH = 8.5 * 72,
				     DOC_HEIGHT = 11 * 72;

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

class Drawable {
	public:
		virtual Size get_size() =0;
		virtual void draw(Point) =0;
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
		Size get_size();
		void draw(Point);
	private:
		PangoLayout *layout;
		Size size;
};

class WrappedText : public DrawableText {
	public:
		WrappedText(cairo_t *, Font *, string, double max_width = DOC_WIDTH,
							  double line_spacing = 0);
		~WrappedText();
		Size get_size();
		void draw(Point);
	private:
		double max_width, line_spacing;
		Size size;
		std::vector<UnwrappedText*> lines;

		string longest_substring_that_fits(string);
		string rest_of_string(string, size_t);
};

class Document {
	public:
		Document(string);
		~Document();
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
};

class ResumeTypesetter : public Typesetter {
	public:
		ResumeTypesetter(Document *);
		~ResumeTypesetter();
		void write();
	private:
		Document *document;
		map<string,Font*> fonts;

};
