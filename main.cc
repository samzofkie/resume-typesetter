#include "ResumeTypesetter.h"

int main() {
	ResumeTypesetter typesetter;
	Font font(12, "IBMPlexSans");
	Text text(typesetter.cr, &font, "Hiya world"); 
	typesetter.doShit();
	return 0;
}
