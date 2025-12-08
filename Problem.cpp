#include "Problem.h"
#include <iostream>


using namespace std;

ostream &operator<<(ostream &output, const ShipNode &node){
    //i starts backwards can row 1 is at the bottom of the node and row 8 is at the top
    for (int i = node.default_ship_state.size() - 1; i > 0; i--) { 
        for (int j = 0; j < node.default_ship_state[i].size(); j++) { 
            output << node.default_ship_state[i][j] << "\n"; 
        }
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

// void Problem::updateContainers(const ShipNode& node){
//     for(Container &box : containers){ 
//         for(int i = node.default_ship_state.size()-1; i >= 0; i--){
//             for(int j = 0; j < node.default_ship_state[i].size(); j++){

//                 if(trim(node.default_ship_state[i][j].name) == trim(box.name)){
//                     //recomputing final_y and final_x
//                     box.final_y = node.default_ship_state.size() - i;
//                     box.final_x = j + 1;
//                     goto next_container;
//                 }
//             }
//         }
//         next_container:;
//     }
// }

void Problem::updatefinalSpots(ShipNode& node){

    // 9
    for (int i = 0; i < node.default_ship_state.size(); i++) { 
        //12
        for (int j = 0; j < node.default_ship_state[i].size(); j++) { 
            node.default_ship_state[i][j].initial_y = j;
            node.default_ship_state[i][j].initial_x = i;
        }
    }

}


void Problem::printContainersList(ShipNode& node){
    cout << "---------- Containers Printing ----------" << endl;
    for(int i = 0; i < containers.size(); i++){
        //testing for spacing
        //cout << "*" << containers.at(i).name << "\n";
        //int index_coord_y = (node.default_ship_state.size() - containers.at(i).final_y );
        //int index_coord_x = containers.at(i).final_x-1;
        cout << endl;
        int index_coord_y = get_y_coord(node, containers.at(i));
        int index_coord_x = get_x_coord(node, containers.at(i));
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
        if(get_x_coord(node, containers.at(i)) < half_ship ){
            node.p_side_weight += containers.at(i).weight;
        }
        else{
            node.s_side_weight += containers.at(i).weight;
        }
    }

    //set the total weight on the ship
    node.sum_weight = node.p_side_weight + node.s_side_weight;
    cout << node.sum_weight << endl;

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
    if (balCheck) cout << "BALANCED"; 
    else cout << "NOT BALANCED";
    cout << endl;
    cout << "The cost (depth) of the ship is: " << node.cost << endl;
    cout << "The heuristic of the ship is: "<< node.heuristic << endl;
    
}

void Problem::balanceCheck(ShipNode& node){
    bool no_zero_s_side = (node.p_side_weight >= node.s_side_weight) && node.s_side_weight != 0;
    bool no_zero_p_side = (node.s_side_weight >= node.p_side_weight) && node.p_side_weight!= 0;
    if(containers.size()<=1){
        cout << "passed thru container size bal check" << endl;
        balCheck = true;
        
    }
    else if(containers.size() == 2 && no_zero_s_side || no_zero_p_side){
            cout << "pass thru 2 container condition bal check" << endl;
            balCheck = true;
    }
    else if(abs(node.p_side_weight - node.s_side_weight) <= (node.sum_weight*0.10)){
        cout << "passed thru formula bal check" << endl;
        cout << "port side weight:" << node.p_side_weight << " sea side weight: " << node.s_side_weight << endl; 
        balCheck = true;
    }

    if (balCheck) {
        final_ship_state = node;
        cout << "BALANCED according to balance check" << endl;
    }
    else cout << "UNBALANCED according to balance check" << endl;

    


}




void Problem::exploreShipNodes(ShipNode& node, Container& box){
    cout << "---------------- EXPLORE FUNCTION: ----------------" << endl;
    vector<pair<int, int>> dest_list = find_dest_list(node, box);
    cout << "exploring " << dest_list.size() <<" nodes" << endl;
    for (const auto& p : dest_list) {
        int index_y = p.first;
        int index_x = p.second;

        cout << "Index_y: " << index_y << endl;
        cout << "Index_x: " << index_x << endl;

        // if (index_x == 8) break;

        int container_y = get_y_coord(node, box);
        int container_x = get_x_coord(node, box);

        cout << "container_y: " << container_y << endl;
        cout << "container_x: " << container_x << endl;

        ShipNode new_node = node;
        new_node.parent = &node;
        if (new_node.parent == nullptr) cout << "HUH\n\n" << endl;

        cout << "NEW COST BEFORE MOVING CONTAINER: " << new_node.cost << endl;

        new_node.cost += abs(container_y-index_y) + abs(container_x - index_x);
        cout << "NEW COST AFTER MOVING CONTAINER: " << new_node.cost << endl;

        swap(new_node.default_ship_state[container_y][container_x], new_node.default_ship_state[index_y][index_x]);


       //getCrane, grab the y and x indexes, swap crane with index_y-1, index_x DO IT ON NEW_NODE !!
        int crane_y = get_y_coord(new_node, getCrane(new_node));
        int crane_x = get_x_coord(new_node, getCrane(new_node));
        swap(new_node.default_ship_state[crane_y][crane_x], new_node.default_ship_state[index_y-1][index_x]);

        calculateShipNode(new_node);
        cout << "PORT side weight:" << new_node.p_side_weight << " STARBOARD weight: " << new_node.s_side_weight << endl; 
    
        //append to unexplored
        unexplored_ship_states.push(new_node);
        cout << "___________________________________________\n" << endl;

    }

};


vector<pair<int,int>> Problem::find_dest_list(const ShipNode& node, Container& box) {
    vector<pair<int,int>> dest_list;
    for(int j = 0; j < 12; j++) {
        for(int i = 8; i > 0; i--) {
            // skip the crane column
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

    cout << "COST BEFORE MOVING CRANE: " << node.cost << endl;
    node.cost += (abs(container_y-crane_y) + abs(container_x-crane_x))-1;
    cout << "COST AFTER MOVING CRANE: " << node.cost << endl;


    swap(node.default_ship_state[crane_y][crane_x], node.default_ship_state[container_y-1][container_x]);
    cout << "MOVED CRANE TO CONTAINER. Crane is now at: " << "(" << get_y_coord(node, getCrane(node)) << ", " << get_x_coord(node, getCrane(node)) << "). name: "<<node.default_ship_state[container_y-1][container_x].name << endl;
};

void Problem::moveCranetoOrigin(ShipNode &node){
    int crane_y = get_y_coord(node, getCrane(node));
    int crane_x = get_x_coord(node, getCrane(node));

    node.cost += (abs(crane_y+crane_x));

    swap(node.default_ship_state[crane_y][crane_x], node.default_ship_state[0][0]);
    cout << "finished moving crane to origin. crane is now at: " << "(" << get_y_coord(node, getCrane(node)) << ", " << get_x_coord(node, getCrane(node)) << "). name: "<<node.default_ship_state[0][0].name << endl;

}

//search algorithm here
void Problem::searchSolutionPath(){
    calculateShipNode(initial_ship_state);
    cout << initial_ship_state.p_side_weight << " " << initial_ship_state.s_side_weight << endl;
    cout << endl;
    cout << "-------------------- SEARCH SOLUTION PATH FUNCTION --------------------" << endl;
    ShipNode curr_node;
    unexplored_ship_states.push(initial_ship_state);
    while(!unexplored_ship_states.empty() && !balCheck){
        
        
        curr_node = unexplored_ship_states.top();
        cout << "COST FOR THE TOP OF THE QUEUE: " << curr_node.cost << endl; 
        unexplored_ship_states.pop();
        balanceCheck(curr_node);
        if(balCheck) {
                cout << "TOP OF PRIORITY QUEUE iS BALANCED" << endl;
                cout << curr_node << endl;
                moveCranetoOrigin(final_ship_state);
                //update the final coordinates for output
                updatefinalSpots(final_ship_state);
                goto exit;
        } 

        // seending each movement to queue
        for (int c = 0; c < containers.size(); c++){
            int index_coord_y = get_y_coord(curr_node, containers.at(c));
            int index_coord_x = get_x_coord(curr_node, containers.at(c));
            cout << "Container Indexes [y,x]: " << index_coord_y << " " << index_coord_x << endl;
            //ensuring container is not going out of bounds
            bool noboundaryUp = index_coord_y > 0;
            //grabbing name of spot above container
            string upspot_name = trim(curr_node.default_ship_state[index_coord_y-1][index_coord_x].name);
            
            //if crane is above container
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
                    crane_moved_node.parent = &curr_node;
                    if (crane_moved_node.parent == nullptr) cout << "HUH\n\n" << endl;
                    moveCranetoContainer(crane_moved_node, containers.at(c));
                    unexplored_ship_states.push(crane_moved_node);
                }

            }
        } 
        
    }
    exit:;
    cout << endl;
    cout << "----------------- SEARCH COMPLETE -----------------" << endl;
    printCalculations(final_ship_state);


    
};

//once final ShipNode is found, this function adds it and all it's ancestors to the final solution stack
void Problem::traceSolutionPath(){
    ShipNode* the_parent = &final_ship_state;
    // solution_path.push(the_parent);
    // the_parent = the_parent->parent;  
    // if (the_parent == nullptr) cout << "YEHAHADHUIFHSDF" << endl;
    // cout << the_parent << endl;
    // solution_path.push(the_parent);
    // the_parent = the_parent->parent;  
    // solution_path.push(the_parent);
    // the_parent = the_parent->parent;  
    // solution_path.push(the_parent);

    while (the_parent != nullptr) {
        
        solution_path.push(the_parent);
        the_parent = the_parent->parent; 
        cout << " OISDFIISJDFIOSDFJIOIODJSJIFJIFDS\n\n\n" << endl; 
    }
    
    cout << "solution_path.size(): " << solution_path.size() << endl;



};


void Problem::algo(ShipNode& node, ofstream& log_file, string filename) {

    time_t now = time(0);
    tm *local = localtime(&now);
    string verb = "are ";

    findContainers(node);
    if (containers.size() == 1) verb = "is ";

    log_file << (local->tm_mon + 1) << " "
        << local->tm_mday << " "
        << (local->tm_year + 1900) << ": "
        << setw(2) << setfill('0') << local->tm_hour << ":"
        << setw(2) << setfill('0') << local->tm_min
        << " Manifest " << filename << " is opened, there " << verb 
        << containers.size() << " container(s) on the ship." << endl;

    searchSolutionPath();
    traceSolutionPath();
   

}

void Problem::alterLog(ofstream& log_file, string comment) {
    time_t now = time(0);
    tm *local = localtime(&now);

    log_file << (local->tm_mon + 1) << " "
        << local->tm_mday << " "
        << (local->tm_year + 1900) << ": "
        << setw(2) << setfill('0') << local->tm_hour << ":"
        << setw(2) << setfill('0') << local->tm_min
        << comment << endl;
}

void Problem::setUI() {
    // for each node in solution_path, reset the json file 
    // enter should go to the next node in solution_path
    // if you hit enter and the json file doesn't get updated ? then return finished solution 
    // screen ?
    
    if (solution_path.size() > 0) {
        ShipNode* curr_node = solution_path.top();
        solution_path.pop();

        json shipData;
        for(int i = 0; i < containers.size(); i++){
            shipData[i]["name"] = containers[i].weight;
            shipData[i]["x"] = get_x_coord(*curr_node, containers.at(i));
            shipData[i]["y"] = get_y_coord(*curr_node, containers.at(i))-1;
            // add a bool to color the active box / crane red
            // shipData[i]["active"] =
        }

        // add crane info 
        Container crane = getCrane(*curr_node);
        shipData[containers.size()]["name"] = crane.name;
        shipData[containers.size()]["x"] = get_x_coord(*curr_node, crane);
        shipData[containers.size()]["y"] = get_y_coord(*curr_node, crane)-1;
        // shipData[containers.size()]["active"] =  

        ofstream file("UI/data.json");
        file << shipData.dump(4); 
        file.close();
    }
    
}
