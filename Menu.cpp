#include "Menu.h"
#include <iostream>
#include <limits>
#include <iomanip>

using namespace std;

Menu::Menu() {
}

Menu::~Menu() {
}

void Menu::setTitle(const string& title) {
    this->title = title;
}

void Menu::addOption(const string& option) {
    this->options.push_back(option);
}

int Menu::getOption() const {
    while (true) {
        cout << "\n" << CYAN << BOLD;
        cout << "╔════════════════════════════════════════════════════════════╗\n";
        cout << "║ " << left << setw(58) << title << " ║\n";
        cout << "╚════════════════════════════════════════════════════════════╝\n" << RESET;
        
        for (size_t i = 0; i < options.size(); i++) {
            cout << "   " << GREEN << BOLD << (i + 1) << ". " << RESET << options[i] << "\n";
        }
        cout << YELLOW << "\n   Option: " << RESET;

        int opt;
        while (!(cin >> opt) || opt < 1 || opt > static_cast<int>(options.size())) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "Invalid option! Enter a number between 1 and " << options.size() << ": " << RESET;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return opt;
    }
} 