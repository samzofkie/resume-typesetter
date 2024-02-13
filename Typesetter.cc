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

DrawableText::DrawableText(cairo_t *cr, Font *font, const char *str) 
	:cr(cr), font(font), str(str) {}

UnwrappedText::UnwrappedText(cairo_t *cr, Font *font, const char *str)
	:DrawableText(cr, font, str), layout(pango_cairo_create_layout(cr))
{
	pango_layout_set_text(layout, str, -1);
	pango_layout_set_font_description(layout, font->get_description());

	int _width, _height;
	pango_layout_get_size(layout, &_width, &_height);
	size.width = (double)_width / PANGO_SCALE;
	size.height = (double)_height / PANGO_SCALE;
}

UnwrappedText::~UnwrappedText() {
	g_object_unref(layout);
}

Size UnwrappedText::get_size() {
	return size;
}

void UnwrappedText::draw(Point point) {
	cairo_move_to(cr, point.x, point.y);
	pango_cairo_show_layout(cr, layout);
}

WrappedText::WrappedText(cairo_t *cr, Font *font, const char *str, 
																 double max_width, double line_spacing)
	:DrawableText(cr, font, str), max_width(max_width), line_spacing(line_spacing) 
{
	// TODO: Break str up into a vector of UnwrappedText*
	//       Calculate and define size
}

Size WrappedText::get_size() {
	// TODO
	return size;
}

void WrappedText::draw(Point point) {
	cairo_move_to(cr, point.x, point.y);
	// TODO
}

Typesetter::Typesetter() 
	:surface(cairo_pdf_surface_create("resume.pdf", DOC_WIDTH, DOC_HEIGHT)),
	 cr(cairo_create(surface)), font_name("IBMPlexSans"),
	 small(new Font(10, font_name)), medium(new Font(14, font_name)),
	 large(new Font(18, font_name))
{
	Font font(10, "Arial");
	UnwrappedText text(cr, &font, "hey there");
}

Typesetter::~Typesetter() {
	delete small;
	delete medium;
	delete large;
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}
