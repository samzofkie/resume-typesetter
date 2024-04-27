#ifndef RESUME_TYPESETTER_INCLUDE_DOCUMENT_H_
#define RESUME_TYPESETTER_INCLUDE_DOCUMENT_H_

#include <string>
#include <cairo-pdf.h>
#include "sized.h"
using namespace std;


/* The Document object mainly contains a name that the final pdf is saved to,
	 and since it is Sized, the height and width of it. */
class Document : public Sized {
	public:
		Document(string, Size size = {8.5 * 72, 11 * 72});
		virtual ~Document();
	private:
		string name;
		cairo_surface_t *surface;
	public:
		cairo_t *cr;
};

#endif // RESUME_TYPESETTER_INCLUDE_DOCUMENT_H_