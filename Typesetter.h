#include <vector>
#include <pango/pangocairo.h>

const double DOC_WIDTH = 8.5 * 72,
				     DOC_HEIGHT = 11 * 72;

/* Font constructs and deconstructs a PangoFontDescription object based on
	 it's inputs:
	  	- int size, the font size and
			- const char *font_str, the string name of the font family. */
class Font {
	public:
		Font(int, const char *font_str);
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

/* Text constructs and deconstructs a string rendered with a font with
			- cairo_t *cr, the Cairo object used to calculate the dimensions of the
					           Text once rendered,
			- Font *font, a Font specifying the font to be used, and
			- const char *str, the actual string of the word to be rendered.
	 It has creates and frees it's PangoLayout object, and shared that and two
	 doubles representing the Text's prospective height in width in the context
	 of the document being typeset. 

	Text's .write method is responsible for rendering the text it has created to
	the Cairo object at whatever coordinates it is handed. This includes doing
	all the logic of line-wrapping, if necessary.

	 */
class Text {
	public:
		Text(cairo_t*, Font*, const char *, double max_width =DOC_WIDTH, double line_spacing =0);
		~Text();
		PangoLayout *get_layout();
		Size get_size();
	private:
		cairo_t *cr;
		const char *str;
		const double max_width;
		const double line_spacing;
		PangoLayout *layout;
		Size size;
};

/* Typesetter provides the interface which it's creator and caller can use to
	 specify a document. */
class Typesetter {
	public:
		Typesetter();
		~Typesetter();
		void write(const char *);
 
	private:
		const double MARGIN = 25;
		cairo_surface_t *surface;
		cairo_t *cr;
		const char *font_name;
		Font *small, *medium, *large;
		bool str_fits(const char *, double);
};
