#include "text.h"
#include <string>
#include <vector>
#include <pango/pangocairo.h>
#include "font.h"
using namespace std;

/* Objects implementing DrawableText do the calls to create the prospective
	object to get it's size, to fufill the height() and width() methods from
	Sized, but don't actual render the text to a surface or Document until their
	draw() method is called, with the Point that they're supposed to be drawn at.
	Then the destructor is natually responsible for the clean up Pnago and Cairo
	calls.

	Doing it this way allows for the top level Typesetter program to create a bit
	of text, understand it's prospective height and width, and then render at the
	place it decides by calling it's draw() method. */
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

void UnwrappedText::draw(Point point) {
	cairo_move_to(cr, point.x, point.y);
	pango_cairo_show_layout(cr, layout);
}

/* This helper function finds the longest space-separated sub string of it's
	argument (str) that fits into max_width. It does this by creating an
	UnwrappedText on the stack and querying it's width in the condition of the
	while loop. */
string WrappedText::longest_substring_that_fits(string str) {
	size_t substr_length = 0, prev_length= 0;
	string substr = str.substr(0, substr_length);
	
	while (UnwrappedText(cr, font, substr).width() < max_width) {
		prev_length = substr_length;
		substr_length = str.find(' ', substr_length + 1);
		substr = str.substr(0, substr_length);
	};
	
	return str.substr(0, prev_length);
}

string WrappedText::rest_of_string(string str, size_t start) {
	return str.substr(start, str.size() - start);
}

WrappedText::WrappedText(cairo_t *cr, 
												 Font *font,
												 string str, 
												 double max_width, 
												 double line_spacing)
	:DrawableText(cr, font, str), max_width(max_width), line_spacing(line_spacing) 
{

	string _str = str, line;

	while (UnwrappedText(cr, font, _str).width() > max_width) {
		line = longest_substring_that_fits(_str);
		lines.push_back(new UnwrappedText(cr, font, line));
		_str = rest_of_string(_str, line.size() + 1);
	}
	lines.push_back(new UnwrappedText(cr, font, _str));

	size.width = lines.size() > 1 ? max_width : lines[0]->width();
	size.height = (lines.size() + line_spacing) * lines[0]->height();
}

WrappedText::~WrappedText() {
	for (long unsigned int i=0; i<lines.size(); i++)
		delete lines[i];
}

void WrappedText::draw(Point point) {
	Point cursor = point;
	for (long unsigned int i=0; i<lines.size(); i++) {
		lines[i]->draw(cursor);
		cursor.y += lines[i]->height() + line_spacing;
	}
}