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

class DrawableText : public Drawable {
	public:
		DrawableText(cairo_t *, Font *, const char *);
	protected:
		cairo_t *cr;
		Font *font;
		const char *str;	
};

class UnwrappedText : public DrawableText {
	public:
		UnwrappedText(cairo_t *, Font *, const char *);
		~UnwrappedText();
		Size get_size();
		void draw(Point);
	private:
		PangoLayout *layout;
		Size size;
};

class WrappedText : public DrawableText {
	public:
		WrappedText(cairo_t *, Font *, const char *, double max_width = DOC_WIDTH,
							  double line_spacing = 0);
		Size get_size();
		void draw(Point);
	private:
		double max_width, line_spacing;
		Size size;
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
