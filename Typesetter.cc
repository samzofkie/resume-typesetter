#include "Typesetter.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <pango/pangocairo.h>
#include <cairo-pdf.h>
using namespace std;

Font::Font(int size, string font_str) 
	:description(pango_font_description_from_string(font_str.c_str()))
{
	pango_font_description_set_size(description, size * PANGO_SCALE);	
}

Font::~Font() {
	pango_font_description_free(description);
}

PangoFontDescription *Font::get_description() {
	return description;
}

Size Sized::get_size() { return size; }
double Sized::width() { return size.width; }
double Sized::height() { return size.height; }

DrawableText::DrawableText(cairo_t *cr, Font *font, string str) 
	:cr(cr), font(font), str(str) {}

UnwrappedText::UnwrappedText(cairo_t *cr, Font *font, string str)
	:DrawableText(cr, font, str), layout(pango_cairo_create_layout(cr))
{
	pango_layout_set_text(layout, str.c_str(), -1);
	pango_layout_set_font_description(layout, font->get_description());

	int _width, _height;
	pango_layout_get_size(layout, &_width, &_height);
	size.width = (double)_width / PANGO_SCALE;
	size.height = (double)_height / PANGO_SCALE;
}

UnwrappedText::~UnwrappedText() {
	g_object_unref(layout);
}

void UnwrappedText::draw(Point point) {
	cairo_move_to(cr, point.x, point.y);
	pango_cairo_show_layout(cr, layout);
}

string WrappedText::longest_substring_that_fits(string str) {
	size_t substr_length = 0, prev_length= 0;
	string substr = str.substr(0, substr_length);
	
	while (UnwrappedText(cr, font, substr).width() < max_width) {
		prev_length = substr_length;
		substr_length = str.find(' ', substr_length + 1);
		substr = str.substr(0, substr_length);
	};
	
	return str.substr(0, prev_length);
}

string WrappedText::rest_of_string(string str, size_t start) {
	return str.substr(start, str.size() - start);
}

WrappedText::WrappedText(cairo_t *cr, 
												 Font *font,
												 string str, 
												 double max_width, 
												 double line_spacing)
	:DrawableText(cr, font, str), max_width(max_width), line_spacing(line_spacing) 
{

	string _str = str, line;

	while (UnwrappedText(cr, font, _str).width() > max_width) {
		line = longest_substring_that_fits(_str);
		lines.push_back(new UnwrappedText(cr, font, line));
		_str = rest_of_string(_str, line.size() + 1);
	}
	lines.push_back(new UnwrappedText(cr, font, _str));

	size.width = lines.size() > 1 ? max_width : lines[0]->width();
	size.height = (lines.size() + line_spacing) * lines[0]->height();
}

WrappedText::~WrappedText() {
	for (long unsigned int i=0; i<lines.size(); i++)
		delete lines[i];
}

void WrappedText::draw(Point point) {
	Point cursor = point;
	for (long unsigned int i=0; i<lines.size(); i++) {
		lines[i]->draw(cursor);
		cursor.y += lines[i]->height() + line_spacing;
	}
}

Document::Document(string name, Size _size) 
	:name(name),
	 surface(cairo_pdf_surface_create(name.c_str(), _size.width, _size.height)),
	 cr(cairo_create(surface))
{
	size = _size;
	cairo_set_line_width(cr, 0.5);
}

Document::~Document() {
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}

Typesetter::Typesetter(Document *document) 
	:document(document), 
	 cr(document->cr) 
{}

ResumeTypesetter::ResumeTypesetter(Document *document, ResumeInfo info) 
	:Typesetter(document), info(info) 
{
	string main_font = "IBMPlexSans";
	string bold_font = main_font + " Bold";
	string italic_font = main_font + " Italic";

	int small = 10, medium = 14, large = 18;

	fonts["small"] = new Font(small, main_font);
	fonts["section"] = new Font(medium, bold_font);
	fonts["name"] = new Font(large, bold_font);
	fonts["small bold"] = new Font(small, bold_font);
	fonts["medium"] = new Font(medium, main_font);
	fonts["small italic"] = new Font(small, italic_font);

	margin = 25;
	padding = 10;
	inner_width = document->width() - (margin * 2);

	header = new Header(*this);
	education = new EducationSection(*this);
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

	WrappedText cute_note(cr, fonts["small italic"], "This resume was created with a typesetting program I wrote in C++.", inner_width);
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

		int underline_height = 2;
		
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
	vector<Bullet> bullets
)
	:Element(typesetter), 
	 bullet_spacing(10),
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
			inner_width - padding * 3 - bullet_spacing);
		
		size.height += bullet_texts[i]->bullet_text->height();
		
		bullet_texts[i]->subbullet_texts = {};
		
		for (size_t j=0; j<bullets[i].subbullets.size(); j++) {
			
			bullet_texts[i]->subbullet_texts.push_back(new WrappedText(
				cr,
				fonts["small"], 
				bullets[i].subbullets[j],
				typesetter.inner_width - padding * 3 - bullet_spacing * 3));
			
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
			cursor.x = point.x + bullet_spacing * 2;
			subbullet->draw(cursor);
			cursor.x += bullet_spacing;
			bullet_texts[i]->subbullet_texts[j]->draw(cursor);
			cursor.y += bullet_texts[i]->subbullet_texts[j]->height();
		}
	}
}

ResumeTypesetter::Project::Project(ResumeTypesetter &typesetter, 
																	 ProjectDescription project_description)
	:Element(typesetter),
	 name(new UnwrappedText(cr, fonts["medium"], project_description.name)),
	 summary(new WrappedText(
		 cr, 
		 fonts["small"],
		 project_description.summary,
		 inner_width - padding * 4
	 )),
	 bullets(new BulletList(typesetter, project_description.bullets))
{
	size = {inner_width - padding * 2,
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
		Project *project = new Project(typesetter, info.projects[i]);
		size.height += project->height();
		projects.push_back(project);
	}
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
			inner_width - padding);
		
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
