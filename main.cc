#include <iostream>
#include "Typesetter.h"

int main() {
	Document *resume = new Document("resume.pdf");
	ResumeInfo info;
	info.name = "My Name";
	info.links = {"myemail@email.com", "website.com/myname", "myname.com"};
	ResumeTypesetter typesetter(resume, info);
	typesetter.write();
	delete resume;
	return 0;
}
