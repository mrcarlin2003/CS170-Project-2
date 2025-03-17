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

    vector<int> totalFeatures(data.numFeatures);
    
    for (int i = 0; i < data.numFeatures; ++i) {
        totalFeatures.push_back(i);
    }

    double initialAccuracy = leaveOneOutCrossValidation(data, totalFeatures);

    cout << "This dataset has " << data.numFeatures << " features (not including the class attribute), with " << data.numInstances << " instances." << endl;
    cout << "Running nearest neighbor with all " << data.numFeatures << " features, using \"leaving-one-out\" evaluation, I get an accuracy of " << initialAccuracy * 100 << "%" << endl << endl;

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

    for (size_t i = 0; i < a.size(); ++i) {
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

    cout << "Beginning search." << endl << endl;

    for (int level = 0; level < data.numFeatures; ++level) {
        int bestFeature = -1;
        double bestAccuracy = 0.0;

        for (int feature : availableFeatures) {
            vector<int> tempFeatures = selectedFeatures;
            tempFeatures.push_back(feature);

            double accuracy = leaveOneOutCrossValidation(data, tempFeatures);

            cout << "Using feature(s) {";

            for (size_t i = 0; i < tempFeatures.size(); ++i) {
                cout << tempFeatures[i] + 1;
                if (i < tempFeatures.size() - 1) {
                    cout << ",";
                }
            }

            cout << "} accuracy is " << accuracy * 100 << "%" << endl;

            if (accuracy > bestAccuracy) {
                bestAccuracy = accuracy;
                bestFeature = feature;
            }
        }

        if (bestFeature != -1) {
            selectedFeatures.push_back(bestFeature);
            availableFeatures.erase(remove(availableFeatures.begin(), availableFeatures.end(), bestFeature), availableFeatures.end());

            cout << endl << "Feature set {";

            for (size_t i = 0; i < selectedFeatures.size(); ++i) {
                cout << selectedFeatures[i] + 1;
                if (i < selectedFeatures.size() - 1) {
                    cout << ",";
                }
            }

            cout << "} was best, accuracy is " << bestAccuracy * 100 << "%" << endl << endl;

            if (bestAccuracy > bestTotalAccuracy) {
                bestTotalAccuracy = bestAccuracy;
                bestFeatures = selectedFeatures;
            }
            else {
                cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)" << endl;
            }
        }
        else {
            break;
        }
    }

    cout << endl << "Finished search!!! The best Feature subset is {";
    for (size_t i = 0; i < bestFeatures.size(); ++i) {
        cout << bestFeatures[i] + 1;
        if (i < bestFeatures.size() - 1) {
            cout << ",";
        }
    }
    cout << "}, which has an accuracy of " << bestTotalAccuracy * 100 << "%" << endl;
}

void backwardElimination(const Dataset &data) {
    vector<int> selectedFeatures(data.numFeatures);
    
    for (int i = 0; i < data.numFeatures; ++i) {
        selectedFeatures[i] = i;
    }

    vector<int> bestFeatures = selectedFeatures;
    double bestTotalAccuracy = leaveOneOutCrossValidation(data, selectedFeatures);

    cout << "Beginning search." << endl << endl;

    cout << "Using feature(s) {";

    for (size_t i = 0; i < selectedFeatures.size(); ++i) {
        cout << selectedFeatures[i] + 1;
        if (i < selectedFeatures.size() - 1) {
            cout << ",";
        }
    }

    cout << "} accuracy is " << bestTotalAccuracy * 100 << "%" << endl;

    for (int level = data.numFeatures; level > 1; --level) {
        int worstFeature = -1;
        double bestAccuracy = 0.0;
        vector<int> tempBestFeatures = selectedFeatures;

        for (int feature : selectedFeatures) {
            vector<int> tempFeatures = selectedFeatures;
            tempFeatures.erase(remove(tempFeatures.begin(), tempFeatures.end(), feature), tempFeatures.end());

            double accuracy = leaveOneOutCrossValidation(data, tempFeatures);

            cout << "Using feature(s) {";

            for (size_t i = 0; i < tempFeatures.size(); ++i) {
                cout << tempFeatures[i] + 1;
                if (i != tempFeatures.size() - 1) {
                    cout << ",";
                }
            }

            cout << "} accuracy is " << accuracy * 100 << "%" << endl;

            if (accuracy > bestAccuracy) {
                bestAccuracy = accuracy;
                worstFeature = feature;
                tempBestFeatures = tempFeatures;
            }
        }

        if (worstFeature != -1) {
            selectedFeatures.erase(remove(selectedFeatures.begin(), selectedFeatures.end(), worstFeature), selectedFeatures.end());

            cout << endl << "Feature set {";

            for (size_t i = 0; i < selectedFeatures.size(); ++i) {
                cout << selectedFeatures[i] + 1;
                if (i != selectedFeatures.size() - 1) {
                    cout << ",";
                }
            }

            cout << "} was best, accuracy is " << bestAccuracy * 100 << "%" << endl << endl;

            if (bestAccuracy > bestTotalAccuracy) {
                bestTotalAccuracy = bestAccuracy;
                bestFeatures = selectedFeatures;
            }
            else {
                cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)" << endl;
            }
        }
        else {
            break;
        }
    }

    cout << endl << "Finished search!!! The best feature subset is {";
    for (size_t i = 0; i < bestFeatures.size(); ++i) {
        cout << bestFeatures[i] + 1;
        if (i != bestFeatures.size() - 1) {
            cout << ",";
        }
    }
    cout << "}, which has an accuracy of " << bestTotalAccuracy * 100 << "%" << endl;
}