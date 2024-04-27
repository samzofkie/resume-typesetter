#ifndef RESUME_TYPESETTER_INCLUDE_DRAWABLE_H_
#define RESUME_TYPESETTER_INCLUDE_DRAWABLE_H_

#include "sized.h"
#include "point.h"


class Drawable : public Sized {
	public:
		virtual void draw(Point) = 0;
};

#endif // RESUME_TYPESETTER_INCLUDE_DRAWABLE_H_