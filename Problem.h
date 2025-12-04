#ifndef PROBLEM_H
#define PROBLEM_H

#include <iostream>
#include <queue>
#include <stack>
#include <cmath>
#include <iomanip>
#include <vector>
#include <string>
#include <utility>
#include "UI/json.hpp"
#include <fstream>
#include <utility>

using json = nlohmann::json;

using namespace std;

/* example ShipNode object (2D array) -> type ShipNode
        1      2        3       4       5      6        7       8       9       10      11      12
    {
8    {PARK, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED},
7    {UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED},
6    {UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED},
5    {UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED},
4    {UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED},
3    {UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED},
2    {UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED},
1    {NAN, UNUSED, Fish for Wendys, UNUSED, UNUSED, Bikes, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, NAN},
    };
*/

struct Container{
    
    //stores the container's name
    string name;
    
    //stores the weight of the container
    int weight;

    //storing initial and final positions for easier access for output purposes
    //stores the initial position of the container
    /* UPDATE: now in [x,y] format*/
    int initial_x;
    int initial_y;

    //stores the final position of the container
    /* UPDATE: now in [x,y] format*/
    int final_x = initial_x;
    int final_y = initial_y;

    //free spot flag (empty space, UNUSED)
    bool free_spot = false;

    //default Container object
    Container() : name("UNUSED"), weight(0), initial_x(0), initial_y(0), final_x(0), final_y(0), free_spot(true) {};
    
    //Container constructor (to make all the containers from file read)
    Container(int given_x, int given_y, int given_weight, string given_name) : name(given_name), weight(given_weight), initial_x(given_x), initial_y(given_y), final_x(given_x), final_y(given_y), free_spot(false){};


    //helping overload operators :) besties fr
    friend ostream& operator<<(ostream& out, const Container& box){
        //saving the format of the current output 
        ios oldState(nullptr);
        oldState.copyfmt(out);
        out << "[" << setw(2) << setfill('0') << box.final_y << "," << setw(2) << setfill('0') << box.final_x << "]" << ", " 
        << "{" << setw(5) << setfill('0') << box.weight << "}" << ", " << box.name;
        //restoring the format of the current output 
        out.copyfmt(oldState);
        return out;
    }

    //operators

    bool operator==(const Container& rhs) const{
        bool twinsies = true;
        if (!(weight == rhs.weight && final_y == rhs.final_y && final_x == rhs.final_x)){
            twinsies = false;
        }
        return twinsies;
    }

    bool operator!=(const Container& rhs) const{
        bool twinsies = false;
        if (weight == rhs.weight && final_y == rhs.final_y && final_x == rhs.final_x){
            twinsies = true;
        }
        return twinsies;
    }

};


struct ShipNode{

    vector<vector<Container>> default_ship_state = vector<vector<Container>>(9, vector<Container>(12));


    // //stores a list of containers in the ship (pointers? im doing copies for time being)
    // vector<Container> containers;
    
    //highkey don't remember much on pointers. isabelleeeeee helpppp plzzzz
    ShipNode* parent;


    //total weight on the ship node
    int sum_weight;

    //total weight on the port side
    int p_side_weight;

    //total weight on the sea side
    int s_side_weight;

    //cost (depth) of the ship node
    int cost = 0;

    //heuristic (how far node is from final solution)
    //measured via distance from minimum viable balancing different
    int heuristic;

    //overload operators for ShipNode 

    //less than operator for the priority queue to measure priority
    bool operator< (const ShipNode& rhs) const{

        return (cost+heuristic) > (rhs.cost + rhs.heuristic);
    }

    //assignment operator to make a copy of a ShipNode
    ShipNode &operator=(const ShipNode& rhs){
        
        //copying all the containers from rhs to shipNode
        for (int i = default_ship_state.size() - 1; i >= 0; i--) { 
            for (int j = 0; j < default_ship_state[i].size(); j++) { 

                default_ship_state[i][j] = rhs.default_ship_state[i][j];
            }

        }
        

        //containers = rhs.containers;

        sum_weight = rhs.sum_weight;
        
        p_side_weight = rhs.p_side_weight;

        s_side_weight = rhs.s_side_weight;

        cost = rhs.cost;

        heuristic = rhs.heuristic;

        //idk about this...
        parent = rhs.parent;

        return *this;

    }

    //equals to comparison operator 
    bool operator==(const ShipNode& rhs){
        bool twinsies = true;
        for (int i = default_ship_state.size() - 1; i >= 0; i--) { 
            for (int j = 0; j < default_ship_state[i].size(); j++) { 
                if(default_ship_state[i][j] != rhs.default_ship_state[i][j]){
                    twinsies = false;
                }
            }

        }
        return twinsies;
    }



    //if we make parent a pointer, change it here (currently set to -> this, should it be nullptr?)
    //ShipNode(): default_ship_state(vector<vector<Container>>(8, vector<Container>(12))) {};
    ShipNode(): default_ship_state(vector<vector<Container>>(3, vector<Container>(4))), parent(this), sum_weight(0), p_side_weight(0), s_side_weight(0), cost(0), heuristic(0) {};
    ShipNode(const vector<vector<Container>>& given_ship_node) : default_ship_state(given_ship_node), parent(this), sum_weight(0), p_side_weight(0), s_side_weight(0), cost(0), heuristic(0) {};



};

//output operator for testing purposes (until we get the GUI ready, this is just for commandline output visual)
ostream &operator<<(ostream &output, const ShipNode &node);

class Problem
{
public:

    Problem();
    Problem(ShipNode& node): initial_ship_state(node) {};

    ShipNode initial_ship_state;
    ShipNode final_ship_state;

    //stores a list of containers in the ship (pointers? im doing copies for time being)
    vector<Container> containers;

    //priority based on lowest cost states
    priority_queue<ShipNode> unexplored_ship_states;

    vector<ShipNode> explored_ship_states;

    //stores the order of each step to the final solution state
    stack<ShipNode> solution_path;


    //finds all the containers in the ship & adds it to the containers list
    void findContainers(const ShipNode& node);

    //updates the final x and y of containers
    void updateContainers(const ShipNode& node);

    bool containerBelowCrane(const ShipNode& node, Container& box);

    void updatefreeSpots(ShipNode& node);


    //search algo function 
    void searchSolutionPath();

    ShipNode returnSolutionNode();

    //calculates the heuristics, p side and s side etc...
    void calculateShipNode(ShipNode& node);

    //calls the ShipNode operations and creates new ship nodes to add to queue
    void exploreShipNodes(ShipNode& node, Container& box);

    //once final ShipNode is found, this function adds it and all it's ancestors to the final solution stack
    void traceSolutionPath(ShipNode& node);

    //checker to see if containers was populated
    //DONE
    void printContainersList(ShipNode& node);

    //checker to see all calculations for a node
    //DONE
    void printCalculations(ShipNode& node);

    //checks the balance of the ship
    //DONE
    bool balanceCheck(ShipNode& node);


    void setUI(const ShipNode &node);

    void alterLog(string comment);

    vector<pair<int,int>> find_dest_list(const ShipNode& node, Container& box) {}



};


#endif