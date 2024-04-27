#include "resume-typesetter.h"
#include <string>
#include <vector>
#include <map>
#include "typesetter.h"
#include "document.h"
#include "drawable.h"
#include "resume-info.h"
#include "text.h"
#include "point.h"
#include <pango/pangocairo.h>
using namespace std;


/* The nested classes of ResumeTypesetter are all passed a reference to the 
	outside class, so they can read it's properties such as it's font map, and 
	the margin, padding, and inner_width values. */
ResumeTypesetter::ResumeTypesetter(Document &document, ResumeInfo info) 
	:Typesetter(document), info(info) 
{
	string main_font = "IBMPlexSans";
	string bold_font = main_font + " Bold";
	string italic_font = main_font + " Italic";

	int small = 8, medium = 10, large = 18;

	fonts["small"] = new Font(small, main_font);
	fonts["medium"] = new Font(medium, main_font);
	fonts["large"] = new Font(large, main_font);
	fonts["section"] = new Font(medium, bold_font);
	fonts["name"] = new Font(large, bold_font);
	fonts["small bold"] = new Font(small, bold_font);
	fonts["small italic"] = new Font(small, italic_font);

	margin = 25;
	padding = 10;
	inner_width = document.width() - (margin * 2);

	header = new Header(*this);
	education = new EducationSection(*this);
	//work_experience = new ExperienceSection(*this);
	experience = new ExperienceSection(*this);
	skills = new SkillsSection(*this);
}

ResumeTypesetter::~ResumeTypesetter() {
	for(map<string,Font*>::iterator i = fonts.begin(); i!=fonts.end(); i++)
		delete i->second;
	delete header;
	delete education;
	delete experience;
	delete skills;
}

void ResumeTypesetter::write() {
	Point cursor {margin, margin};
	header->draw(cursor);
	cursor.y += header->height() + padding;
	education->draw(cursor);
	cursor.y += education->height();
	experience->draw(cursor);
	cursor.y += experience->height();
	skills->draw(cursor);
	cursor.y += skills->height() + padding;

	WrappedText cute_note(cr, fonts["small italic"], "This resume was typeset with a program I wrote in C++", inner_width);
	cursor.x = margin + (inner_width - cute_note.width()) / 2;
	cute_note.draw(cursor);
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
		UnwrappedText *p = new UnwrappedText(cr, fonts["small"], info.links[i]);
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
		links[i]->draw(cursor);

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
	 school(new UnwrappedText(cr, fonts["small bold"], info.school)),
   degree(new UnwrappedText(cr, fonts["small"], ", " + info.degree)),
	 date(new UnwrappedText(cr, fonts["small"], info.school_date))
{
	size.height += school->height() + padding;
}

ResumeTypesetter::EducationSection::~EducationSection() {
	delete school;
	delete degree;
	delete date;
}

void ResumeTypesetter::EducationSection::draw(Point point) {
	Point cursor = point;
	Section::draw(cursor);
	cursor.y += title->height() + padding / 2;
	cursor.x += padding / 2;
	school->draw(cursor);
	cursor.x += school->width();
	degree->draw(cursor);
	cursor.x = point.x + size.width - date->width() - padding / 2;
	date->draw(cursor);
}

ResumeTypesetter::BulletList::BulletList(
	ResumeTypesetter &typesetter,
	double max_width,
	vector<Bullet> bullets
)
	:MaxWidthElement(typesetter, max_width), 
	 bullet_spacing(15),
	 bullet(new UnwrappedText(cr, fonts["small"], "•")),
	 subbullet(new UnwrappedText(cr, fonts["small"], "○"))
{
	size.height = 0;
	
	for (size_t i=0; i<bullets.size(); i++) {
		bullet_texts.push_back(new BulletText);
		
		bullet_texts[i]->bullet_text = new WrappedText(
			cr,
			fonts["small"], 
			bullets[i].text,
			max_width - bullet_spacing);
		
		size.height += bullet_texts[i]->bullet_text->height();
		
		bullet_texts[i]->subbullet_texts = {};
		
		for (size_t j=0; j<bullets[i].subbullets.size(); j++) {
			
			bullet_texts[i]->subbullet_texts.push_back(new WrappedText(
				cr,
				fonts["small"], 
				bullets[i].subbullets[j],
				max_width - bullet_spacing * 2)
			);
			
			size.height += bullet_texts[i]->subbullet_texts[j]->height();
		}
	}
	size.width = inner_width - padding * 3;
}

ResumeTypesetter::BulletList::~BulletList() {
	delete bullet;
	delete subbullet;
	for (size_t i=0; i<bullet_texts.size(); i++) {
		delete bullet_texts[i]->bullet_text;
		for (size_t j=0; j<bullet_texts[i]->subbullet_texts.size(); j++) {
			delete bullet_texts[i]->subbullet_texts[j];
		}
		delete bullet_texts[i];
	}
}

void ResumeTypesetter::BulletList::draw(Point point) {
	Point cursor = point;
  for (size_t i=0; i<bullet_texts.size(); i++) {
		cursor.x = point.x;
		bullet->draw(cursor);
		
		cursor.x += bullet_spacing;
		bullet_texts[i]->bullet_text->draw(cursor);
		
		cursor.y += bullet_texts[i]->bullet_text->height();
		for (size_t j=0; j<bullet_texts[i]->subbullet_texts.size(); j++) {
			cursor.x = point.x + bullet_spacing;
			subbullet->draw(cursor);
			
			cursor.x += bullet_spacing;
			bullet_texts[i]->subbullet_texts[j]->draw(cursor);
			cursor.y += bullet_texts[i]->subbullet_texts[j]->height();
		}
	}
}

ResumeTypesetter::Project::Project(
	ResumeTypesetter &typesetter,
	double max_width,
	ProjectDescription project_description
)
	:MaxWidthElement(typesetter, max_width),
	 name(new UnwrappedText(cr, fonts["medium"], project_description.name)),
	 summary(new WrappedText(
		 cr, 
		 fonts["small"],
		 project_description.summary,
		 max_width - padding
	 )),
	 bullets(new BulletList(
		 typesetter, 
		 max_width - padding,
		 project_description.bullets
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
	name->draw(cursor);
	
	cursor.y += name->height();
	cursor.x += padding;
	summary->draw(cursor);
	
	cursor.y += summary->height();
	bullets->draw(cursor);
};

ResumeTypesetter::ExperienceSection::ExperienceSection(
	ResumeTypesetter &typesetter
) 
	:Section(typesetter, "Experience")
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
	size.height += padding;
}
	
ResumeTypesetter::ExperienceSection::~ExperienceSection() {
	for (size_t i=0; i<projects.size(); i++) {
		delete projects[i];	
	}
}

void ResumeTypesetter::ExperienceSection::draw(Point point) {
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