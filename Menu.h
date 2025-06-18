#ifndef MENU_H_
#define MENU_H_

#include <string>
#include <vector>
#include <iostream>
#include <limits>
#include "cores.h"

using namespace std;

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