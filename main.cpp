#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <random>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <variant>
#include <set>
#include <cmath>
#include <chrono>

using namespace std;

// According to the key, I must use Small 119 and Big 60

struct Dataset {
    vector<vector<double>> features;
    vector<int> classes;
    int numInstances;
    int numFeatures;
};


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

    // Dataset data = loadDataset(userFile);

    // cout << "This dataset has " << data.numFeatures << " features (not including the class attribute), with " << data.numInstances << " instances." << endl << endl;

    if (userChoice == 1) {
        // forwardSelection(data);
    }
    else if (userChoice == 2) {
        // backwardElimination(data);
    }

    return 0;
}