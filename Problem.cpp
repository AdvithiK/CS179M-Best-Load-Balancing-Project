//lol 

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


void Problem::findContainers(const ShipNode& node){
    //loop thru the 2D array and find all Containers, append them to the list (copy for now, prob can be done with pointers)
    
    for (int i = node.default_ship_state.size() - 1; i >= 0; i--) { 
        for (int j = 0; j < node.default_ship_state[i].size(); j++) { 
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


void Problem::queueNonFloatingStates(Container& box){
    for(int i = 0; i < explored_ship_states.size(); i++){
        if(!checkDown(explored_ship_states.at(i), box)){
            unexplored_ship_states.push(explored_ship_states.at(i));
        }
    }
}

bool Problem::floatingState(ShipNode& node, Container& box){
    int index_coord_y = get_y_coord(node, box);
    int index_coord_x = get_x_coord(node, box);
    bool checkD = false;
    bool boundaryD = index_coord_y < node.default_ship_state.size()-1;
    if(boundaryD){
        if(node.default_ship_state[index_coord_y+1][index_coord_x].free_spot == true){
            checkD = true;
        }
    }

    return checkD;

}

void Problem::findFinalPath(ShipNode& node){
    ShipNode new_node = node;
    ShipNode solution_node;
    while(!solutionCheck){
        for(int i = 0; i < containers.size(); i++){
            int index_coord_y = get_y_coord(node, containers.at(i));
            int index_coord_x = get_x_coord(node, containers.at(i));
            cout << "Initial Indexes:" << index_coord_y << "," << index_coord_x << "\n";
            if(checkUp(new_node, containers.at(i))){
                //new_node = searchSolutionPath(new_node, containers.at(i));
                if(balanceCheck(new_node)){
                    cout << "*BAGELRAT*" << endl;
                    solutionCheck = true;
                    solution_node = new_node;
                    goto success;
                }
                else{
                    new_node = searchSolutionPath(new_node, containers.at(i));
                }
            }
        }
    }
    success:;
    final_ship_state = solution_node;
    cout << final_ship_state;

}


//search algo function, a* algo
ShipNode Problem::searchSolutionPath(ShipNode& node, Container& box){

    bool floatingCheck = true;
    ShipNode curr_node = node;
    ShipNode solution_node;

    unexplored_ship_states.push(node);
    //cout << "*" << unexplored_ship_states.size() << "*" << endl;

    while(!unexplored_ship_states.empty()){

        cout << "!*\n\n"<< unexplored_ship_states.top() << "*!" << endl;
        curr_node = unexplored_ship_states.top();
        unexplored_ship_states.pop();
        explored_ship_states.push_back(curr_node);
        
        if(!floatingCheck){
            cout << "*BAGELBIRD*" << endl;
            floatingCheck = floatingState(curr_node, box);
            //solution_node = curr_node;
            goto rat;
        }
        else{
            cout << "*BAGELDOG*" << endl;
            updateContainers(curr_node);
            exploreShipNodes(curr_node, box);
        }

    }
    rat:;
    return curr_node;

};

/*
//search algo function, a* algo
void Problem::searchSolutionPath(ShipNode& node){

    bool solutionCheck = false;

    ShipNode curr_node = node;
    ShipNode solution_node;

    unexplored_ship_states.push(node);
    //cout << "*" << unexplored_ship_states.size() << "*" << endl;
    bool floating = true;
    while(!unexplored_ship_states.empty() && !solutionCheck){
        for(int i = 0; i < containers.size(); i++){
            int index_coord_y = get_y_coord(node, containers.at(i));
            int index_coord_x = get_x_coord(node, containers.at(i));
            cout << "Initial Indexes:" << index_coord_y << "," << index_coord_x << "\n";
            if(checkUp(curr_node, containers.at(i))){
                cout << "**PASSED**" << endl;
                while(floating){
                    cout << "**PASSED FLOATING**" << endl;
                    cout << "!*\n\n"<< unexplored_ship_states.top() << "*!" << endl;
                    curr_node = unexplored_ship_states.top();
                    unexplored_ship_states.pop();
                    explored_ship_states.push_back(curr_node);
                    
                    if(balanceCheck(curr_node)){
                        cout << "*BAGELRAT*" << endl;
                        solutionCheck = true;
                        solution_node = curr_node;
                        if(solutionCheck){
                            goto rat;
                        }
                    }
                    else{
                        cout << "*BAGELDOG*" << endl;
                        updateContainers(curr_node);
                        exploreShipNodes(curr_node, containers.at(i));
                        floating = floatingState(curr_node, containers.at(i));
                    }
                }
                //is there containers floating? 
                //yes -> push the non-floating containers to unexplored
                //call a function that loops through explored_ship_states and added the non-floating states to unexplored
                //unexplored_ship_states.push(curr_node);
                queueNonFloatingStates(containers.at(i));
                curr_node = unexplored_ship_states.top();
            }
            
        }
    }
    rat:;
    final_ship_state = solution_node;
    cout << final_ship_state;

};
*/


ShipNode Problem::returnSolutionNode(){
    return final_ship_state;
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



//calls the ShipNode operations and creates new ship nodes to add to queue
void Problem::exploreShipNodes(ShipNode& node, Container& box){
    //basically check if the up,down,left,right containers has freespot=true
    //if so call the up,down,left,right functions and push to queue
    //check for duplicates!! just create the node as normal then call the == and if so, dont push to queue
        int index_coord_y = get_y_coord(node, box);
        int index_coord_x = get_x_coord(node, box);
        bool boundaryUp = index_coord_y > 0 && node.default_ship_state[index_coord_y-1][index_coord_x].free_spot == true;
        //if a container can't move up, it should not be able to move at all !
        //move up & all the way down OR UP + L,R & Down
        if(boundaryUp){
            bool checkU = node.default_ship_state[index_coord_y-1][index_coord_x].free_spot == true;
            if(checkU){
                cout << box << endl;
                ShipNode final_ship_node = up(node, box);
                if(!checkDuplicate(final_ship_node)){
                    unexplored_ship_states.push(final_ship_node);
                }
            }
        }
        //move left & all the way down

        bool boundaryL = index_coord_x > 0;
        if(boundaryUp && boundaryL){
            bool checkL = index_coord_x > 0 && node.default_ship_state[index_coord_y][index_coord_x-1].free_spot == true;
            if(checkL){
                ShipNode final_ship_node = left(node, box);
                if(!checkDuplicate(final_ship_node)){
                    unexplored_ship_states.push(final_ship_node);
                }
            }
        }
        //move right & all the way down

        bool boundaryR = index_coord_x < node.default_ship_state[0].size()-1;
        if(boundaryUp && boundaryR){
            bool checkR = node.default_ship_state[index_coord_y][index_coord_x+1].free_spot == true;
            if(checkR){
                ShipNode final_ship_node = right(node, box);
                if(!checkDuplicate(final_ship_node)){
                    unexplored_ship_states.push(final_ship_node);
                }
            }
        }

        bool boundaryD = index_coord_y < node.default_ship_state.size()-1;
        if(boundaryD){
            bool checkD = node.default_ship_state[index_coord_y+1][index_coord_x].free_spot == true;
            if(checkD){
                cout << box << endl;
                ShipNode final_ship_node = down(node, box);
                if(!checkDuplicate(final_ship_node)){
                    unexplored_ship_states.push(final_ship_node);
                }
            }
        }


};

//swap the containers in the node
//y coords (8-1) -> index (0-7) & x coords (1-12) -> index (0-11)
//all swaps deal with index only, this function changes the final_y & final_x manually 
void Problem::swapContainers(ShipNode& node, int prev_y, int prev_x, int new_y, int new_x){

    //int temp_y = node.default_ship_state[new_y][new_x].final_y;
    //int temp_x = node.default_ship_state[new_y][new_x].final_x;
    //cout << "(" << node.default_ship_state[new_y][new_x].final_y <<","<< node.default_ship_state[new_y][new_x].final_x << ")" <<endl;
    
    swap(node.default_ship_state[prev_y][prev_x], node.default_ship_state[new_y][new_x]);

    //cout << "(" << node.default_ship_state[new_y][new_x].final_y <<","<< node.default_ship_state[new_y][new_x].final_x << ")" <<endl;
    
    //restore the final y & final x at coords
    //node.default_ship_state[prev_y][prev_x].final_y = node.default_ship_state[new_y][new_x].final_y;
    //node.default_ship_state[prev_y][prev_x].final_x = node.default_ship_state[new_y][new_x].final_x;

    //node.default_ship_state[new_y][new_x].final_y = temp_y;
    //node.default_ship_state[new_y][new_x].final_x = temp_x;

    //cout << "(" << node.default_ship_state[new_y][new_x].final_y <<","<< node.default_ship_state[new_y][new_x].final_x << ")" <<endl;

    //update the final y and x for each of the containers swapped
    node.default_ship_state[new_y][new_x].final_y = node.default_ship_state.size()-new_y;
    node.default_ship_state[new_y][new_x].final_x = new_x+1;

    node.default_ship_state[prev_y][prev_x].final_y = node.default_ship_state.size()-prev_y;
    node.default_ship_state[prev_y][prev_x].final_x = prev_x+1;
};

bool Problem::checkDuplicate(ShipNode& node){
    bool twinsies = false;

    for(int i = 0; i < explored_ship_states.size(); i++){
        if(explored_ship_states.at(i) == node){
            twinsies = true;
            break;
        }
    }
    return twinsies;
}

//once final ShipNode is found, this function adds it and all it's ancestors to the final solution stack
void Problem::traceSolutionPath(ShipNode& node){};


//check if there is a free spot above a container
bool Problem::checkUp(const ShipNode &node, const Container& box){

    int index_coord_y = get_y_coord(node, box);
    int index_coord_x = get_x_coord(node, box);
    bool availability = false;

    //if index_coord_y > 0
    if(index_coord_y > 0){
        if(node.default_ship_state[index_coord_y-1][index_coord_x].free_spot == true){
            availability = true;
        }
    }
    return availability;

};

//check if there is a free spot left of a container
bool Problem::checkLeft(const ShipNode &node, const Container& box){
    int index_coord_y = (node.default_ship_state.size() - box.final_y );
    int index_coord_x = box.final_x-1;
    bool availability = false;

    //if index_coord_x > 0
    if(index_coord_x > 0){
        if(node.default_ship_state[index_coord_y][index_coord_x-1].free_spot == true){
            availability = true;
        }
    }

    return availability;
};

//check if there is a free spot right of a container
bool Problem::checkRight(const ShipNode &node, const Container& box){
    int index_coord_y = (node.default_ship_state.size() - box.final_y );
    int index_coord_x = box.final_x-1;
    bool availability = false;

    //if index_coord_x < 11
    if(index_coord_x < node.default_ship_state[0].size()-1){
        if(node.default_ship_state[index_coord_y][index_coord_x+1].free_spot == true){
            availability = true;
        }
    }
        
    return availability;
};

//check if there is a free spot below a container
bool Problem::checkDown(const ShipNode &node, const Container& box){
    int index_coord_y = get_y_coord(node, box);
    int index_coord_x = get_x_coord(node, box);
    bool availability = false;
    cout << "(" << index_coord_y <<","<< index_coord_x << ")" <<endl;

    //if index_coord_y < 7
    if(index_coord_y < node.default_ship_state.size()-1){
        if(node.default_ship_state[index_coord_y+1][index_coord_x].free_spot == true){
            availability = true;
        }
    }
    return availability;
};



//movements for different states of ShipNode
//up
ShipNode Problem::up(const ShipNode &node, Container& box){
    //temp Shipnode = node
    //swap the container above it with it
    //reset cost, parent, heurisitic?? -> call calculateShipNode
    //return the temp
    cout << "Bringing Up" <<endl;
    ShipNode new_ship_node = node;
    //y coords (8-1) -> index (0-7) & x coords (1-12) -> index (0-11)
    //find the index coordinate for y and x
    int index_coord_y = get_y_coord(new_ship_node, box);
    int index_coord_x = get_x_coord(new_ship_node, box);


    // int index_coord_y = get_y_coord(node, box);
    // int index_coord_x = get_x_coord(node, box);

    cout << "#(" << box.final_y <<","<< box.final_x << ")#" <<endl;

    cout << "(" << index_coord_y <<","<< index_coord_x << ")" <<endl;
    //cout << index_coord_y << ", " << index_coord_x << endl;
    
    //swap the containers (this function updates their final y and final x too, dw queen)
    //swapContainers(new_ship_node, index_coord_y, index_coord_x, index_coord_y-1, index_coord_x);

    swap(new_ship_node.default_ship_state[index_coord_y][index_coord_x], new_ship_node.default_ship_state[index_coord_y-1][index_coord_x]);

    //update final y for container
    //box.final_y += 1;

    //cout << "#(" << box.final_y <<","<< box.final_x << ")#" <<endl;

    //finds & updates final y and final x for all containers
    updateContainers(new_ship_node);

    cout << "#(" << box.final_y <<","<< box.final_x << ")#" <<endl;

    //increate the cost bc we MADE A MOVEEE (i only make a move in code, never to a crush </3 <- heartbreak emoji)
    new_ship_node.cost += 1;

    //update index of the parent
    new_ship_node.parent = explored_ship_states.size()-1;

    //update all calculations for the ship node
    calculateShipNode(new_ship_node);

    // tbh - pnd lyrics break cause i can 
    //cause baby close just isn't close enoughhh, cause maybe my passion just aint enoughhh
    //i can feel that u love meeee yoOUououUhhh, i can feel that u love me yuhhh

    //return a new node !
    return new_ship_node;

};

//down
ShipNode Problem::down(const ShipNode &node, Container& box){
    //temp Shipnode = node
    //swap the container below it with it
    //reset cost, parent, heurisitic?? -> call calculateShipNode
    //return the temp

    ShipNode new_ship_node = node;
    int index_coord_y = get_y_coord(new_ship_node, box);
    int index_coord_x = get_x_coord(new_ship_node, box);

    cout << "Bringing Down" <<endl;
    //keep swapping with container below
    swapContainers(new_ship_node, index_coord_y, index_coord_x, index_coord_y+1, index_coord_x);

    //finds & updates final y and final x for all containers
    updateContainers(new_ship_node);
    
    //increate the cost 
    new_ship_node.cost += 1;

    //finds & updates final y and final x for all containers
    //findContainers(new_ship_node);

    //update index of the parent
    new_ship_node.parent = explored_ship_states.size()-1;

    //update all calculations for the ship node
    calculateShipNode(new_ship_node);



    //return a new node !
    return new_ship_node;

};

// //down
// ShipNode Problem::down(const ShipNode &node, Container& box){
//     //temp Shipnode = node
//     //swap the container below it with it
//     //reset cost, parent, heurisitic?? -> call calculateShipNode
//     //return the temp
    

//     ShipNode new_ship_node = node;
//     //y coords (8-1) -> index (0-7) & x coords (1-12) -> index (0-11)
//     //find the index coordinate for y and x
//     int index_coord_y = (node.default_ship_state.size() - box.final_y );
//     int index_coord_x = box.final_x-1;
    
//     //swap the containers (this function updates their final y and final x too, dw queen)
//     swapContainers(new_ship_node, index_coord_y, index_coord_x, index_coord_y+1, index_coord_x);
    
//     //update final y for container
//     box.final_y -= 1;

//     //updating y coord
//     index_coord_y += 1;

//     //increate the cost 
//     new_ship_node.cost += 1;

//     /* moving the container down until below it free_spot = false */
//     while( index_coord_y+1 < new_ship_node.default_ship_state.size() && new_ship_node.default_ship_state[index_coord_y+1][index_coord_x].free_spot == true){
//         //keep swapping with container below
//         swapContainers(new_ship_node, index_coord_y, index_coord_x, index_coord_y+1, index_coord_x);
        
//         //update final y for container
//         box.final_y -= 1;

//         //updating y coord
//         index_coord_y += 1;

//         //increate the cost 
//         new_ship_node.cost += 1;
        
//         //finds & updates final y and final x for all containers
//         findContainers(new_ship_node);
        
//     }

//     //finds & updates final y and final x for all containers
//     //findContainers(new_ship_node);

//     //update index of the parent
//     new_ship_node.parent = explored_ship_states.size()-1;

//     //update all calculations for the ship node
//     calculateShipNode(new_ship_node);



//     //return a new node !
//     return new_ship_node;

// };

//left
ShipNode Problem::left(const ShipNode &node, Container& box){
    //temp Shipnode = node
    //swap the container to the left of it with it
    //reset cost, parent, heurisitic?? -> call calculateShipNode
    //return the temp
    cout << "Bringing Left" <<endl;
    ShipNode new_ship_node = node;
    //y coords (8-1) -> index (0-7) & x coords (1-12) -> index (0-11)
    //find the index coordinate for y and x
    int index_coord_y = get_y_coord(new_ship_node, box);
    int index_coord_x = get_x_coord(new_ship_node, box);
    cout << "(" << index_coord_y << "," << index_coord_x << ")\n";
    
    cout << "(" << index_coord_y <<","<< index_coord_x << ")" <<endl;

    //swap the containers (this function updates their final y and final x too, dw queen)
    swapContainers(new_ship_node, index_coord_y, index_coord_x, index_coord_y, index_coord_x-1);
    
    //update final x for container
    //box.final_x -= 1;

    //finds & updates final y and final x for all containers
    updateContainers(new_ship_node);
    
    //increate the cost 
    new_ship_node.cost += 1;

    //update index of the parent
    new_ship_node.parent = explored_ship_states.size()-1;

    //update all calculations for the ship node
    calculateShipNode(new_ship_node);

    index_coord_y = get_y_coord(new_ship_node, box);
    index_coord_x = get_x_coord(new_ship_node, box);


    cout << "(" << index_coord_y <<","<< index_coord_x << ")" <<endl;
    //return a new node !
    return new_ship_node;
};

//right
ShipNode Problem::right(const ShipNode &node, Container& box){
    //temp Shipnode = node
    //swap the container to the right of it with it
    //reset cost, parent, heurisitic?? -> call calculateShipNode
    //return the temp

    cout << "Bringing Right" <<endl;
    ShipNode new_ship_node = node;

    //y coords (8-1) -> index (0-7) & x coords (1-12) -> index (0-11)
    //find the index coordinate for y and x
    int index_coord_y = get_y_coord(new_ship_node, box);
    int index_coord_x = get_x_coord(new_ship_node, box);

    cout << "(" << index_coord_y <<","<< index_coord_x << ")" <<endl;
    //swap the containers (this function updates their final y and final x too, dw queen)
    swapContainers(new_ship_node, index_coord_y, index_coord_x, index_coord_y, index_coord_x+1);

    //update final x for container
    //box.final_x += 1;


    //finds & updates final y and final x for all containers
    updateContainers(new_ship_node);
    //increate the cost 
    new_ship_node.cost += 1;

    //update index of the parent
    new_ship_node.parent = explored_ship_states.size()-1;

    index_coord_y = get_y_coord(new_ship_node, box);
    index_coord_x = get_x_coord(new_ship_node, box);

    cout << "(" << index_coord_y <<","<< index_coord_x << ")" <<endl;

    //update all calculations for the ship node
    calculateShipNode(new_ship_node);

    //return a new node !
    return new_ship_node;
};
