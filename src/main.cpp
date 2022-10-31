#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
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

template<typename T>
std::vector<T> slice(std::vector<T> const &v, int m, int n)
{
    auto first = v.cbegin() + m;
    auto last = v.cbegin() + n + 1;
 
    std::vector<T> vec(first, last);
    return vec;
}
 
void show_vector(vector<int> vec){
  int i;
  for(i=0; i< vec.size(); i++){
      cout << vec[i] << " ";
    }
  cout << endl;
}

vector<insertion> generate_insertion_cost(solution& s_, vector<int>&CL){
  //generates a full list of edges between s' nodes and the nodes from candidates list
  vector<insertion> insertion_cost((s_.route.size()-1) * CL.size());
  int i=0, count =0;

  for(int r0 = 0, r1 = 1; count < s_.route.size() - 1; r0++, r1++){
    //cout << s_.route[r0] << "->" << s_.route[r1] << endl;
    int origin = s_.route[r0];
    int next_neighbour = s_.route[r1];

    for(auto r2 : CL){
      insertion_cost[i].node = r2;
      insertion_cost[i].deleted_edge = make_tuple(r0, r1);
      insertion_cost[i].cost = distance_matrix[origin][r2] + distance_matrix[r2][next_neighbour] - distance_matrix[origin][next_neighbour];
      //cout << "From: " << origin << " -> " << next_neighbour << " To: " << origin << " -> " << r2 << " -> " << next_neighbour << " | Cost: " <<  insertion_cost[i].cost << " Deleted Edge: " << get<0>(insertion_cost[i].deleted_edge) << "," << get<1>(insertion_cost[i].deleted_edge) << endl;
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

    /*cout << endl << "AFTER SORTING" << endl;
    for(auto k: insertion_cost){
    cout << k.node << ":" << k.cost << " | ";
    };*/

    sort(insertion_cost.begin(), insertion_cost.end(), less_than_cost());

    /*cout << endl << "AFTER SORTING" << endl;
    for(auto k: insertion_cost){
    cout << k.node << ":" << k.cost << " | ";
    };*/

    double r = (double) rand()/ RAND_MAX;
    int chosen_node = rand() % ((int) ceil(r * insertion_cost.size()));

    //cout << get<1>(insertion_cost[chosen_node].deleted_edge) << endl;

    s_.route.insert(s_.route.begin() + (get<1>(insertion_cost[chosen_node].deleted_edge)), insertion_cost[chosen_node].node);

    //Could it be done better?
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

solution pertubation(solution s_){
  int bridge1_size, bridge2_size, i, j, max_i, min_j, max_j, j_range;
  vector<int> bridge1, bridge2;
  double new_route_cost = 0;

  if(s_.route.size()<30){
    bridge1_size = rand() % 2 + 1; // rand % range + min
    bridge2_size = rand() % 2 + 1; // rand % range + min
  }
  else{
    bridge1_size = rand() % s_.route.size()/10 + 1;
    bridge2_size = rand() % s_.route.size()/10 + 1;
  }
  //cout << "bridge1: " << bridge1_size << endl << "bridge2: " << bridge2_size << endl;
  max_i = s_.route.size() - 1 - bridge2_size - bridge1_size;
  //cout << "max_i: " << max_i << endl;
  i = rand() % max_i + 1;
  //cout << "i: " << i << endl;
  max_j = s_.route.size() - bridge2_size;
  //cout << "max_j: " << max_j << endl;

  min_j = i + bridge1_size;
  //cout << "min_j: " << min_j << endl;

  j_range = max_j - min_j;
  j = rand() % j_range + min_j;
  //cout << "j: " << j << endl << endl;

  bridge1 = slice(s_.route, i, i + bridge1_size-1);
  bridge2 = slice(s_.route, j, j + bridge2_size-1);
  //show_vector(s_.route);
  //show_vector(bridge1);
  s_.route.insert(s_.route.begin() + j, bridge1.begin(), bridge1.end());
  //show_vector(s_.route);
  s_.route.erase(s_.route.begin() + i, s_.route.begin() + i + bridge1_size);
  //show_vector(s_.route);
  //show_vector(bridge2);
  s_.route.insert(s_.route.begin() + i, bridge2.begin(), bridge2.end());
  //show_vector(s_.route);
  s_.route.erase(s_.route.begin() + j + bridge2_size, s_.route.begin() + j + 2*bridge2_size);
  for(i=1; i<s_.route.size(); i++){
    new_route_cost += distance_matrix[s_.route[i-1]][s_.route[i]];
  }
  s_.cost = new_route_cost;
  //cout << "new cost: " << s_.cost << endl;
  return s_;
}

bool assessment_cost(solution& s){
  int i;
  double true_cost=0;
  for(i=1;i<s.route.size();i++){
    true_cost += distance_matrix[s.route[i-1]][s.route[i]];
  }
  if(s.cost != true_cost){
    return true;
  }
  else{
    return false;
  }
}

bool best_improvement_swap(solution& s_){

  double best_delta = 0;
  double delta, partial_cost, aux_cost, a, b, c, d, a_, b_, c_, d_;
  int best_i, best_j;

  for(int i =1; i< s_.route.size() - 1; i++){
    
    a = distance_matrix[s_.route[i-1]][s_.route[i]]; // (i-1)->(i)
    b = distance_matrix[s_.route[i]][s_.route[i+1]]; // (i)->(i+1)

    partial_cost = -a -b;

    for(int j=i+1; j< s_.route.size() - 1; j++){

      d = distance_matrix[s_.route[j]][s_.route[j+1]]; // (j)->(j+1)
      a_ = distance_matrix[s_.route[i-1]][s_.route[j]]; // (i-1)->(j) -swap
      d_ = distance_matrix[s_.route[i]][s_.route[j+1]]; // (i)->(j+1) -swap

      aux_cost = -d + a_ + d_;

      if(i==j-1){
        b_ = distance_matrix[s_.route[j]][s_.route[i]]; // (j)->(i) -swap

        delta = partial_cost + aux_cost + b_;
      }
      else{
        c = distance_matrix[s_.route[i-1]][s_.route[j]]; // (j-1)->(j)
        b_ = distance_matrix[s_.route[j]][s_.route[i+1]]; // (j)->(i+1) -swap
        c_ = distance_matrix[s_.route[j-1]][s_.route[i]]; // (j-1)->(i) -swap

        delta = partial_cost + aux_cost -c + b_ + c_;
      }
    
      if(delta<best_delta){
        best_i = i;
        best_j = j;
        best_delta = delta;
      }
    }
  }
  
  if(best_delta<0){
    swap(s_.route[best_i], s_.route[best_j]);
    s_.cost += best_delta;
    //if(assessment_cost(s_)){
    //  cout << "Wrong 0 swap!" << endl;
    //  }
    return true;
  }
  return false;
}

bool best_improvement_2opt(solution& s_){
  double partial_cost, delta, best_delta = 0;
  int a, b, c, d, a_, b_, c_, d_, best_i, best_j;

  for(int i = 1; i< s_.route.size() - 3; i++){
    
    a = distance_matrix[s_.route[i-1]][s_.route[i]];
    partial_cost = - a;

    for(int j = i + 2; j< s_.route.size() - 1; j++){

      b = distance_matrix[s_.route[j]][s_.route[j+1]];
      a_ = distance_matrix[s_.route[i-1]][s_.route[j]];
      b_ = distance_matrix[s_.route[i]][s_.route[j+1]];
      
      delta = - a - b + a_ + b_;

      if(delta < best_delta){
        best_i = i;
        best_j = j;
        best_delta = delta;
      }
    }
  }
  if(best_delta < 0){
    int j = best_j;
    for(int i = best_i; i < j; i++){
      swap(s_.route[i], s_.route[j]);
      j--;
    }
    s_.cost += best_delta;
    //if(assessment_cost(s_)){
    //  cout << "###### Wrong 2 opt!" << endl;
  //}
    return true;
  }
  return false;
}

bool best_improvement_or_opt(solution& s_, int c){
  int i, j, best_i, best_j;
  double best_delta = 0, delta;

  switch(c){
    case 1:
    for(i=1; i< s_.route.size()-1; i++){
      for(j=1; j< s_.route.size()-1; j++){
        if(i+1==j){
          ;
        }
        else if(i<j){ 
          delta = - distance_matrix[s_.route[i]][s_.route[i+1]] - distance_matrix[s_.route[j-1]][s_.route[j]]
                  - distance_matrix[s_.route[j]][s_.route[j+1]] + distance_matrix[s_.route[i]][s_.route[j]]
                  + distance_matrix[s_.route[j]][s_.route[i+1]] + distance_matrix[s_.route[j-1]][s_.route[j+1]];
        }
        else if(i==j+1){
          delta = - distance_matrix[s_.route[i]][s_.route[i+1]] - distance_matrix[s_.route[j]][s_.route[i]]
                  - distance_matrix[s_.route[j-1]][s_.route[j]] + distance_matrix[s_.route[i]][s_.route[j]]
                  + distance_matrix[s_.route[j]][s_.route[i+1]] + distance_matrix[s_.route[j-1]][s_.route[i]];
        }
        else if(j<i){
          delta = - distance_matrix[s_.route[i]][s_.route[i+1]] - distance_matrix[s_.route[j]][s_.route[j+1]]
                  - distance_matrix[s_.route[j-1]][s_.route[j]] + distance_matrix[s_.route[i]][s_.route[j]]
                  + distance_matrix[s_.route[j]][s_.route[i+1]] + distance_matrix[s_.route[j-1]][s_.route[j+1]];
        }
        else{
          continue;
        }
        if(delta < best_delta){
        best_i = i;
        best_j = j;
        best_delta = delta;
        }
      }
    }
    if(best_delta<0){
      //cout << "previous route: " << endl;
      //for(int i=0;i<s_.route.size();i++){
      //  cout << s_.route[i] << " ";
      //}
      //cout << endl << best_i << " // " << best_j;
      //cout << endl << "cost " << best_delta << endl; 
      if(best_i<best_j){
        s_.route.insert(s_.route.begin() + best_i + 1, s_.route[best_j]);
        s_.route.erase(s_.route.begin() + best_j + 1);
      }
      else{
        s_.route.insert(s_.route.begin() + best_i + 1, s_.route[best_j]);
        s_.route.erase(s_.route.begin()+best_j);
      }
      s_.cost += best_delta;
      //cout << "best delta: " << best_delta << endl;
      //cout << "new route: " << endl;
      //for(int i=0;i<s_.route.size();i++){
      //  cout << s_.route[i] << " ";
      //}
      //cout << endl << endl;
      //if(assessment_cost(s_)){
      //  cout << "###### Wrong Reinsertion!" << endl;
        //cout << "best delta: " << best_delta << endl;
        //cout << "new route: " << endl;
        //for(int i=0;i<s_.route.size();i++){
        //cout << s_.route[i] << " ";
        //}
        //cout << endl << endl;
      //  }
      return true;
    }
    else{
      return false;
    }
    case 2:
      for(i=1; i<s_.route.size()-1;i++){
        for(j=1; j<s_.route.size()-2;j++){//changed upper limit from -3 to -2
          if(i+1<j || j<i-2){
            delta = - distance_matrix[s_.route[i]][s_.route[i+1]] - distance_matrix[s_.route[j-1]][s_.route[j]]
                    - distance_matrix[s_.route[j+1]][s_.route[j+2]] + distance_matrix[s_.route[i]][s_.route[j]]
                    + distance_matrix[s_.route[j+1]][s_.route[i+1]] + distance_matrix[s_.route[j-1]][s_.route[j+2]];
          }
          else{
            continue;
          }
          if(delta < best_delta){
            best_i = i;
            best_j = j;
            best_delta = delta;
          }
        }
      }
      if(best_delta<0){
        //cout << "previous route: " << endl;
        //for(int i=0;i<s_.route.size();i++){
        //  cout << s_.route[i] << " ";
        //}
        if(best_i < best_j){
          //cout << endl << "best i: " << best_i << " best node i: " << s_.route[best_i] << endl;
          //cout << "best j: " << best_j << " best node j: " << s_.route[best_j] << endl;
          s_.route.insert(s_.route.begin() + best_i + 1, s_.route[best_j]);
          s_.route.insert(s_.route.begin() + best_i + 2, s_.route[best_j + 2]);
          s_.route.erase(s_.route.begin() + best_j + 3);
          s_.route.erase(s_.route.begin() + best_j + 2);
        }
        else{
          //cout << endl << "best i: " << best_i << " best node i: " << s_.route[best_i] << endl;
          //cout << "best j: " << best_j << " best node j: " << s_.route[best_j] << endl;
          s_.route.insert(s_.route.begin() + best_i + 1, s_.route[best_j]);
          s_.route.insert(s_.route.begin() + best_i + 2, s_.route[best_j+1]);
          s_.route.erase(s_.route.begin()+best_j+1);
          s_.route.erase(s_.route.begin()+best_j);
        }
        s_.cost += best_delta;

        //cout << "best delta: " << best_delta << endl;
        //cout << "new route: " << endl;
        //for(int i=0;i<s_.route.size();i++){
        //  cout << s_.route[i] << " ";
        //}
        //cout << endl << endl;
        //if(assessment_cost(s_)){
        //  cout << "###### Wrong Or-2opt!" << endl;
        //  }
        //else{
        //  cout << "Right Or-2opt!" << endl;
        //}
        return true;
      }
      else{
      return false;
    }
    case 3:
      for(i=1; i<s_.route.size()-1;i++){
        for(j=1; j<s_.route.size()-3;j++){//changed from -4 to -3
          if(i+1<j || j<i-3){
            delta = - distance_matrix[s_.route[i]][s_.route[i+1]] - distance_matrix[s_.route[j-1]][s_.route[j]]
                    - distance_matrix[s_.route[j+2]][s_.route[j+3]] + distance_matrix[s_.route[i]][s_.route[j]]
                    + distance_matrix[s_.route[j+2]][s_.route[i+1]] + distance_matrix[s_.route[j-1]][s_.route[j+3]];
          }
          else{
            continue;
          }
          if(delta < best_delta){
            best_i = i;
            best_j = j;
            best_delta = delta;
          }
        }
      }
      if(best_delta<0){
        //cout << "previous route: " << endl;
        //for(int i=0;i<s_.route.size();i++){
        //  cout << s_.route[i] << " ";
        //}
        if(best_i < best_j){
        //  cout << endl << "best i: " << best_i << " best node i: " << s_.route[best_i] << endl;
        //  cout << "best j: " << best_j << " best node j: " << s_.route[best_j] << endl;
          s_.route.insert(s_.route.begin() + best_i + 1, s_.route[best_j]);
          s_.route.insert(s_.route.begin() + best_i + 2, s_.route[best_j + 2]);
          s_.route.insert(s_.route.begin() + best_i + 3, s_.route[best_j + 4]);
          s_.route.erase(s_.route.begin() + best_j + 5);
          s_.route.erase(s_.route.begin() + best_j + 4);
          s_.route.erase(s_.route.begin() + best_j + 3);
        }
        else{
          //cout << endl << "best i: " << best_i << " best node i: " << s_.route[best_i] << endl;
          //cout << "best j: " << best_j << " best node j: " << s_.route[best_j] << endl;
          s_.route.insert(s_.route.begin() + best_i + 1, s_.route[best_j]);
          s_.route.insert(s_.route.begin() + best_i + 2, s_.route[best_j+1]);
          s_.route.insert(s_.route.begin() + best_i + 3, s_.route[best_j+2]);
          s_.route.erase(s_.route.begin()+best_j+2);
          s_.route.erase(s_.route.begin()+best_j+1);
          s_.route.erase(s_.route.begin()+best_j);
        }
        s_.cost += best_delta;
        //cout << "best delta: " << best_delta << endl;
        //cout << "new route: " << endl;
        //for(int i=0;i<s_.route.size();i++){
        //  cout << s_.route[i] << " ";
        //}
        //cout << endl << endl;
        //if(assessment_cost(s_)){
        //  cout << "######### Wrong Or-3opt!" << endl;
        //  }
        return true;
      }
      else{
      return false;
    }
    default:
      cout << "Error: best_improvement_or_opt invalid parameter." << endl;
      break;
  }
  return false;
}

void local_search(solution& s_){
  
  std::vector<int> NL = {1,2,3,4,5};
  bool improved = false;
  
  while(!NL.empty()){
    int n = rand() % NL.size();
    switch (NL[n]){
      case 0:
        //improved = false;
        improved = best_improvement_swap(s_);
        break;
      case 1:
        //improved = false;
        improved = best_improvement_2opt(s_);
        break;
      case 2:
        improved = best_improvement_or_opt(s_, 1); // Reinsertion
        //improved = false;
        break;
      case 3:
        //improved = false;
        improved = best_improvement_or_opt(s_, 2); // Or-opt2
        break;
      case 4:
        //improved = false;
        improved = best_improvement_or_opt(s_, 3); // Or-opt3
        break;
    }
    
    if (improved){
      NL = {1,2,3,4,5};
    }
    else{
      NL.erase(NL.begin() + n);
    }
  }
}

int main(int argc, char** argv) {

  auto start = chrono::steady_clock::now();
  //printData();

  vector<int> CL, CL_;
  int i, max_i, max_iter_ils, count;
  solution s_, best_s, best_all_s;

  //read the instance through the auxiliary function ReadData.h based on the user input parameters
  readData(argc, argv, &dimension, &distance_matrix);

  for(i=2; i<=dimension; i++){
    CL.push_back(i);
  }

  if(dimension<=150){
    max_iter_ils = dimension/2.0;
    //max_iter_ils = 10;
  }
  else{
    max_iter_ils = dimension;
    //max_iter_ils = 2;
  }

  max_i = 50;
  best_all_s.cost = (double) INFINITY;

  for(i=0; i< max_i; i++){
    
    CL_ = CL;
    s_ = construction(CL_);
    
    //cout << "Initial Cost:" << s_.cost << endl << "Initial route:" << " ";
    //for(i=0; i<s_.route.size(); i++){
    //  cout << s_.route[i] << " ";
    //}

    best_s = s_;
    count = 0;

    while(count < max_iter_ils){
      local_search(s_);
      //cout << s_.cost << endl;
      if(s_.cost < best_s.cost){
        best_s = s_;
        //cout << best_s.cost << endl;
        count = 0;
      }
      s_ = pertubation(best_s);
      //cout << s_.cost << endl;
      //cout << count << endl;
      count++;
    }
  if(best_s.cost < best_all_s.cost){
    best_all_s = best_s;
  }
  }

  auto end = chrono::steady_clock::now();

  cout << endl << "Cost:" << best_all_s.cost << endl << "route:" << " ";
  for(i=0;i<best_all_s.route.size();i++){
    cout << best_all_s.route[i] << ' ';
  }

  cout << endl << "Elapsed Time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" <<endl;
  return 0;
}

/*
Cost:4456
route: 1 2 7 6 5 12 13 3 4 14 10 9 8 11 1
Elapsed Time: 501 µs
*/

/*
Cost:3323
route: 1 10 9 11 8 13 7 12 6 5 4 3 14 2 1 
Elapsed Time: 3 ms
*/

/*
Cost:3181
route: 1 2 3 3 12 6 5 5 7 13 11 11 9 8 1 
Elapsed Time: 7 ms
*/