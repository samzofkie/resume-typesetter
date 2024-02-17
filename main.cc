#include "Typesetter.h"

int main() {
	Document *resume = new Document("resume.pdf");

	ResumeInfo info;
	info.name = "My Name";
	info.links = {"myemail@email.com", "website.com/myname", "myname.com"};
	info.school = "My College",
	info.degree = "A college degree";
	info.school_date = "2024";

	ProjectDescription p1 {
		"My Project",
		"This, in turn, would be the description for thee project, detailed via points of bullet in the below space:",
		{
			{"And indeed, this here, would be where a bullet does indeed lie."},
			{"And here, yet lies another bullet! Hark!", 
				{
					"And this, here-- this is a so-called \"sub-bullet\". A curious phenomenon, nonetheless worth the steep price of implementation twice over, doesn't one agree?",
					"How lovelyly reassuring to find that the sub-bullet functionality has agreed to work for multiple sub-bullets! The text do floweth, that's for certain."
				}
			},
			{"This bullet here is just to make sure that we can nicely return to proper bulletry in spite of the preceeding sub-bulletry."}
		}
	};

	ProjectDescription p2 {
		"Another Proj'",
		"Hey check out this other project! It's actually pretty sick that this all works well without issues for the most part. Woo hoo.",
		{
			{"Bullet"},
			{"Bullet"},
			{"Bullet", {"Sub-bullet"}}
		}
	};

	info.projects = {p1, p2};

	ResumeTypesetter typesetter(resume, info);
	typesetter.write();
	delete resume;
	return 0;
}
