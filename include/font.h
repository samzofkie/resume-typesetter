#ifndef RESUME_TYPESETTER_INCLUDE_FONT_H_
#define RESUME_TYPESETTER_INCLUDE_FONT_H_

#include <string>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>
using namespace std;


/* An object representing a font-- an int size, and a string naming the font
	family. This object exists to make use of a destructor that calls a 
	Pango function to free the font. */
class Font {
	public:
		Font(int, string);
		~Font();
		PangoFontDescription *get_description();
		int size();
	private:
		int _size;
		PangoFontDescription *description;
};

#endif // RESUME_TYPESETTER_INCLUDE_FONT_H_