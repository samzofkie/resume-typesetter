#include "ResumeTypesetter.h"
#include <iostream>
#include <pango/pangocairo.h>
#include <cairo-pdf.h>

Font::Font(int size, const char *font_str) {
	description = pango_font_description_from_string(font_str);
	pango_font_description_set_size(description, size * PANGO_SCALE);
	
}

Font::~Font() {
	pango_font_description_free(description);
}

Text::Text(cairo_t *cr, Font *font, const char *str) {
	cr = cr;
	str = str;

	layout = pango_cairo_create_layout(cr);
	pango_layout_set_text(layout, str, -1);
	pango_layout_set_font_description(layout, font->description);

	int _width, _height;
	pango_layout_get_size(layout, &_width, &_height);
	width = (double)_width / PANGO_SCALE;
	height = (double)_height / PANGO_SCALE;
}

Text::~Text() {
	g_object_unref(layout);
}

ResumeTypesetter::ResumeTypesetter() {
	surface = cairo_pdf_surface_create("resume.pdf", DOC_WIDTH, DOC_HEIGHT);
	cr = cairo_create(surface);
}

ResumeTypesetter::~ResumeTypesetter() {
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}

void ResumeTypesetter::doShit() {
	std::cout << "this some shit" << std::endl;
}
