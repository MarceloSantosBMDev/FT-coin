#include "Controller.h"
#include "Colors.h"

#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <cstdlib>

using namespace std;

void chooseDatabase(unique_ptr<Controller>& controller) {
    cout << YELLOW << "Select the database mode:\n" << RESET;
    cout << GREEN << BOLD << "1." << RESET << " Memory\n" << RESET;
    cout << GREEN << BOLD << "2." << RESET << " Database (MariaDB)\n\n" << RESET;
    cout << YELLOW << "Option: " << RESET;

    int option;
    while (!(cin >> option) || (option != 1 && option != 2)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << RED << "Invalid option! Enter 1 or 2: " << RESET;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    DataBaseSelector dbType = (option == 1) ? DataBaseSelector::MEMORY : DataBaseSelector::DATABASE;
    controller = make_unique<Controller>(dbType);
}

int main() {
    system("clear");
    
    try {
        unique_ptr<Controller> mainController;
        chooseDatabase(mainController);
        mainController->start();
    } catch (const exception& e) {
        cerr << RED << "\nError: " << e.what() << RESET << endl;
        return 1;
    }

    return 0;
}