#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

struct TheSetOfData {
    int countOfNumberInstances;
    int countOfNumberFeatures;
    vector<int> theDiffGroupsClasses;
    vector<vector<double> > thelistofdifferentFeautures;
};

TheSetOfData loadData(const string& thenameoftheInputFile) {
    TheSetOfData thadatastreamWeGetting;
    string thepartweReading;
    double thegroupClValue;
    double thegroupFeatValue;

    ifstream fileStreamOpen(thenameoftheInputFile);

    if (!fileStreamOpen.is_open()) {
        cerr << "Error: Could not open file " << thenameoftheInputFile << endl;
        exit(1);
    }

    while (getline(fileStreamOpen, thepartweReading)) {
        istringstream iss(thepartweReading);

        iss >> thegroupClValue;
        thadatastreamWeGetting.theDiffGroupsClasses.push_back(static_cast<int>(thegroupClValue));

        vector<double> listofAllInstFeaturesAvaialable;
        while (iss >> thegroupFeatValue) {
            listofAllInstFeaturesAvaialable.push_back(thegroupFeatValue);
        }

        thadatastreamWeGetting.thelistofdifferentFeautures.push_back(listofAllInstFeaturesAvaialable);
    }

    thadatastreamWeGetting.countOfNumberInstances = thadatastreamWeGetting.theDiffGroupsClasses.size();
    if (thadatastreamWeGetting.countOfNumberInstances > 0) {
        thadatastreamWeGetting.countOfNumberFeatures = thadatastreamWeGetting.thelistofdifferentFeautures[0].size();
    } else {
        thadatastreamWeGetting.countOfNumberFeatures = 0;
    }

    return thadatastreamWeGetting;
}

double calculateAccuracy(const TheSetOfData& theDataInputtoCalculate, const vector<bool>& theSelectedFeatChooseFromList) {
    int i;
    int j;
    int k;
    int RightIntToUse = 0;
    double nearestDistfromCurrLocation = INFINITY;
    int nearestClfromCurrLocation = -1;
    double distToCalculateandUse = 0;

    for ( i = 0; i < theDataInputtoCalculate.countOfNumberInstances; i++) {
        for ( j = 0; j < theDataInputtoCalculate.countOfNumberInstances; j++) {
            if (i == j) continue;

            for ( k = 0; k < theDataInputtoCalculate.countOfNumberFeatures; k++) {
                if (theSelectedFeatChooseFromList[k]) {
                    double subtractionResultUse = theDataInputtoCalculate.thelistofdifferentFeautures[i][k] - theDataInputtoCalculate.thelistofdifferentFeautures[j][k];
                    distToCalculateandUse += subtractionResultUse * subtractionResultUse;
                }
            }
            distToCalculateandUse = sqrt(distToCalculateandUse);

            if (distToCalculateandUse < nearestDistfromCurrLocation) {
                nearestDistfromCurrLocation = distToCalculateandUse;
                nearestClfromCurrLocation = theDataInputtoCalculate.theDiffGroupsClasses[j];
            }
        }

        if (nearestClfromCurrLocation == theDataInputtoCalculate.theDiffGroupsClasses[i]) RightIntToUse++;
    }

    return 100.0 * RightIntToUse / theDataInputtoCalculate.countOfNumberInstances;
}

void printFeatureSet(const vector<bool>& listOffeatTouse) {
    int i;
    cout << "{";
    bool ifBeginningThere = true;

    for ( i = 0; i < listOffeatTouse.size(); i++) {
        if (listOffeatTouse[i]) {
            if (!ifBeginningThere) cout << ",";
            cout << (i + 1);
            ifBeginningThere = false;
        }
    }
    cout << "}";
}

void forwardSelection(const TheSetOfData& DataWeUsingList) {
    int i;
    int j;
    vector<bool> ListweOnPresent(DataWeUsingList.countOfNumberFeatures, false);
    vector<bool> theMostSuitedvalues = ListweOnPresent;
    double bestthingSoFarinList = 0;
    int thesystemweAddIntoIt = -1;
    double bestSystemOnPresent = 0;

    cout << "\nBeginning Forward Selection search.\n" << endl;

    for ( i = 0; i < DataWeUsingList.countOfNumberFeatures; i++) {

        for ( j = 0; j < DataWeUsingList.countOfNumberFeatures; j++) {
            if (!ListweOnPresent[j]) {
                ListweOnPresent[j] = true;

                cout << "Using feature(s) ";
                printFeatureSet(ListweOnPresent);

                double theValueofAccGood = calculateAccuracy(DataWeUsingList, ListweOnPresent);
                cout << " accuracy is " << fixed << setprecision(1) << theValueofAccGood << "%" << endl;

                if (theValueofAccGood > bestSystemOnPresent) {
                    bestSystemOnPresent = theValueofAccGood;
                    thesystemweAddIntoIt = j;
                }

                ListweOnPresent[j] = false;
            }
        }

        if (thesystemweAddIntoIt != -1) {
            ListweOnPresent[thesystemweAddIntoIt] = true;

            cout << "\nFeature set ";
            printFeatureSet(ListweOnPresent);
            cout << " was best, accuracy is " << fixed << setprecision(1) << bestSystemOnPresent << "%" << endl;

            if (bestSystemOnPresent > bestthingSoFarinList) {
                bestthingSoFarinList = bestSystemOnPresent;
                theMostSuitedvalues = ListweOnPresent;
            } else {
                cout << "(Warning, accuracy decreased! Continuing search in case of local maxima)" << endl;
            }
        }
    }

    cout << "\nFinished search!! The best feature subset is ";
    printFeatureSet(theMostSuitedvalues);
    cout << ", which has an accuracy of " << fixed << setprecision(1) << bestthingSoFarinList << "%" << endl;
}

void backwardElimination(const TheSetOfData& DataWeUsingList) {
    int i;
    int j;
    vector<bool> ListweOnPresent(DataWeUsingList.countOfNumberFeatures, true);
    vector<bool> theMostSuitedvalues = ListweOnPresent;
    double bestthingSoFarinList = calculateAccuracy(DataWeUsingList, ListweOnPresent);

    cout << "\nBeginning Backward Elimination search.\n" << endl;
    cout << "Using all features, accuracy is " << fixed << setprecision(1) << bestthingSoFarinList << "%" << endl;

    for ( i = 0; i < DataWeUsingList.countOfNumberFeatures; i++) {
        int theFeatWeMustRemoveToAdvance = -1;
        double bestSystemOnPresent = 0;

        for ( j = 0; j < DataWeUsingList.countOfNumberFeatures; j++) {
            if (ListweOnPresent[j]) {
                ListweOnPresent[j] = false;

                cout << "Using feature(s) ";
                printFeatureSet(ListweOnPresent);

                double theValueofAccGood = calculateAccuracy(DataWeUsingList, ListweOnPresent);
                cout << " accuracy is " << fixed << setprecision(1) << theValueofAccGood << "%" << endl;

                if (theValueofAccGood > bestSystemOnPresent) {
                    bestSystemOnPresent = theValueofAccGood;
                    theFeatWeMustRemoveToAdvance = j;
                }

                ListweOnPresent[j] = true;
            }
        }

        if (theFeatWeMustRemoveToAdvance != -1) {
            ListweOnPresent[theFeatWeMustRemoveToAdvance] = false;

            cout << "\nFeature set ";
            printFeatureSet(ListweOnPresent);
            cout << " was best, accuracy is " << fixed << setprecision(1) << bestSystemOnPresent << "%" << endl;

            if (bestSystemOnPresent > bestthingSoFarinList) {
                bestthingSoFarinList = bestSystemOnPresent;
                theMostSuitedvalues = ListweOnPresent;
            } else {
                cout << "(Warning, accuracy decreased! Continuing search in case of local maxima)" << endl;
            }
        }
    }

    cout << "\nFinished search!! The best feature subset is ";
    printFeatureSet(theMostSuitedvalues);
    cout << ", which has an accuracy of " << fixed << setprecision(1) << bestthingSoFarinList << "%" << endl;
}

int main() {
    string nameOfInputFileToPut;
    int theusersChoiceForFile;

    cout << "Welcome to Feature Selection with Nearest Neighbor Algorithm." << endl;
    cout << "Type in the name of the file to test: ";
    cin >> nameOfInputFileToPut;

    TheSetOfData theDataSetWeShallLoadAndUse = loadData(nameOfInputFileToPut);
    cout << "This dataset has " << theDataSetWeShallLoadAndUse.countOfNumberFeatures << " features (not including the class attribute), with "
         << theDataSetWeShallLoadAndUse.countOfNumberInstances << " instances." << endl;

    vector<bool> theListOfAllPossibleFeat(theDataSetWeShallLoadAndUse.countOfNumberFeatures, true);
    double theAccuracythatwecanGetTheLowestOf = calculateAccuracy(theDataSetWeShallLoadAndUse, theListOfAllPossibleFeat);

    cout << "\nRunning nearest neighbor with all " << theDataSetWeShallLoadAndUse.countOfNumberFeatures
         << " features, using \"leaving-one-out\" evaluation, I get an accuracy of "
         << fixed << setprecision(1) << theAccuracythatwecanGetTheLowestOf << "%" << endl;

    cout << "\nType the number of the algorithm you want to run." << endl;
    cout << "  1) Forward Selection" << endl;
    cout << "  2) Backward Elimination" << endl;
    cout << "Algorithm choice: ";
    cin >> theusersChoiceForFile;

    if (theusersChoiceForFile == 1) forwardSelection(theDataSetWeShallLoadAndUse);
    else if (theusersChoiceForFile == 2) backwardElimination(theDataSetWeShallLoadAndUse);
    else cout << "Invalid choice." << endl;

    return 0;
}
