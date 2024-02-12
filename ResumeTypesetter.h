#include <pango/pangocairo.h>

class Font {
	public:
		Font(int, const char *);
		~Font();
		PangoFontDescription *description;
};

class Text {
	public:
		Text(cairo_t*, Font*, const char *);
		~Text();
	private:
		cairo_t *cr;
		const char *str;
		PangoLayout *layout;
		double width, height;
};

class ResumeTypesetter {
	public:
		ResumeTypesetter();
		~ResumeTypesetter();
		void doShit();
		cairo_t *cr;
  private:
		const double DOC_WIDTH = 8.5 * 72,
				         DOC_HEIGHT = 11 * 72,
						     MARGIN = 25;
		cairo_surface_t *surface;
};
