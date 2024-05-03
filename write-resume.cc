#include <iostream>
//#include <yaml.h>
#include "document.h"
#include "resume-info.h"
#include "resume-typesetter.h"
using namespace std;

/*void parse_yaml() {
	YAML::Node info = YAML::LoadFile("sam.yaml");
	YAML::Node bullets = info["experience"][0]["bullets"];
	
	for (size_t i=0; i<bullets.size(); i++) {
		if (bullets[i].IsMap()) {
			YAML::Node node = bullets[i].begin()->second;
			for (size_t j=0; j<node.size(); j++) {
				cout << node[j] << "\n" << endl;
			}
			//cout << node << endl;
			//cout << node.IsSequence() << endl;
			//cout << bullets[i].begin()->second << endl;
			//for(YAML::const_iterator it=bullets[i].begin(); it!=bullets[i].end(); ++it) {
				//cout << it->second << endl;
			//}
		}
	}
}*/

int main() {
  Document resume("test-resume.pdf");
  
	using namespace resume_info;

	ResumeInfo info;
	info.name = "My Name";
	info.links = {
		{"mywebsite.com", "https://mywebsite.com"},
		{"me@email.com", "mailto:me@email.com"},
		{"myportfolio.com", "https://myportfolio.com"}
	};
	info.education = {
		{
			"My second degree",
			"what the degree is",
			"when it's from"
		},
		{
			"My College",
			"my degree",
			"grad date"
		}
	};
	info.jobs = {
		{
			{"Most recent job", "link"},
			"Role",
			"from - to",
			"Brief description of what I did",
			{
				{ "First bullet", 0 },
				{ "Second bullet point", 0}
			}
		},
		{
			{"Second most recent job", "link"},
			"Role",
			"from - to",
			"Brief summary",
			{
				{"First bullet", 0},
				{"A sub bullet!", 1},
				{"Second bullet", 0},
				{"Second sub bullet", 1}
			}
		}
	};
	info.projects = {
		{
			{"Project name", "project link"},
			"Project summary",
			{
				{ "First bullet", 0 },
				{ "Second bullet", 0 },
				{ "A sub bullet", 1}
			}
		},
		{
			{"Project name", "project link"},
			"Project summary",
			{
				{ "First bullet", 0 },
				{ "Second bullet", 0 },
				{ "A sub bullet", 1}
			}
		}
	};
	info.skill_categories = {
		{
			"First skill category",
			{
				"skill1", "skill2", "skill3", "skill4"
			}
		},
		{
			"Second skill category",
			{
				"skill1", "skill2", "skill3", "skill4"
			}
		},
		{
			"Third skill category",
			{
				"skill1", "skill2", "skill3", "skill4"
			}
		},
	};
	

  ResumeTypesetter typesetter(resume, info);
	typesetter.write();

  return 0;
}