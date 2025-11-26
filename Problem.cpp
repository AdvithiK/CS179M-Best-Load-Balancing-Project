//lol 

#include "Problem.h"
#include <iostream>

using namespace std;

ostream &operator<<(ostream &output, const ShipNode &node){
    //i starts backwards can row 1 is at the bottom of the node and row 8 is at the top
    for (int i = node.default_ship_state.size() - 1; i >= 0; i--) { 
        for (int j = 0; j < node.default_ship_state[i].size(); j++) { 
            output << node.default_ship_state[i][j] << "\n"; 
        }
        output << "\n";
    }
    return output;
}

//helper function to trim spacing for name
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}


void Problem::findContainers(ShipNode& node){
    //loop thru the 2D array and find all Containers, append them to the list (copy for now, prob can be done with pointers)
    

    for (int i = node.default_ship_state.size() - 1; i >= 0; --i) { 
        for (int j = 0; j < node.default_ship_state[i].size(); ++j) { 
            string name = trim(node.default_ship_state[i][j].name);
            // if (node.default_ship_state[i][j].name != "UNUSED" && node.default_ship_state[i][j].name != "NAN" ){
            //     Container temp_container = node.default_ship_state[i][j];
            //     containers.push_back(temp_container);
            // }
            if (name != "UNUSED" && name != "NAN"){
                Container temp_container = node.default_ship_state[i][j];
                containers.push_back(temp_container);
            }
        }

    }

};

void Problem::printContainersList(){
    for(int i = 0; i < containers.size(); i++){
        //testing for spacing
        //cout << "*" << containers.at(i).name << "\n";
        cout << containers.at(i).name << "\n";
    }
    //cout << containers.size();
}



//search algo function, a* algo
void Problem::searchSolutionPath(ShipNode& node){
    //push initial ShipNode to priority queue

    //make temp ShipNode

    //while the queue is not empty or a solution is found
    //temp Shipnode = top of priority queue + pop it off 
    //check if shipnode is in a valid state
    //else explore the node (add this to the priority queue)


};

//calculates the heuristics, p side and s side etc...
void Problem::calculateShipNode(ShipNode& node){
    //find vals for: 
    //int sum_weight; int p_side_weight; int s_side_weight; int heuristic;
    //NOTE: cost should be calculated in search

    //set the p_side & s_side weights
    for(int i = 0; i < containers.size(); i++){
        //checks to see if it should be on p_side (x coord is 1-6 (0-5 index)) or s_side (x coord is 7-12 (6-11 index))
        
        int half_ship = (node.default_ship_state[0].size()/2);
        if(containers.at(i).final_x < half_ship ){
            node.p_side_weight += containers.at(i).weight;
        }
        else{
            node.s_side_weight += containers.at(i).weight;
        }
    }

    //set the total weight on the ship
    node.sum_weight = node.p_side_weight + node.s_side_weight;

    node.difference_weight = abs(node.p_side_weight - node.s_side_weight);

    //calculate the heuristic
    node.heuristic = node.difference_weight - (node.sum_weight*0.10);

};

void Problem::printCalculations(ShipNode& node){
    cout << "The weights & details for this ship is: " << endl;
    cout << "The total port side weight on this ship is: " << node.p_side_weight << endl;
    cout << "The total sea side weight on this ship is: " << node.s_side_weight << endl;
    cout << "The total weight on this ship is: " << node.sum_weight << endl;
    cout << "The difference between port side and sea side weight on this ship is: " << node.difference_weight << endl;
    cout << "The max overweight factor is: " << (node.sum_weight*0.10) << endl;
    cout << "This ship is: ";
    if(balanceCheck(node)){
        cout << "BALANCED";
    }
    else{
        cout << "NOT BALANCED";
    }
    cout << endl;
    cout << "The cost (depth) of the ship is: " << node.cost << endl;
    cout << "The heuristic of the ship is: "<< node.heuristic << endl;
    
}

bool Problem::balanceCheck(ShipNode& node){
    if(node.difference_weight <= (node.sum_weight*0.10)){
        return true;
    }
    return false;
}



//calls the ShipNode operations and creates new ship nodes to add to queue
void Problem::exploreShipNodes(ShipNode& node){
    //basically check if the up,down,left,right containers has freespot=true
    //if so call the up,down,left,right functions and push to queue
    //check for duplicates!! just create the node as normal then call the == and if so, dont push to queue
};

//repeatedly calls the container to be moved down until container is not floating
void Problem::moveShipNodeDown(ShipNode& node){};

//once final ShipNode is found, this function adds it and all it's ancestors to the final solution stack
void Problem::traceSolutionPath(ShipNode& node){};



//movements for different states of ShipNode
//up
ShipNode Problem::up(const ShipNode &node){
    //temp Shipnode = node
    //swap the container above it with it
    //reset cost, parent, heurisitic?? -> call calculateShipNode
    //return the temp
    return node;

};

//down
ShipNode Problem::down(const ShipNode &node){
    //temp Shipnode = node
    //swap the container below it with it
    //reset cost, parent, heurisitic?? -> call calculateShipNode
    //return the temp
    return node;

};

//left
ShipNode Problem::left(const ShipNode &node){
    //temp Shipnode = node
    //swap the container to the left of it with it
    //reset cost, parent, heurisitic?? -> call calculateShipNode
    //return the temp
    return node;
};

//right
ShipNode Problem::right(const ShipNode &node){
    //temp Shipnode = node
    //swap the container to the right of it with it
    //reset cost, parent, heurisitic?? -> call calculateShipNode
    //return the temp

    return node;
};
