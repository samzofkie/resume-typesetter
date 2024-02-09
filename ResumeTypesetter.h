#include <pango/pangocairo.h>

class ResumeTypesetter {
	public:
		ResumeTypesetter();
		~ResumeTypesetter();
		void doShit();
  private:
		const double DOC_WIDTH = 8.5 * 72,
				         DOC_HEIGHT = 11 * 72,
						     MARGIN = 25;
		cairo_surface_t *surface;
		cairo_t *cr;
};
