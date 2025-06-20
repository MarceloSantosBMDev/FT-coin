#ifndef MENU_H_
#define MENU_H_

#include <string>
#include <vector>
#include "Colors.h"

using namespace std;

// A simple utility for creating and displaying interactive console menus.
class Menu {
private:
	string title;
	vector<string> options;

public:
	Menu();
	virtual ~Menu();
	void setTitle(const string& title);
	void addOption(const string& option);
	int getOption() const;
};

#endif /* MENU_H_ */ 