#ifndef RESUME_TYPESETTER_INCLUDE_RESUME_INFO_H_
#define RESUME_TYPESETTER_INCLUDE_RESUME_INFO_H_

#include <string>
#include <vector>
#include <yaml.h>
using namespace std;

namespace resume_info {
	struct Text {
		string text, link = "";
	};

	struct Bullet {
		string text;
		int indentation;
	};

	struct EducationInfo {
		string school, degree, date;
	};

	struct JobInfo {
		Text company;
		string role, date, summary;
		vector<Bullet> bullets;
	};

	struct ProjectInfo {
		Text name;
		string summary;
		vector<Bullet> bullets;
	};

	struct SkillCategory {
		string name;
		vector<string> skills;
	};

	class ResumeInfo {
		public:
			ResumeInfo() = default;
			ResumeInfo(YAML::Node);
			
			string name;
			vector<Text> links = {};
			vector<EducationInfo> education = {};
			vector<JobInfo> jobs = {};
			vector<ProjectInfo> projects = {};
			vector<SkillCategory> skill_categories = {};

		private:
			vector<Bullet> parse_bullets(YAML::Node);
	};
}
#endif // RESUME_TYPESETTER_INCLUDE_RESUME_INFO_H_