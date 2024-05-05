#include <iostream>
#include <yaml.h>
#include "document.h"
#include "resume-info.h"
#include "resume-typesetter.h"
using namespace std;

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		cerr << "Usage:\n\t./write-resume <path-to-my-info.yaml>" << endl;
		return 1;
	}

	YAML::Node yaml_file = YAML::LoadFile(argv[1]);

	Document resume(yaml_file["filename"].as<string>());

	resume_info::ResumeInfo info(yaml_file);

  ResumeTypesetter typesetter(resume, info);
	typesetter.write();

  return 0;
}