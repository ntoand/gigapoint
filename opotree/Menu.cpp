#include "Menu.h"

using namespace std;

PCMenu::PCMenu(const Option* option): needReloadShader (false) {
	initMenu(option);
}

PCMenu::~PCMenu() {

}

void PCMenu::initMenu(const Option* option) {
	MenuItem item1;
	item1.name = "material";
	item1.type = ITEM_STRING;
	item1.options.push_back("rgb");
	item1.options.push_back("elevation");
	item1.value = option->material;
	menu.push_back(item1);

	MenuItem item2;
	item2.name = "quality";
	item2.type = ITEM_STRING;
	item2.options.push_back("square");
	item2.options.push_back("circle");
	item2.value = option->quality;
	menu.push_back(item2);

	MenuItem item3;
	item3.name = "sizetype";
	item3.type = ITEM_STRING;
	item3.options.push_back("fixed");
	item3.options.push_back("adaptive");
	item3.value = option->sizeType;
	menu.push_back(item3);

	MenuItem item4;
	item4.name = "pointscale";
	item4.minvalue = 0.05;
	item4.maxvalue = 1;
	item4.stepinc = 0.01;
	item4.value = option->pointScale;
	menu.push_back(item4);

	item = 0;
}

bool PCMenu::updateOption(Option* option) {
	option->material  = menu[0].value;
	option->quality = menu[1].value;
	option->sizeType = menu[2].value;
	option->pointScale = menu[3].value;
	if(needReloadShader) {
		needReloadShader = false;
		return true;
	}
	return false;
}

void PCMenu::updateMenu(const Option* option) {
	menu.clear();
	initMenu(option);
}

string PCMenu::getString() {
	string str = "< ";
	str.append(menu[item].getString());
	str.append(" >");
	return str;
}

void PCMenu::next() {
	item++;
	if(item > menu.size() -1) 
		item = 0;
}
	
void PCMenu::prev() {
	item --;
	if(item < 0)
		item = menu.size() - 1;
}
	
void PCMenu::up() {
	menu[item].up();
	if(item != 3)
		needReloadShader = true;
}
	
void PCMenu::down() {
	menu[item].down();
	if(item != 3)
		needReloadShader = true;
}
