#include "document.h"
#include <string>
#include <pango/pangocairo.h>
#include <cairo-pdf.h>

/* Document mainly manages the Cairo surface and Cairo object *cr. */
Document::Document(string name, Size _size) 
	:name(name),
	 surface(cairo_pdf_surface_create(name.c_str(), _size.width, _size.height)),
	 cr(cairo_create(surface))
{
	size = _size;
	cairo_set_line_width(cr, 0.5);
}

Document::~Document() {
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}