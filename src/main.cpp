#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <cfloat>
#include <algorithm>
#include <iomanip>

using namespace std;

int dimension; //Number of locations
double ** distanceMatrix; //Matrix representing the distance between two cities.
int nearestRoute;
int nearestNeighbor;

void printData() {
  cout << "dimension: " << dimension << endl << endl;
  cout << "Distance Matrix:" << endl;
  for (size_t i = 1; i <= dimension; i++) {
    for (size_t j = 1; j <= dimension; j++) {
      cout << distanceMatrix[i][j] << " ";
    }
    cout << endl;
  }
}

struct initialSolution {
  vector<int> route;
  int distance;
};

initialSolution genInitialSolution(){
  vector<int> initialRoute(dimension);
  int routeSize=0;

  for (int i = 0; i < dimension; i++){
    initialRoute[i] = i;
  }

  for (int l1 = 1; l1 < dimension-1; l1++){
    nearestRoute = distanceMatrix[initialRoute[l1-1]+1][initialRoute[l1]+1];
    nearestNeighbor = initialRoute[l1];
    //cout << endl << endl << "L1: " << nearestNeighbor << " Distance from " << initialRoute[l1-1] << ": "<< nearestRoute << endl;

    vector<int> notVisitedNeighbors = {initialRoute.begin() + l1, initialRoute.end()};

  //for (int n = 1; n < notVisitedNeighbors.size(); n++){
    //cout << notVisitedNeighbors[n] << " ";
  //}

    for (int l2 = 1; l2 < notVisitedNeighbors.size(); l2++){
      //cout << "L2: " << notVisitedNeighbors[l2] << " | " << "Distance from " << initialRoute[l1-1] << ": " << distanceMatrix[initialRoute[l1-1]+1][notVisitedNeighbors[l2]+1] << "  ";
      if((initialRoute[l1-1] != notVisitedNeighbors[l2]) && (distanceMatrix[initialRoute[l1-1]+1][notVisitedNeighbors[l2]+1] < nearestRoute)){
        nearestRoute = distanceMatrix[initialRoute[l1-1]+1][notVisitedNeighbors[l2]+1];
        nearestNeighbor = notVisitedNeighbors[l2];
        }
      }
    if (nearestNeighbor != l1){
      //cout << "Neighbor: " << nearestNeighbor << "Distance: " << nearestRoute << endl;
      //cout << endl <<"Before swap:" << endl;
      //for (int i = 0; i < dimension; i++){
      //  cout << initialRoute[i] << " ";
      //}
      int index = find(notVisitedNeighbors.begin(), notVisitedNeighbors.end(), nearestNeighbor) - notVisitedNeighbors.begin();
      index += l1;

      swap(initialRoute[l1], initialRoute[index]);
      //cout << endl <<"Swaped:" << endl;
      for (int k = 0; k < dimension; k++){
        //cout << initialRoute[k] << " ";
      }
    }
  }

  initialRoute.push_back(0);

  for (int i=1; i < dimension +1; i++){
    //cout
    cout << distanceMatrix[initialRoute[i-1]+1][initialRoute[i]+1] << " ";
    routeSize += distanceMatrix[initialRoute[i-1]+1][initialRoute[i]+1];
  }

  return initialSolution{initialRoute, routeSize};
}

int main(int argc, char** argv){
  readData(argc, argv, &dimension, &distanceMatrix);

  printData();
  //Generates Initial Solution
  initialSolution baselineSolution;
  baselineSolution = genInitialSolution();
  cout << endl << "InitialRoute: ";
  for (int i = 0; i < dimension+1; i++){
  baselineSolution.route[i] += 1;
  cout << baselineSolution.route[i] << " ";
  }
  cout << endl << "Distance: " << baselineSolution.distance << endl;

    return 0;
}

