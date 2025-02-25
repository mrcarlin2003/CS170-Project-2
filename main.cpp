#include <iostream>
#include <string>

using namespace std;

// According to the key, I must use Small 119 and Big 60

int main() {
    string userFile = "";
    int userChoice = 0;

    cout << "Welcome to Michael Carlin's Feature Selection Algorithm." << endl;
    cout << "Type in the name of the file to test: ";

    cin >> userFile;

    cout << endl;

    cout << "Type the number of the algorithm you want to run." << endl << endl;

    cout << "1) Forward Selection" << endl;
    cout << "2) Backward Elimination" << endl;

    cin >> userChoice;
    cout << endl;

    cout << "This dataset has [blank] features (not including the class attribute), with [blank] instances." << endl;


    return 0;
}