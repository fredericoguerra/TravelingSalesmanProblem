#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <cfloat>
#include <cmath>
#include <tuple>
#include <algorithm>
#include <iomanip>

using namespace std;

int dimension; //Number of locations
double ** distance_matrix; //Matrix representing the distance between two cities.

//insertiong structure to help managing the nodes insertion into the current solution s' route.
struct insertion{
  int node; //evaluated node to be inserted into s'
  tuple<int,int> deleted_edge; //index of the removed edge when inserting the new node
  double cost; //cost of the insertion movement
};

//solution structure
struct solution{
  vector<int> route; //sequence of nodes starting from the first node, visiting all the remaining and returning to the start.
  double cost; //cost of the route
};

struct less_than_cost{
  inline bool operator() (const insertion& insertion1, const insertion& insertion2){
    return(insertion1.cost < insertion2.cost);
  }
};

void printData() {
  cout << "dimension: " << dimension << endl;
  for (size_t i = 1; i <= dimension; i++) {
    for (size_t j = 1; j <= dimension; j++) {
      cout << distance_matrix[i][j] << " ";
    }
    cout << endl;
  }
}

vector<insertion> generate_insertion_cost(solution& s_, vector<int>&CL){
  //generates a full list of edges between s' nodes and the nodes from candidates list
  vector<insertion> insertion_cost((s_.route.size()-1) * CL.size());
  int i=0, count =0;

  for(int r0 = 0, r1 = 1; count < s_.route.size() - 1; r0++, r1++){
    cout << s_.route[r0] << "->" << s_.route[r1] << endl;
    int origin = s_.route[r0];
    int next_neighbour = s_.route[r1];

    for(auto r2 : CL){
      insertion_cost[i].node = r2;
      insertion_cost[i].deleted_edge = make_tuple(origin, next_neighbour);
      insertion_cost[i].cost = distance_matrix[origin][r2] + distance_matrix[r2][next_neighbour] - distance_matrix[origin][next_neighbour];
      cout << "From: " << origin << " -> " << next_neighbour << " To: " << origin << " -> " << r2 << " -> " << next_neighbour << " | Cost: " <<  insertion_cost[i].cost << " Deleted Edge: " << get<0>(insertion_cost[i].deleted_edge) << "," << get<1>(insertion_cost[i].deleted_edge) << endl;
      i++;
    }
    count++;
  }

  return insertion_cost;
}

solution construction(vector<int>& CL) {
  
  int random_index, count, i;
  solution s_;
  
  //Assign the initial cost to zero and add the node 1 as the start and end location in the route
  s_.cost = 0;
  s_.route.insert(s_.route.end(), {1,1});

  // 
  for(count = 1; count <= 3; count++){
    
    /*for(i=0; i<CL.size();i++){
      cout << CL[i] << ' ';
    }*/
    
    random_index = rand() % CL.size();
    s_.route.insert(s_.route.end() - 1, CL[random_index]);
    CL.erase(CL.begin() + random_index);
    
    /*for(i=0; i< s_.route.size();i++){
      cout << s_.route[i] << ' ';
    }*/
  };

  //computes the cost of the initial route after randomly adding the 3 nodes in the route.
  for(count = 1; count <= 4; count++){
    //cout << s_.cost << endl;
    //cout << s_.route[count-1] << '-' << s_.route[count] << endl;
    s_.cost += distance_matrix[s_.route[count-1]][s_.route[count]];
    //cout << s_.cost << endl;
  };

  while(!CL.empty()){
  //for(i=1;i<2;i++){
    //creates the list of candidates from CL that can be inserted in s' computing its insertion cost 
    vector<insertion> insertion_cost = generate_insertion_cost(s_, CL);

    sort(insertion_cost.begin(), insertion_cost.end(), less_than_cost());

    double r = (double) rand()/ RAND_MAX;
    int chosen_node = rand() % ((int) ceil(r * insertion_cost.size()));

    s_.route.insert(s_.route.begin() + get<1>(insertion_cost[chosen_node].deleted_edge), insertion_cost[chosen_node].node);

    //It can be done better
    int aux = 0;
    while(true){
      if(insertion_cost[chosen_node].node == CL[aux]){
        CL.erase(CL.begin()+aux);
        break;
      }
      aux++;
    }

    //new solution cost
    s_.cost += insertion_cost[chosen_node].cost;
  }
  
  return s_;
}

int main(int argc, char** argv) {

  //printData();

  vector<int> CL;
  int i;
  solution s_;

  //read the instance through the auxiliary function ReadData.h based on the user input parameters
  readData(argc, argv, &dimension, &distance_matrix);

  for(i=2; i<=dimension; i++){
    CL.push_back(i);
  }

  s_ = construction(CL);

  cout << "cost:" << s_.cost << endl << "route:" << endl;
  for(i=0;i<s_.route.size();i++){
    cout << s_.route[i] << ' ';
  }

  return 0;
}