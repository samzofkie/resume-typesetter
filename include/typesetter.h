#ifndef RESUME_TYPESETTER_INCLUDE_TYPESETTER_H_
#define RESUME_TYPESETTER_INCLUDE_TYPESETTER_H_

#include <pango/pangocairo.h>
#include "document.h"

/* This abstract class respresent an object that draws lots of things on a
	Document that it's passed. */
class Typesetter {
	public:
		Typesetter(Document&);
		virtual void write() = 0;
	
	protected:
		Document& document;
		cairo_t *cr;
};

#endif // RESUME_TYPESETTER_INCLUDE_TYPESETTER_H_