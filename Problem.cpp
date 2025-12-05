#include "Problem.h"
#include <iostream>

using namespace std;

ostream &operator<<(ostream &output, const ShipNode &node){
    //i starts backwards can row 1 is at the bottom of the node and row 8 is at the top
    for (int i = node.default_ship_state.size() - 1; i > 0; i--) { 
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

//gets the index y coordinate in a node NOT THE FINAL Y 
int get_y_coord(const ShipNode& node, const Container& box){
    for(int i=0; i<node.default_ship_state.size(); i++){
        for(int j=0; j<node.default_ship_state[i].size(); j++){
            const Container& c = node.default_ship_state[i][j];
            if(trim(c.name) == trim(box.name) &&
               c.weight == box.weight &&
               c.initial_y == box.initial_y &&
               c.initial_x == box.initial_x){
                return i;
            }
        }
    }
    return -1;
}

//gets the index x coordinate in a node NOT THE FINAL X 
int get_x_coord(const ShipNode& node, const Container& box){
    for(int i=0; i<node.default_ship_state.size(); i++){
        for(int j=0; j<node.default_ship_state[i].size(); j++){
            const Container& c = node.default_ship_state[i][j];
            if(trim(c.name) == trim(box.name) &&
               c.weight == box.weight &&
               c.initial_y == box.initial_y &&
               c.initial_x == box.initial_x){
                return j;
            }
        }
    }
    return -1;
}



void Problem::findContainers(const ShipNode& node){
    //loop thru the 2D array and find all Containers, append them to the list (copy for now, prob can be done with pointers)
    
    for (int i = node.default_ship_state.size() - 1; i >= 0; i--) { 
        for (int j = 0; j < node.default_ship_state[i].size(); j++) { 
            string name = trim(node.default_ship_state[i][j].name);
            // if (node.default_ship_state[i][j].name != "UNUSED" && node.default_ship_state[i][j].name != "NAN" ){
            //     Container temp_container = node.default_ship_state[i][j];
            //     containers.push_back(temp_container);
            // }
            if (name != "UNUSED" && name != "NAN" && name != "CRANE"){
                Container temp_container = node.default_ship_state[i][j];
                containers.push_back(temp_container);
            }
        }
    }

};

void Problem::updateContainers(const ShipNode& node){
    for(Container &box : containers){ 
        for(int i = node.default_ship_state.size()-1; i >= 0; i--){
            for(int j = 0; j < node.default_ship_state[i].size(); j++){

                if(trim(node.default_ship_state[i][j].name) == trim(box.name)){
                    //recomputing final_y and final_x
                    box.final_y = node.default_ship_state.size() - i;
                    box.final_x = j + 1;
                    goto next_container;
                }
            }
        }
        next_container:;
    }
}

void Problem::printContainersList(ShipNode& node){

    for(int i = 0; i < containers.size(); i++){
        //testing for spacing
        //cout << "*" << containers.at(i).name << "\n";
        int index_coord_y = (node.default_ship_state.size() - containers.at(i).final_y );
        int index_coord_x = containers.at(i).final_x-1;
        cout << containers.at(i) << endl;
        cout << "index: (" << index_coord_y << ", " << index_coord_x << ")" << endl;
    }
    //cout << containers.size();
}


//calculates the heuristics, p side and s side etc...
void Problem::calculateShipNode(ShipNode& node){
    //find vals for: 
    //int sum_weight; int p_side_weight; int s_side_weight; int heuristic;
    //NOTE: cost should be calculated in search
    node.p_side_weight = 0;
    node.s_side_weight = 0;

    //set the p_side & s_side weights
    for(int i = 0; i < containers.size(); i++){
        //checks to see if it should be on p_side (x coord is 1-6 (0-5 index)) or s_side (x coord is 7-12 (6-11 index))
        
        int half_ship = (node.default_ship_state[0].size()/2);
        if(containers.at(i).final_x <= half_ship ){
            node.p_side_weight += containers.at(i).weight;
        }
        else{
            node.s_side_weight += containers.at(i).weight;
        }
    }

    //set the total weight on the ship
    node.sum_weight = node.p_side_weight + node.s_side_weight;

    //calculate the heuristic
    node.heuristic = abs(node.p_side_weight - node.s_side_weight) - (node.sum_weight*0.10);

};

void Problem::printCalculations(ShipNode& node){
    cout << "The weights & details for this ship is: " << endl;
    cout << "The total port side weight on this ship is: " << node.p_side_weight << endl;
    cout << "The total sea side weight on this ship is: " << node.s_side_weight << endl;
    cout << "The total weight on this ship is: " << node.sum_weight << endl;
    cout << "The difference between port side and sea side weight on this ship is: " << abs(node.p_side_weight - node.s_side_weight) << endl;
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
    bool truth = false;
    if(containers.size()<=1){
        truth = true;
    }
    // else if(containers.size() == 2 && (node.p_side_weight >= node.s_side_weight || node.s_side_weight >= node.p_side_weight)){
    //     truth = true;
    // }
    else if(abs(node.p_side_weight - node.s_side_weight) <= (node.sum_weight*0.10)){
        truth = true;
    }
    return truth;
}




void Problem::exploreShipNodes(ShipNode& node, Container& box){
    cout << "exploring possible nodes" << endl;
    vector<pair<int, int>> dest_list = find_dest_list(node, box);
    cout << "exploring " << dest_list.size() <<" nodes" << endl;
    for (const auto& p : dest_list) {
        int index_y = p.first;
        int index_x = p.second;

        int container_y = get_y_coord(node, box);
        int container_x = get_x_coord(node, box);

        ShipNode new_node = node;

        new_node.cost += abs(container_y-index_y) - abs(container_x - index_x);

        swap(new_node.default_ship_state[index_y][index_x], new_node.default_ship_state[index_y][index_x]);


       //getCrane, grab the y and x indexes, swap crane with index_y-1, index_x DO IT ON NEW_NODE !!
        int crane_y = get_y_coord(new_node, getCrane(new_node));
        int crane_x = get_x_coord(new_node, getCrane(new_node));
        swap(new_node.default_ship_state[crane_y][crane_x], new_node.default_ship_state[index_y-1][index_x]);

        calculateShipNode(new_node);

        if(balanceCheck(new_node)){
            cout << "exploring function balance check made" << endl;
            final_ship_state = new_node;
            balCheck = true;
            goto rat;
        }
        
        //append to unexplored
        unexplored_ship_states.push(new_node);

    }
    rat:;

};


vector<pair<int,int>> Problem::find_dest_list(const ShipNode& node, Container& box) {
    vector<pair<int,int>> dest_list;
    for(int j = 0; j < 12; j++) {
        for(int i = 8; i >= 0; i--) {
            if (j == get_x_coord(node, box)){
                goto next_x;
            }
            if (trim(node.default_ship_state[i][j].name) == "UNUSED") {
                pair<int,int> dest = {i,j};
                dest_list.push_back(dest);
                cout << "NEW DESTINATION ADDED: (" << dest.first << ", " << dest.second << ")" << endl;
                goto next_x;
            }

        } 
        next_x:;      
    }

    return dest_list;
    

}



bool Problem::containerBelowCrane(const ShipNode& node, Container& box) {

    //includes the row of air above the ship
     for(int i = 8; i >= 0; i--){
        for(int j = 0; j < 12; j++) {
            if (node.default_ship_state[i][j].name == "CRANE") {
                if (node.default_ship_state[i+1][j] == box)
                    return true;
                return false;
            }
            
        }
    }

    cout << "CRANE is missing" << endl;
    return false;


}



void Problem::updatefreeSpots(ShipNode& node){
    for (int i = node.default_ship_state.size() - 1; i >= 0; --i) { 
        for (int j = 0; j < node.default_ship_state[i].size(); ++j) { 
            string name = trim(node.default_ship_state[i][j].name);
            if(name == "UNUSED"){
                node.default_ship_state[i][j].free_spot = true;
            }
        }
    }
}

Container Problem::getCrane(const ShipNode& node){
    for(int i=0; i<node.default_ship_state.size(); i++){
        for(int j=0; j<node.default_ship_state[i].size(); j++){
            const Container& c = node.default_ship_state[i][j];
            if(trim(c.name) == "CRANE" ){
                return c;
            }
        }
    }
    cout << "OBJECT NOT FOUND" << endl;
    return node.default_ship_state[0][0];

}

bool Problem::craneCheck(const ShipNode& node, const Container& c){
    bool truth = false;

    int container_y = get_y_coord(node, c);
    int container_x = get_x_coord(node, c);
    int crane_y = get_y_coord(node, getCrane(node));
    int crane_x = get_x_coord(node, getCrane(node));

    if(crane_y+1 == container_y && crane_x == container_x){
        truth = true;
    }
    return truth;
};

bool Problem::checkUp(const ShipNode &node, const Container& box){

    int index_coord_y = get_y_coord(node, box);
    int index_coord_x = get_x_coord(node, box);
    bool availability = false;

    
    if(index_coord_y > 0){
        if(node.default_ship_state[index_coord_y-1][index_coord_x].free_spot == true){
            availability = true;
        }
    }
    cout << "(" << index_coord_y-1 << ", " << index_coord_x << ")!!" << endl;
    cout << "truth value: " << (node.default_ship_state[index_coord_y-1][index_coord_x].free_spot) << endl;
    cout << "!!" << availability<< "!!" << endl;
    return availability;

};

void Problem::moveCranetoContainer(ShipNode &node, const Container& box){
    cout << "moving crate to container" << endl;
    int container_y = get_y_coord(node, box);
    int container_x = get_x_coord(node, box);
    int crane_y = get_y_coord(node, getCrane(node));
    int crane_x = get_x_coord(node, getCrane(node));

    node.cost += (abs(container_y-crane_y) + abs(container_x-crane_x))-1;

    swap(node.default_ship_state[crane_y][crane_x], node.default_ship_state[container_y-1][container_x]);
    cout << "finished moving crane to container. crane is now at: " << "(" << get_y_coord(node, getCrane(node)) << ", " << get_x_coord(node, getCrane(node)) << "). name: "<<node.default_ship_state[container_y-1][container_x].name << endl;
};

//search algorithm here
void Problem::searchSolutionPath(){
    cout << "in search solution path function" << endl;
    ShipNode curr_node;
    unexplored_ship_states.push(initial_ship_state);
    cout << "size of unexplored ship states" << endl;
    while(!unexplored_ship_states.empty() && !balCheck){
        cout << "in while loop" << endl;
        curr_node = unexplored_ship_states.top();
        unexplored_ship_states.pop();
        for (int c = 0; c < containers.size(); c++){
            cout << "in for loop for container:" << containers.at(c) << endl;
            int index_coord_y = get_y_coord(curr_node, containers.at(c));
            int index_coord_x = get_x_coord(curr_node, containers.at(c));
            //ensuring container is not going out of bounds
            bool noboundaryUp = index_coord_y > 0;
            //grabbing name of spot above container
            string upspot_name = trim(curr_node.default_ship_state[index_coord_y-1][index_coord_x].name);
            
            if(balCheck){
                cout << "ran through balCheck" << endl;
                goto dog;
            }
            
            //if crane is above c
            if(craneCheck(curr_node, containers.at(c))){
                cout << "ran through craneCheck" << endl;
                exploreShipNodes(curr_node, containers.at(c));

            }
            else{
                cout << "in else" << endl;
                //if c is moveable
                cout << "truth value: " << noboundaryUp << endl;
                if(noboundaryUp && upspot_name == "UNUSED"){
                    cout << "ran through container is moveable" << endl;
                    ShipNode crane_moved_node = curr_node;
                    moveCranetoContainer(crane_moved_node, containers.at(c));
                    unexplored_ship_states.push(crane_moved_node);
                    //should not explore here because it'll reference initial node as parent, it should reference cr
                    //exploreShipNodes(crane_moved_node, containers.at(c));
                }

            }
            cout << "done with 1 run of for loop" << endl;
        } 
        
    }
    dog:;
    cout << "finished search" << endl;
    final_ship_state = curr_node;


    
};

//once final ShipNode is found, this function adds it and all it's ancestors to the final solution stack
void Problem::traceSolutionPath(ShipNode& node){};
