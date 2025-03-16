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
    vector<vector<double>> features; // 2d vector
    vector<int> classes;
    int numInstances;
    int numFeatures;
};

Dataset loadDataset(const string &userFile);

double euclideanDistance(const vector<double> &a, const vector<double> &b);

int nearestNeighborClassifier(const Dataset &data, int testIndex, const vector<int> &selectedFeatures);

double leaveOneOutCrossValidation(const Dataset &data, const vector<int> &selectedFeatures);

void forwardSelection(const Dataset &data);

void backwardElimination(const Dataset &data);

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

    if (data.numInstances == 0 || data.numFeatures == 0) {
        cout << "Error: Dataset is empty or incorrectly formatted." << endl;
        return 1;
    }

    // cout << "This dataset has " << data.numFeatures << " features (not including the class attribute), with " << data.numInstances << " instances." << endl << endl;

    if (userChoice == 1) {
        forwardSelection(data);
    }
    else if (userChoice == 2) {
        backwardElimination(data);
    }

    return 0;
}

Dataset loadDataset(const string &userFile) { // read from file
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

double euclideanDistance(const vector<double> &a, const vector<double> &b) { // calculate the distance between two features
    double sum = 0.0;

    for (int i = 0; i < a.size(); ++i) {
        sum += pow(a[i] - b[i], 2);
    }

    return sqrt(sum);
}

int nearestNeighborClassifier(const Dataset &data, int testIndex, const vector<int> &selectedFeatures) { // returns the class of the nearest neighbor
    int nearestNeighbor = -1;
    double minDistance = INFINITY;

    for (int i = 0; i < data.numInstances; ++i) {
        if (i == testIndex) continue;

        vector<double> testFeatures, trainFeatures;
        for (int feature : selectedFeatures) {
            testFeatures.push_back(data.features[testIndex][feature]);
            trainFeatures.push_back(data.features[i][feature]);
        }

        double distance = euclideanDistance(testFeatures, trainFeatures);
        
        if (distance < minDistance) {
            minDistance = distance;
            nearestNeighbor = i;
        }
    }

    return data.classes[nearestNeighbor];
}

double leaveOneOutCrossValidation(const Dataset &data, const vector<int> &selectedFeatures) { // calculate accuracy
    int numCorrectClassifications = 0;

    for (int i = 0; i < data.numInstances; ++i) {
        int predictedClass = nearestNeighborClassifier(data, i, selectedFeatures);

        if (predictedClass == data.classes[i]) {
            ++numCorrectClassifications;
        }
    }

    return static_cast<double>(numCorrectClassifications) / data.numInstances; // (correct classifications / total instances)
}

void forwardSelection(const Dataset &data) {
    vector<int> selectedFeatures;
    vector<int> availableFeatures(data.numFeatures);
    vector<int> bestFeatures;
    
    for (int i = 0; i < data.numFeatures; ++i) {
        availableFeatures[i] = i;
    }

    double bestTotalAccuracy = 0.0;

    cout << "Runnning Forward Selection..." << endl;

    for (int level = 0; level < data.numFeatures; ++level) {
        int bestFeature = -1;
        double bestAccuracy = 0.0;

        for (int feature : availableFeatures) {
            vector<int> tempFeatures = selectedFeatures;
            tempFeatures.push_back(feature);

            double accuracy = leaveOneOutCrossValidation(data, tempFeatures);

            cout << "Considering adding feature " << feature + 1 << ", Accuracy: " << accuracy << endl;

            if (accuracy > bestAccuracy) {
                bestAccuracy = accuracy;
                bestFeature = feature;
            }
        }

        if (bestFeature != -1) {
            selectedFeatures.push_back(bestFeature);
            availableFeatures.erase(remove(availableFeatures.begin(), availableFeatures.end(), bestFeature), availableFeatures.end());

            cout << "On level " << level + 1 << " added feature " << bestFeature + 1 << "to the current set." << endl;

            if (bestAccuracy > bestTotalAccuracy) {
                bestTotalAccuracy = bestAccuracy;
                bestFeatures = selectedFeatures;
            }
        }
        else {
            break;
        }
    }

    cout << endl << "Best Feature Subset: ";
    for (int f: bestFeatures) {
        cout << f + 1 << " ";
    }
    cout << endl << "Final Accuracy: " << bestTotalAccuracy << endl;
}

void backwardElimination(const Dataset &data) {
    vector<int> selectedFeatures(data.numFeatures);
    
    for (int i = 0; i < data.numFeatures; ++i) {
        selectedFeatures[i] = i;
    }

    vector<int> bestFeatures = selectedFeatures;
    double bestTotalAccuracy = leaveOneOutCrossValidation(data, selectedFeatures);

    cout << "Runnning Backward Elimination..." << endl;

    for (int level = data.numFeatures; level > 1; --level) {
        int worstFeature = -1;
        double bestAccuracy = 0.0;

        for (int feature : selectedFeatures) {
            vector<int> tempFeatures = selectedFeatures;
            tempFeatures.erase(remove(tempFeatures.begin(), tempFeatures.end(), feature), tempFeatures.end());

            double accuracy = leaveOneOutCrossValidation(data, tempFeatures);

            cout << "Considering removing feature " << feature + 1 << ", Accuracy: " << accuracy << endl;

            if (accuracy > bestAccuracy) {
                bestAccuracy = accuracy;
                worstFeature = feature;
            }
        }

        if (worstFeature != -1) {
            selectedFeatures.erase(remove(selectedFeatures.begin(), selectedFeatures.end(), worstFeature), selectedFeatures.end());

            cout << "On level " << data.numFeatures - level + 1 << ", removed features " << worstFeature + 1 << "." << endl;

            if (bestAccuracy > bestTotalAccuracy) {
                bestTotalAccuracy = bestAccuracy;
                bestFeatures = selectedFeatures;
            }
        }
        else {
            break;
        }
    }

    cout << endl << "Best Feature Subset: ";
    for (int f: bestFeatures) {
        cout << f + 1 << " ";
    }
    cout << endl << "Final Accuracy: " << bestTotalAccuracy << endl;
}