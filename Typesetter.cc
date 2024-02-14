#include "Typesetter.h"
#include <iostream>
#include <vector>
#include <pango/pangocairo.h>
#include <cairo-pdf.h>
using namespace std;

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

int WrappedText::index_of_first_space(const char *str) {
	int i = 0;
	while (str[i] != ' ' && str[i] != 0) { 
		i++;
	}
	return i;
}

int WrappedText::length_longest_string_that_fits(const char *str, 
																						     double max_width) {
	
	size_t i = 0, prev = 0;
	while (i < strlen(str)) {
		i += index_of_first_space(str + i);
		
		char *slice = strdup(str);
		slice[i] = '\0';
		double prospective_width = UnwrappedText(cr, font, slice).get_size().width;
		free(slice);

		if (prospective_width > max_width) 
			return prev;

		prev = i;

		if (str[i] == ' ')
			i++;
	}
	return i;
}

WrappedText::WrappedText(cairo_t *cr, Font *font, const char *str, 
																 double max_width, double line_spacing)
	:DrawableText(cr, font, str), max_width(max_width), line_spacing(line_spacing) 
{
	size_t i = 0;
	while (i < strlen(str)) {
		char *line = strdup(str + i);
		int length = length_longest_string_that_fits(line, max_width);
		line[length] = '\0';
		lines.push_back(new UnwrappedText(cr, font, line));
		free(line);
		i += length;
	}

	size.width = lines.size() > 1 ? max_width : lines[0]->get_size().width;
	size.height = (lines.size() + line_spacing) * lines.size();
}

WrappedText::~WrappedText() {
	for (long unsigned int i=0; i<lines.size(); i++)
		delete lines[i];
}

Size WrappedText::get_size() {
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
	WrappedText text(cr, &font, "this is a very very very long line. my name is sam whats up. how is it that you are today. whats bracking. whats up. yea yea yea yea yea yea yea yea. coolio. this is a very very very long line. my name is sam whats up. how is it that you are today. whats bracking. whats up. yea yea yea yea yea yea yea yea. coolio.");
}

Typesetter::~Typesetter() {
	delete small;
	delete medium;
	delete large;
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}
