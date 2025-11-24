#ifndef PROBLEM_H
#define PROBLEM_H

#include <iostream>
#include <queue>
#include <stack>
#include <cmath>
#include <iomanip>

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
    /* NOTE: ALL COORDS ARE GIVEN AS [y,x] NOT [x,y] !! */
    int initial_y;
    int initial_x;

    //stores the final position of the container
    /* NOTE: ALL COORDS ARE GIVEN AS [y,x] NOT [x,y] !! */
    int final_y = initial_y;
    int final_x = initial_x;

    //free spot flag (empty space, UNUSED)
    bool free_spot = false;

    //default Container object
    Container() : name("UNUSED"), weight(0), initial_y(0), initial_x(0), final_y(0), final_x(0), free_spot(true) {}
    
    //Container constructor (to make all the containers from file read)
    Container(int given_y, int given_x, int given_weight, string given_name) : name(given_name), weight(given_weight), initial_y(given_y), initial_x(given_x), final_y(given_y), final_x(given_x), free_spot(false){}


    //helping overload operators :) besties fr
    friend ostream& operator<<(ostream& out, const Container& box){
        //saving the format of the current output 
        ios oldState(nullptr);
        oldState.copyfmt(out);
        out << "[" << setw(2) << setfill('0') << box.initial_y << "," << setw(2) << setfill('0') << box.initial_x << "]" << ", " 
        << "{" << setw(5) << setfill('0') << box.weight << "}" << ", " << box.name;
        //restoring the format of the current output 
        out.copyfmt(oldState);
        return out;
    }

};


struct ShipNode{

    //deafult object shape for each ShipNode
    vector<vector<Container>> default_ship_state;

    //overload operators for Node output, assignment, & comparisons to right hand side Nodes
    
    //less than operator for comparison
    bool operator< (const ShipNode&rhs) const{
        return default_ship_state < rhs.default_ship_state;
    }

    //equal to operator for comparison
    bool operator== (const ShipNode&rhs) const{
        return default_ship_state == rhs.default_ship_state;
    }

    //assignment operator for assignment
    ShipNode &operator= (const ShipNode&rhs){
        if(this != &rhs){
            default_ship_state = rhs.default_ship_state;
        }
        return *this;
    }



    ShipNode(const vector<vector<Container>>& given_ship_node) : default_ship_state(given_ship_node) {}



};

//output operator for testing purposes (until we get the GUI ready, this is just for commandline output visual)
// ostream &operator<<(ostream &output, const ShipNode &node){
//     for (int i = 0; i < node.default_ship_state.size(); ++i) { 
//         for (int j = 0; j < node.default_ship_state[i].size(); ++j) { 
//             output << node.default_ship_state[i][j] << "\n"; 
//         }
//         output << "\n";
//     }
//     return output;
// }

class Problem
{
public:

    ShipNode initial_ship_state;
    ShipNode final_ship_state;

    //priority based on lowest cost states
    priority_queue<ShipNode> unexplored_ship_states;

    vector<ShipNode> explored_ship_states;

    //stores the order of each step to the final solution state
    stack<ShipNode> solution_path;

    Problem(char manifest_ship_node[8][12]);


    //movements for different states of ShipNode
    //up
    ShipNode up(const ShipNode &node);
    
    //down
    ShipNode down(const ShipNode &node);
    
    //left
    ShipNode left(const ShipNode &node);
    
    //right
    ShipNode right(const ShipNode &node);

    //f(n) = g(n) + h(n)

    //g(n) = depth = cost

    //heuristic??


    /*heuristic final check 
    
    number ! final_end_outcome: p.side,  s.side = (p.side,  s.side) - sumx0.10

    
    */



};


#endif