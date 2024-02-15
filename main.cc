#include <iostream>
#include "Typesetter.h"

int main() {
	Document *resume = new Document("sam_zofkie_resume.pdf");
	ResumeTypesetter typesetter(resume);
	delete resume;
	return 0;
}
