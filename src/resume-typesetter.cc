#include "resume-typesetter.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "typesetter.h"
#include "document.h"
#include "drawable.h"
#include "resume-info.h"
#include "text.h"
#include "point.h"
#include <pango/pangocairo.h>
using namespace std;
using namespace resume_info;

/* The nested classes of ResumeTypesetter are all passed a reference to the 
	outside class, so they can read it's properties such as it's font map, and 
	the margin, padding, and inner_width values. */
ResumeTypesetter::ResumeTypesetter(Document &document, ResumeInfo info) 
	:Typesetter(document), info(info) 
{
	string main_font = "IBMPlexSans";
	string bold_font = main_font + " Bold";
	string italic_font = main_font + " Italic";

	int small = 8, medium = 10, large = 18, unbold_large = 12;

	fonts["small"] = new Font(small, main_font);
	fonts["medium"] = new Font(medium, main_font);
	fonts["large"] = new Font(large, main_font);
	fonts["section"] = new Font(unbold_large, main_font);
	fonts["name"] = new Font(large, bold_font);
	fonts["small bold"] = new Font(small, bold_font);
	fonts["small italic"] = new Font(small, italic_font);
	fonts["medium bold"] = new Font(medium, bold_font);

	margin = 25;
	padding = 10;
	inner_width = document.width() - (margin * 2);

	header = new Header(*this);
	education = new EducationSection(*this);
	jobs = new JobsSection(*this);
	projects = new ProjectsSection(*this);
	skills = new SkillsSection(*this);
}

ResumeTypesetter::~ResumeTypesetter() {
	for(map<string,Font*>::iterator i = fonts.begin(); i!=fonts.end(); i++)
		delete i->second;
	delete header;
	delete education;
	delete jobs;
	delete projects;
	delete skills;
}

void ResumeTypesetter::write() {
	Point cursor {margin, margin};
	header->draw(cursor);
	cursor.y += header->height() + padding;

	education->draw(cursor);
	cursor.y += education->height();

	jobs->draw(cursor);
	cursor.y += jobs->height();

	projects->draw(cursor);
	cursor.y += projects->height();

	skills->draw(cursor);
	cursor.y += skills->height() + padding;
}

ResumeTypesetter::Element::Element(ResumeTypesetter &typesetter)
	 :cr(typesetter.cr),
	  info(typesetter.info), 
	  fonts(typesetter.fonts),
	  margin(typesetter.margin), 
	  padding(typesetter.padding),
	  inner_width(typesetter.inner_width) 
{}

ResumeTypesetter::MaxWidthElement::MaxWidthElement(
	ResumeTypesetter &typesetter
)
	:Element(typesetter)
{
	max_width = typesetter.inner_width;
}

ResumeTypesetter::MaxWidthElement::MaxWidthElement(
	ResumeTypesetter &typesetter,
	double max_width
)
	:Element(typesetter),
	 max_width(max_width)
{}

ResumeTypesetter::Header::Header(ResumeTypesetter &typesetter) 
	:Element(typesetter),
	 name(new UnwrappedText(cr, fonts["name"], info.name))
{
	size.width = inner_width;
	size.height = 0;
	for (size_t i = 0; i < info.links.size(); i++) {
		UnwrappedText *p = new UnwrappedText(cr, fonts["small"], info.links[i].text);
		size.height += p->height();
		links.push_back(p);
	}
}

ResumeTypesetter::Header::~Header() {
	for (size_t i = 0; i < links.size(); i++)
		delete links[i];
	delete name;
}

void ResumeTypesetter::Header::draw(Point point) {
	Point cursor = point;
	
	cairo_set_source_rgb(cr, 0, 0, 1);
	for (size_t i=0; i < links.size(); i++) {
		cursor.x += (size.width - links[i]->width());
		cairo_tag_begin(cr, CAIRO_TAG_LINK, ("uri='" + info.links[i].link + "'").c_str());
		links[i]->draw(cursor);
		cairo_tag_end(cr, CAIRO_TAG_LINK);

		int underline_height = fonts["small"]->size() / 5;
		
		cursor.y += links[i]->height() - underline_height;
		cairo_move_to(cr, cursor.x, cursor.y);
		cairo_line_to(cr, cursor.x + links[i]->width(), cursor.y);
		cairo_stroke(cr);
		cursor.y += underline_height;

		cursor.x = point.x;
	}
	cairo_set_source_rgb(cr, 0, 0, 0);
	
	cursor.y -= name->height();
	cursor.x = point.x;
	name->draw(cursor);
}

ResumeTypesetter::Section::Section(ResumeTypesetter &typesetter, string name)
	:Element(typesetter),
	 title(new UnwrappedText(cr, fonts["section"], name))
{
	size = {inner_width, title->height()};
}

ResumeTypesetter::Section::~Section() {
	delete title;
}

void ResumeTypesetter::Section::draw(Point point) {
	Point cursor = point;
	title->draw(cursor);
	double xpadding = 3, ypadding = 2;

	cursor.x = point.x + title->width() + xpadding;
	cursor.y = point.y + ypadding + title->height() / 2;
	cairo_move_to(cr, cursor.x, cursor.y);

	cursor.x = point.x + size.width;
	cairo_line_to(cr, cursor.x, cursor.y);
	cairo_stroke(cr);
}

ResumeTypesetter::EducationSection::EducationSection(
	ResumeTypesetter &typesetter
) 
	:Section(typesetter, "Education"),
	  comma_seperator_symbol(new UnwrappedText(cr, fonts["small"], ", "))
{
	for (size_t i=0; i<info.education.size(); i++) {
		education_texts.push_back(new EducationTexts);
		EducationTexts *curr = education_texts[i];
		curr->school = new UnwrappedText(cr, fonts["small bold"], info.education[i].school);
		curr->degree = new UnwrappedText(cr, fonts["small"], info.education[i].degree);
		curr->date = new UnwrappedText(cr, fonts["small"], info.education[i].date);
		size.height += curr->school->height() + padding / 2;
	}
}

ResumeTypesetter::EducationSection::~EducationSection() {
	delete comma_seperator_symbol;
	for (size_t i=0; i<education_texts.size(); i++) {
		EducationTexts *curr = education_texts[i];
		delete curr->school;
		delete curr->degree;
		delete curr->date;
	}
}

void ResumeTypesetter::EducationSection::draw(Point point) {
	Point cursor = point;
	Section::draw(cursor);
	cursor.y += title->height() + padding / 2;
	cursor.x += padding / 2;

	for (vector<EducationTexts*>::iterator it = education_texts.begin(); it != education_texts.end(); it++) {
		EducationTexts *curr = *it;
		curr->school->draw(cursor);
		cursor.x += curr->school->width();
		comma_seperator_symbol->draw(cursor);
		cursor.x += comma_seperator_symbol->width();
		curr->degree->draw(cursor);
		cursor.x = point.x + size.width - curr->date->width() - padding / 2;
		curr->date->draw(cursor);
		cursor.y += curr->school->height();
		cursor.x = point.x + padding / 2;
	}
}

ResumeTypesetter::BulletList::BulletList(
	ResumeTypesetter &typesetter,
	double max_width,
	vector<Bullet> bullets
)
	:MaxWidthElement(typesetter, max_width), 
	 bullets(bullets),
	 bullet_spacing(15),
	 bullet_symbol(new UnwrappedText(cr, fonts["small"], "•")),
	 subbullet_symbol(new UnwrappedText(cr, fonts["small"], "○"))
{
	size.height = 0;
	
	for (size_t i=0; i<bullets.size(); i++) {
		WrappedText *new_bullet = new WrappedText(
			cr,
			fonts["small"],
			bullets[i].text,
			max_width - (bullet_spacing * bullets[i].indentation)
		);
		size.height += new_bullet->height();
		bullet_texts.push_back(new_bullet);
	}
	size.width = inner_width - padding * 3;
}

ResumeTypesetter::BulletList::~BulletList() {
	delete bullet_symbol;
	delete subbullet_symbol;
	for (size_t i=0; i<bullet_texts.size(); i++) {
		delete bullet_texts[i];
	}
}

void ResumeTypesetter::BulletList::draw(Point point) {
	Point cursor = point;
	point.x += 0;
  for (size_t i=0; i<bullet_texts.size(); i++) {
		WrappedText *curr = bullet_texts[i];
		cursor.x = point.x + (bullets[i].indentation * bullet_spacing);

		if (bullets[i].indentation % 2 == 0)
			bullet_symbol->draw(cursor);
		else 
			subbullet_symbol->draw(cursor);
		cursor.x += bullet_spacing;
		
		curr->draw(cursor);
		cursor.y += curr->height();
	}
}

ResumeTypesetter::Job::Job(
	ResumeTypesetter &typesetter,
	double max_width,
	JobInfo job_info
)
	:MaxWidthElement(typesetter, max_width),
	 job_info(job_info),
	 company(new UnwrappedText(cr, fonts["medium bold"], job_info.company.text)),
	 role(new UnwrappedText(cr, fonts["medium bold"], job_info.role)),
	 date(new UnwrappedText(cr, fonts["small"], job_info.date)),
	 summary(new WrappedText(
		 cr, 
		 fonts["small"],
		 job_info.summary,
		 max_width - padding
	 )),
	 bullets(new BulletList(
		 typesetter, 
		 max_width - padding,
		 job_info.bullets
	 )),
	 separator_symbol(new UnwrappedText(cr, fonts["small"], "— "))
{
	size = {max_width, company->height() + summary->height() + bullets->height()};
}

ResumeTypesetter::Job::~Job() {
	delete company;
	delete role;
	delete date;
	delete summary;
	delete bullets;
	delete separator_symbol;
}

void ResumeTypesetter::Job::draw(Point point) {
	Point cursor = point;

	if (job_info.company.link.size()) {
		cairo_set_source_rgb(cr, 0, 0, 1);
		cairo_tag_begin(cr, CAIRO_TAG_LINK, ("uri='" + job_info.company.link + "'").c_str());
		company->draw(cursor);

		int underline_height = fonts["medium bold"]->size() / 5;
		double y_start = cursor.y;
		cursor.y += company->height() - underline_height;
		cairo_move_to(cr, cursor.x, cursor.y);
		cairo_line_to(cr, cursor.x + company->width(), cursor.y);
		cairo_stroke(cr);
		cursor.y = y_start;

		cairo_tag_end(cr, CAIRO_TAG_LINK);
		cairo_set_source_rgb(cr, 0, 0, 0);
	} else {
		company->draw(cursor);
	}
	cursor.x += company->width();
	
	separator_symbol->draw({
		cursor.x,
		cursor.y + 2
	});
	cursor.x += separator_symbol->width();
	
	role->draw(cursor);
	cursor.x = margin + padding + max_width - date->width();
	cursor.y += role->height() - date->height();
	
	date->draw(cursor);
	cursor.x = point.x + padding;
	cursor.y += date->height();

	summary->draw(cursor);
	cursor.y += summary->height();

	bullets->draw(cursor);
};

ResumeTypesetter::JobsSection::JobsSection(
	ResumeTypesetter &typesetter
) 
	:Section(typesetter, "Experience")
{
	for (size_t i=0; i<info.jobs.size(); i++) {
		Job *job = new Job(
			typesetter, 
			inner_width - padding,
			info.jobs[i]
		);
		size.height += job->height();
		jobs.push_back(job);
	}
}
	
ResumeTypesetter::JobsSection::~JobsSection() {
	for (size_t i=0; i<jobs.size(); i++) {
		delete jobs[i];	
	}
}

void ResumeTypesetter::JobsSection::draw(Point point) {
	Point cursor = point;
	Section::draw(cursor);
	cursor.y += title->height();
	cursor.x += padding;
	for (size_t i=0; i<jobs.size(); i++) {
		jobs[i]->draw(cursor);
		cursor.y += jobs[i]->height();
	}
}


ResumeTypesetter::Project::Project(
	ResumeTypesetter &typesetter,
	double max_width,
	ProjectInfo project_info
)
	:MaxWidthElement(typesetter, max_width),
	 project_info(project_info),
	 name(new UnwrappedText(cr, fonts["medium bold"], project_info.name.text)),
	 summary(new WrappedText(
		 cr, 
		 fonts["small"],
		 project_info.summary,
		 max_width - padding
	 )),
	 bullets(new BulletList(
		 typesetter, 
		 max_width - padding,
		 project_info.bullets
	 ))
{
	size = {max_width,
				 	name->height() + summary->height() + bullets->height()};
}

ResumeTypesetter::Project::~Project() {
	delete name;
	delete summary;
	delete bullets;
}

void ResumeTypesetter::Project::draw(Point point) {
	Point cursor = point;
	if (project_info.name.link.size()) {
		cairo_set_source_rgb(cr, 0, 0, 1);
		cairo_tag_begin(cr, CAIRO_TAG_LINK, ("uri='" + project_info.name.link + "'").c_str());
		name->draw(cursor);

		int underline_height = fonts["medium bold"]->size() / 5;
		double y_start = cursor.y;
		cursor.y += name->height() - underline_height;
		cairo_move_to(cr, cursor.x, cursor.y);
		cairo_line_to(cr, cursor.x + name->width(), cursor.y);
		cairo_stroke(cr);
		cursor.y = y_start;

		cairo_tag_end(cr, CAIRO_TAG_LINK);
		cairo_set_source_rgb(cr, 0, 0, 0);
	} else {
		name->draw(cursor);
	}
	
	cursor.y += name->height();
	cursor.x += padding;
	summary->draw(cursor);
	
	cursor.y += summary->height();
	bullets->draw(cursor);
};

ResumeTypesetter::ProjectsSection::ProjectsSection(
	ResumeTypesetter &typesetter
) 
	:Section(typesetter, "Projects")
{
	for (size_t i=0; i<info.projects.size(); i++) {
		Project *project = new Project(
			typesetter, 
			inner_width - padding,
			info.projects[i]
		);
		size.height += project->height();
		projects.push_back(project);
	}
}
	
ResumeTypesetter::ProjectsSection::~ProjectsSection() {
	for (size_t i=0; i<projects.size(); i++) {
		delete projects[i];	
	}
}

void ResumeTypesetter::ProjectsSection::draw(Point point) {
	Point cursor = point;
	Section::draw(cursor);
	cursor.y += title->height();
	cursor.x += padding;
	for (size_t i=0; i<projects.size(); i++) {
		projects[i]->draw(cursor);
		cursor.y += projects[i]->height();
	}
}


ResumeTypesetter::SkillsSection::SkillsSection(ResumeTypesetter &typesetter)
	:Section(typesetter, "Skills")
{
	skills = {};
	
	for (size_t i=0; i<info.skill_categories.size(); i++) {
		skills.push_back(new SkillsTexts);
		
		skills[i]->category = new UnwrappedText(
			cr,
			fonts["small bold"],
			info.skill_categories[i].name + ": "
		);
		
		string comma_separated_skills = "";
		
		for (size_t j=0; j<info.skill_categories[i].skills.size(); j++) {
			
			comma_separated_skills += info.skill_categories[i].skills[j];
			
			if (!(j == info.skill_categories[i].skills.size() - 1))
				comma_separated_skills += ", ";
		}
		
		skills[i]->skills_list = new WrappedText(
			cr,
			fonts["small"],
			comma_separated_skills,
			inner_width - padding - skills[i]->category->width());
		
		size.height += skills[i]->skills_list->height();
	}
}

ResumeTypesetter::SkillsSection::~SkillsSection() {
	for (size_t i=0; i<skills.size(); i++) {
		delete skills[i]->category;
		delete skills[i]->skills_list;
		delete skills[i];
	}
}

void ResumeTypesetter::SkillsSection::draw(Point point) {
	Point cursor = point;
	Section::draw(cursor);
	cursor.y += title->height();
	for (size_t i=0; i<skills.size(); i++) {
		cursor.x = point.x + padding;
		skills[i]->category->draw(cursor);
		cursor.x += skills[i]->category->width();
		skills[i]->skills_list->draw(cursor);
		cursor.y += skills[i]->skills_list->height();
	}
}