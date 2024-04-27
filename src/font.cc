#include "font.h"
#include <string>
#include <pango/pangocairo.h>


Font::Font(int size, string font_str)
	:_size(size),
	 description(pango_font_description_from_string(font_str.c_str()))
{
	pango_font_description_set_size(description, _size * PANGO_SCALE);	
}

Font::~Font() {
	pango_font_description_free(description);
}

PangoFontDescription *Font::get_description() {
	return description;
}

/* Font's size() is the Font point-- not a Size object. */
int Font::size() {
	return _size;
}