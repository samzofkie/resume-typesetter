#include "typesetter.h"
#include <pango/pangocairo.h>
#include "document.h"

/* Typesetters get their cr from the Document they're working with. */
Typesetter::Typesetter(Document &document) 
	:document(document), 
	 cr(document.cr) 
{}