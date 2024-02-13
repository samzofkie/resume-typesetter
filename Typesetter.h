#include <vector>
#include <pango/pangocairo.h>

const double DOC_WIDTH = 8.5 * 72,
				     DOC_HEIGHT = 11 * 72;


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


class Drawable {
	public:
		virtual Size get_size() =0;
		virtual void draw(Point) =0;
};


class Text : public Drawable {
	public:
		Text(cairo_t*, Font*, const char *);
		~Text();

		Size get_size();
		void draw(Point);
	private:
		cairo_t *cr;
		const char *str;
		PangoLayout *layout;
		Size size;

		int index_of_first_space;
		bool str_fits(const char *, double);
		int length_of_longest_string_that_fits(const char *, double);
};


class LineWrappedText : public Text {
	public:
		LineWrappedText(cairo_t*, Font*, const char *, double max_width =DOC_WIDTH, 
										double line_spacing =0);
	private:
		double max_width, line_spacing;
};


class Typesetter {
	public:
		Typesetter();
		~Typesetter();
 
	private:
		const double MARGIN = 25;
		cairo_surface_t *surface;
		cairo_t *cr;
		const char *font_name;
		Font *small, *medium, *large;
		bool str_fits(const char *, double);
};
