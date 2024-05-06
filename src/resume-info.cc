#include "resume-info.h"
#include <yaml.h>
#include <iostream>
using namespace std;


namespace resume_info {

string node_type(YAML::Node node) {
  if (node.IsSequence()) {
    return "sequence";
  } else if (node.IsMap()) {
    return "map";
  } else if (node.IsScalar()) {
    return "scalar";
  } else {
    return "other";
  }
}

ResumeInfo::ResumeInfo(YAML::Node yaml_file) {

  name = yaml_file["name"].as<string>();

  for (size_t i=0; i<yaml_file["links"].size(); i++) 
    links.push_back({
      yaml_file["links"][i]["text"].as<string>(),
      yaml_file["links"][i]["link"].as<string>()
    });

  for (size_t i=0; i<yaml_file["education"].size(); i++)
    education.push_back({
      yaml_file["education"][i]["school"].as<string>(),
      yaml_file["education"][i]["degree"].as<string>(),
      yaml_file["education"][i]["date"].as<string>()
    });

  for (size_t i=0; i<yaml_file["experience"].size(); i++) {
    YAML::Node job = yaml_file["experience"][i];

    Text company;
    company.text = job["company"]["text"].as<string>();
    if (job["company"]["link"].IsDefined()) {
      company.link = job["company"]["link"].as<string>();
    }

    jobs.push_back({
      company,
      job["role"].as<string>(),
      job["date"].as<string>(),
      job["summary"].as<string>(),
      parse_bullets(job["bullets"])
    });
  }

  for (size_t i=0; i<yaml_file["projects"].size(); i++) {
    YAML::Node project = yaml_file["projects"][i];


    Text name;
    name.text = project["name"]["text"].as<string>();
    if (project["name"]["link"].IsDefined()) {
      name.link = project["name"]["link"].as<string>();
    }

    projects.push_back({
      name,
      project["summary"].as<string>(),
      parse_bullets(project["bullets"])
    });
  }

  for (auto it=yaml_file["skills"].begin(); it != yaml_file["skills"].end(); it++) {
    YAML::Node skill_category = it->second;

    vector<string> skills = {};
    for (size_t i=0; i<skill_category.size(); i++)
      skills.push_back(skill_category[i].as<string>());

    skill_categories.push_back({it->first.as<string>(), skills});
  }
}

vector<Bullet> ResumeInfo::parse_bullets(YAML::Node yaml_bullets) {
  vector<Bullet> bullets = {};

  for (size_t i=0; i<yaml_bullets.size(); i++) {
    YAML::Node bullet = yaml_bullets[i];

    if (bullet.IsScalar()) {
      bullets.push_back({bullet.as<string>(), 0});
    } else {
      for (auto it = bullet.begin(); it != bullet.end(); it++) 
        for (size_t i=0; i<it->second.size(); i++)
          bullets.push_back({it->second[i].as<string>(), 1});        
    }
  }

  return bullets;
}

}