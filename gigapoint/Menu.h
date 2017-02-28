#ifndef _MENU_H_
#define _MENU_H_

#include "Utils.h"
#include <sstream>

using std::string;
using std::vector;

namespace gigapoint {

#define ITEM_NUMBER 0
#define ITEM_STRING 1

struct MenuItem {
	string name;
	int type;
	float value;
	vector<string> options;
	float minvalue, maxvalue, stepinc;

	MenuItem(): name("noname"), type(ITEM_NUMBER), value(0), minvalue(-1), maxvalue(-1), stepinc(1) {
		options.clear();
	}
	
	string getString() {
		string val = name;
		val.append(": "); 
		if(type == ITEM_NUMBER) {	
			std::stringstream ss;
			ss << value;
			val.append(ss.str());
		}
		else {
			val.append(options[(int)value]);
		}
		return val;
	}

	void up() {
		if(type == ITEM_NUMBER) {
			value += stepinc;
			if(value > maxvalue)
				value = minvalue;
		}
		else {
			value++;
			if(value > options.size() - 1)
				value = 0;
		}
	}

	void down() {
		if(type == ITEM_NUMBER) {
			value -= stepinc;
			if(value < minvalue)
				value = maxvalue;
		}
		else {
			value --;
			if(value < 0)
				value = options.size() - 1;
		}
	}
};

class PCMenu {
private:
	vector<MenuItem> menu;
	int item;
	bool needReloadShader;

protected:
	void initMenu(const Option* option);

public:
	PCMenu(const Option* option);
	~PCMenu();
	void updateMenu(const Option* option);
	string getString();
	void next();
	void prev();
	void up();
	void down();
	bool updateOption(Option* option); // return: false = no need to reload shader
	void setReloadShader(bool val) { needReloadShader = val; }
};

}; //namespace gigapoint

#endif
