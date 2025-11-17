#ifndef PROBLEM_H
#define PROBLEM_H

#include <iostream>
#include <queue>
#include <stack>
#include <cmath>

using namespace std;

/* example ShipNode object (2D array) -> int (assign strings to nums) or a char type (keep words)???
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
    int final_y;
    int final_x;


};


struct ShipNode{

    //deafult object shape for each ShipNode
    char default_ship_state[8][12];

    //overload operators for Node output, assignment, & comparisons to right hand side Nodes
    
    //less than operator for comparison
    bool operator< (const ShipNode&rhs) const{}

    //equal to operator for comparison
    bool operator== (const ShipNode&rhs) const{}

    //assignment operator for assignment
    ShipNode &operator= (const ShipNode&rhs) const{}



};

//output operator for testing purposes (until we get the GUI ready, this is just for commandline output visual)
ostream &operator<<(ostream &output, const ShipNode &node);

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


};


#endif