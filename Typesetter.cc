#include "Typesetter.h"
#include <iostream>
#include <vector>
#include <string>
#include <pango/pangocairo.h>
#include <cairo-pdf.h>
using namespace std;

Font::Font(int size, string font_str) 
	:description(pango_font_description_from_string(font_str.c_str()))
{
	pango_font_description_set_size(description, size * PANGO_SCALE);	
}

Font::~Font() {
	pango_font_description_free(description);
}

PangoFontDescription *Font::get_description() {
	return description;
}

DrawableText::DrawableText(cairo_t *cr, Font *font, string str) 
	:cr(cr), font(font), str(str) {}

UnwrappedText::UnwrappedText(cairo_t *cr, Font *font, string str)
	:DrawableText(cr, font, str), layout(pango_cairo_create_layout(cr))
{
	pango_layout_set_text(layout, str.c_str(), -1);
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

string WrappedText::longest_substring_that_fits(string str) {
	size_t substr_length = 0, prev_length= 0;
	string substr = str.substr(0, substr_length);
	
	while (UnwrappedText(cr, font, substr).get_size().width < max_width) {
		prev_length = substr_length;
		substr_length = str.find(' ', substr_length + 1);
		substr = str.substr(0, substr_length);
	};
	
	return str.substr(0, prev_length);
}

string WrappedText::rest_of_string(string str, size_t start) {
	return str.substr(start, str.size() - start);
}

WrappedText::WrappedText(cairo_t *cr, Font *font, string str, 
																 double max_width, double line_spacing)
	:DrawableText(cr, font, str), max_width(max_width), line_spacing(line_spacing) 
{

	string _str = str, line;

	while (UnwrappedText(cr, font, _str).get_size().width > max_width) {
		line = longest_substring_that_fits(_str);
		lines.push_back(new UnwrappedText(cr, font, line));
		_str = rest_of_string(_str, line.size() + 1);
	}
	lines.push_back(new UnwrappedText(cr, font, _str));

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
	Point cursor = point;
	for (long unsigned int i=0; i<lines.size(); i++) {
		lines[i]->draw(cursor);
		cursor.y += lines[i]->get_size().height + line_spacing;
	}
}

Typesetter::Typesetter() 
	:surface(cairo_pdf_surface_create("resume.pdf", DOC_WIDTH, DOC_HEIGHT)),
	 cr(cairo_create(surface)), font_name("IBMPlexSans"),
	 small(new Font(10, font_name)), medium(new Font(14, font_name)),
	 large(new Font(18, font_name))
{
	Font font(10, "Arial");
	WrappedText text(cr, &font, "this is a very very very long line. my name is sam whats up. how is it that you are today. whats bracking. whats up. yea yea yea yea yea yea yea yea. coolio. this is a very very very long line. my name is sam whats up. how is it that you are today. whats bracking. whats up. yea yea yea yea yea yea yea yea. coolio.");
	text.draw({0, 0});
}

Typesetter::~Typesetter() {
	delete small;
	delete medium;
	delete large;
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}
