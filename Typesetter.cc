#include "Typesetter.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
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

Size Sized::get_size() { return size; }
double Sized::width() { return size.width; }
double Sized::height() { return size.height; }

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

void WrappedText::draw(Point point) {
	Point cursor = point;
	for (long unsigned int i=0; i<lines.size(); i++) {
		lines[i]->draw(cursor);
		cursor.y += lines[i]->get_size().height + line_spacing;
	}
}

Document::Document(string name, Size size) :Sized(), name(name),
	surface(cairo_pdf_surface_create(name.c_str(), size.width, size.height)),
	 cr(cairo_create(surface)) {
	size = size;
}

Document::~Document() {
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}

Typesetter::Typesetter(Document *document) 
	:document(document), cr(document->cr) {}

ResumeTypesetter::ResumeTypesetter(Document *document, ResumeInfo info) 
	:Typesetter(document), info(info) {
	string main_font = "IBMPlexSans";
	string bold_font = main_font + " Bold";
	string italic_font = main_font + " Italic";

	int small = 10, medium = 14, large = 18;

	fonts["small"] = new Font(small, main_font);
	fonts["section"] = new Font(medium, bold_font);
	fonts["name"] = new Font(large, bold_font);

	margin = 25;
	
	//header = new ResumeHeader(*this, document->get_size().width - (margin * 2));
}

ResumeTypesetter::~ResumeTypesetter() {
	for(map<string,Font*>::iterator i = fonts.begin(); i!=fonts.end(); i++)
		delete i->second;
	//delete header;
}

void ResumeTypesetter::write() {
	//header->draw({margin, margin});
}

ResumeTypesetter::ResumeElement::ResumeElement(ResumeTypesetter &typesetter,
																						   double max_width)
	:typesetter(typesetter)
{
	size = {max_width, 0};
}

Size ResumeTypesetter::ResumeElement::get_size() {
	return size;
}

/*ResumeTypesetter::ResumeHeader::ResumeHeader(ResumeTypesetter &typesetter,
																						 double max_width) 
	:typesetter(typesetter) 
{
	size.width = max_width;
	size.height = 0;
	for (size_t i = 0; i < typesetter.info.links.size(); i++) {
		UnwrappedText *p = new UnwrappedText(typesetter.cr, 
																			typesetter.fonts["small"], 
																			typesetter.info.links[i]);
		size.height += p->get_size().height;
		links.push_back(p);
	}
	name = new UnwrappedText(typesetter.cr, typesetter.fonts["name"], 
													 typesetter.info.name);
}

ResumeTypesetter::ResumeHeader::~ResumeHeader() {
	for (size_t i = 0; i < links.size(); i++)
		delete links[i];
	delete name;
}*/

/*Size ResumeTypesetter::ResumeHeader::get_size() {
	return size;
}*/

/*void ResumeTypesetter::ResumeHeader::draw(Point point) {
	Point cursor = point;

	for (size_t i=0; i < links.size(); i++) {
		cursor.x += (size.width - links[i]->get_size().width);
		links[i]->draw(cursor);
		cursor.x = point.x;
		cursor.y += links[i]->get_size().height;
	}
	
	cursor.y -= name->get_size().height;
	cursor.x = point.x;
	name->draw(cursor);

}*/
