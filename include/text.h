#ifndef RESUME_TYPESETTER_INCLUDE_TEXT_H_
#define RESUME_TYPESETTER_INCLUDE_TEXT_H_

#include <string>
#include <vector>
#include <pango/pangocairo.h>
#include "drawable.h"
#include "font.h"
#include "point.h"
using namespace std;


/* DrawableText had a draw function, a Font representing the font to be used,
  and a string member "str", representing the actual text to be rendered. */
class DrawableText : public Drawable {
	public:
		DrawableText(cairo_t *, Font *, string);
	protected:
		cairo_t *cr;
		Font *font;
		string str;	
};

/* UnwrappedText is simple-- it just draws the text at the Point it's draw
	method is pased. */
class UnwrappedText : public DrawableText {
	public:
		UnwrappedText(cairo_t *, Font *, string);
		virtual ~UnwrappedText();
		void draw(Point);
	private:
		PangoLayout *layout;
};

/* WrappedText is a bit more complex-- it takes a max_width double, and in it's
	constructor, uses private methods to calculate the longest UnwrappedText 
	that will fit within it's max_width, and so on, breaking up it's str string
	into a vector of UnwrappedText, none of which exceeds max_width. It's draw()
	method draws each UnwrappedText on top of one another, and it's height() and
	width() methods are overriden to accurately represent the final block of text. 
*/
class WrappedText : public DrawableText {
	public:
		WrappedText(cairo_t *, Font *, string, double max_width,
							  double line_spacing = 0);
		virtual ~WrappedText();
		void draw(Point);
	private:
		double max_width, line_spacing;
		vector<UnwrappedText*> lines;

		string longest_substring_that_fits(string);
		string rest_of_string(string, size_t);
};

#endif // RESUME_TYPESETTER_INCLUDE_TEXT_H_
