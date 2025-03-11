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

Dataset loadDataset(const string &userFile);

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

    Dataset data = loadDataset(userFile);

    // cout << "This dataset has " << data.numFeatures << " features (not including the class attribute), with " << data.numInstances << " instances." << endl << endl;

    if (userChoice == 1) {
        // forwardSelection(data);
    }
    else if (userChoice == 2) {
        // backwardElimination(data);
    }

    return 0;
}

Dataset loadDataset(const string &userFile) {
    Dataset data;
    ifstream file(userFile);

    if (!file.is_open()) {
        cout << "Failed to open file: " << userFile << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        double value;
        vector<double> instance;

        iss >> value;
        data.classes.push_back(static_cast<int>(value));

        while (iss >> value) {
            instance.push_back(value);
        }

        data.features.push_back(instance);
    }

    data.numInstances = data.features.size();
    if (data.numInstances > 0) {
        data.numFeatures = data.features[0].size();
    }
    else {
        data.numFeatures = 0;
    }

    file.close();
    return data;
}