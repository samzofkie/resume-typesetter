#ifndef RESUME_TYPESETTER_INCLUDE_RESUME_INFO_H_
#define RESUME_TYPESETTER_INCLUDE_RESUME_INFO_H_

#include <string>
#include <vector>
using namespace std;

/* These four structs are just for separating the data ultimately written to
	 resume in a logical structure that makes sense to the ResumeTypesetter. */
struct Bullet {
	string text;
	vector<string> subbullets = {};
};

struct ProjectDescription {
	string name, summary;
	vector<Bullet> bullets;
};

struct SkillCategory {
	string name;
	vector<string> skills;
};

struct ResumeInfo {
	string name;
	vector<string> links = {};
	string school, degree, school_date;
	vector<ProjectDescription> projects = {};
	vector<SkillCategory> skill_categories = {};
};

#endif // RESUME_TYPESETTER_INCLUDE_RESUME_INFO_H_