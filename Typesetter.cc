#include "Typesetter.h"
#include <iostream>
#include <vector>
#include <pango/pangocairo.h>
#include <cairo-pdf.h>

Font::Font(int size, const char *font_str) 
	:description(pango_font_description_from_string(font_str))
{
	pango_font_description_set_size(description, size * PANGO_SCALE);	
}

Font::~Font() {
	pango_font_description_free(description);
}

PangoFontDescription *Font::get_description() {
	return description;
}

Text::Text(cairo_t *cr, Font *font, const char *str, double max_width, 
					 double line_spacing) 
	:cr(cr), str(str), max_width(max_width), line_spacing(line_spacing), 
	 layout(pango_cairo_create_layout(cr))
{	
	pango_layout_set_text(layout, str, -1);
	pango_layout_set_font_description(layout, font->get_description());

	int _width, _height;
	pango_layout_get_size(layout, &_width, &_height);
	size.width = (double)_width / PANGO_SCALE;
	size.height = (double)_height / PANGO_SCALE;
}

Text::~Text() {
	g_object_unref(layout);
}

PangoLayout *Text::get_layout() {
	return layout;
}

Size Text::get_size() {
	return size;
}

Typesetter::Typesetter() 
	:surface(cairo_pdf_surface_create("resume.pdf", DOC_WIDTH, DOC_HEIGHT)),
	 cr(cairo_create(surface)), font_name("IBMPlexSans"),
	 small(new Font(10, font_name)), medium(new Font(14, font_name)),
	 large(new Font(18, font_name))
{}

Typesetter::~Typesetter() {
	delete small;
	delete medium;
	delete large;
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}

void Typesetter::write(const char *str) {
	Text *text = new Text(cr, small, str);

	std::cout << str_fits(str, 10);
		
	cairo_move_to(cr, 0, 0);
	pango_cairo_show_layout(cr, text->get_layout());
	delete text;
}

bool Typesetter::str_fits(const char *str, double width) {	
	return Text(cr, small, str).get_size().width < width;
}

/*int longest_string_that_fits_in(const char *str, double width) {

}*/
