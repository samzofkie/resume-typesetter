#ifndef RESUME_TYPESETTER_INCLUDE_SIZED_H_
#define RESUME_TYPESETTER_INCLUDE_SIZED_H_

struct Size {
	double width, height;
};

/* This is an abstract class for anything that has a 2D size that might be
  nice for someone else to ask for, such as a Text object, a Document, or
	any of the sub-classes of the actual ResumeTypesetter class (which usually
	correspond to sections, like "experience" or "education". */
class Sized {
	public:
		Size get_size();
		double width();
		double height();
	protected:
		Size size;
};

#endif // RESUME_TYPESETTER_INCLUDE_SIZED_H_