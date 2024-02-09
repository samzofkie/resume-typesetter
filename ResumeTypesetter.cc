#include "ResumeTypesetter.h"
#include <iostream>
#include <pango/pangocairo.h>
#include <cairo-pdf.h>


ResumeTypesetter::ResumeTypesetter() {
	surface = cairo_pdf_surface_create("resume.pdf", DOC_WIDTH, DOC_HEIGHT);
	cr = cairo_create(surface);
}

ResumeTypesetter::~ResumeTypesetter() {
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}

void ResumeTypesetter::doShit() {
	std::cout << "this some shit" << std::endl;
}
