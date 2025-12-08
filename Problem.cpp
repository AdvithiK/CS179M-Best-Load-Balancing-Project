#include "Problem.h"
#include <iostream>
#include <algorithm>


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
            if(trim(c.name) == trim(box.name) && trim(c.name) != "NAN" &&
               c.weight == box.weight){
                return i;
            } else if(trim(c.name) == trim(box.name) &&
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
            if(trim(c.name) == trim(box.name) && trim(c.name) != "NAN" &&
               c.weight == box.weight){
                return j;
            } else if(trim(c.name) == trim(box.name) &&
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
            if (name != "UNUSED" && name != "NAN" && name != "CRANE"){
                Container temp_container = node.default_ship_state[i][j];
                containers.push_back(temp_container);
            }
        }
    }

};

void Problem::findNAN(const ShipNode& node){
    //loop thru the 2D array and find all NAN, append them to the list (copy for now, prob can be done with pointers)
    for (int i = node.default_ship_state.size() - 1; i >= 0; i--) { 
        for (int j = 0; j < node.default_ship_state[i].size(); j++) { 
            string name = trim(node.default_ship_state[i][j].name);
            if (name == "NAN"){
                Container temp_container = node.default_ship_state[i][j];
                NANcontainers.push_back(temp_container);
            }
        }
    }

};



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
    
        cout << endl;
        int index_coord_y = get_y_coord(node, containers.at(i));
        int index_coord_x = get_x_coord(node, containers.at(i));
        cout << containers.at(i) << endl;
        cout << "index: (" << index_coord_y << ", " << index_coord_x << ")" << endl;
    }
   
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



        int container_y = get_y_coord(node, box);
        int container_x = get_x_coord(node, box);

        cout << "container_y: " << container_y << endl;
        cout << "container_x: " << container_x << endl;

        ShipNode new_node = node;

        new_node.parent = explored_ship_states.size()-1;
        new_node.moving_container = box;

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
    balanceCheck(initial_ship_state);
    ShipNode curr_node;
    cout << initial_ship_state.p_side_weight << " " << initial_ship_state.s_side_weight << endl;
    cout << endl;
    cout << "-------------------- SEARCH SOLUTION PATH FUNCTION --------------------" << endl;
    if (!balCheck) {
        unexplored_ship_states.push(initial_ship_state);
    }
    
    while(!unexplored_ship_states.empty() && !balCheck){
        
        curr_node = unexplored_ship_states.top();
        cout << "POPPED NODE MOVING CONTAINER: " << curr_node.moving_container << endl;
        cout << "COST FOR THE TOP OF THE QUEUE: " << curr_node.cost << endl; 
        unexplored_ship_states.pop();
        explored_ship_states.push_back(curr_node);
        balanceCheck(curr_node);
        if(balCheck) {
                cout << "TOP OF PRIORITY QUEUE iS BALANCED" << endl;
                cout << curr_node << endl;
                //explored_ship_states.push_back(final_ship_state);
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
                    //crane_moved_node.parent = &curr_node;
                    //crane_moved_node.parent = &explored_ship_states.back();
                    crane_moved_node.parent = explored_ship_states.size()-1;
                    crane_moved_node.moving_container = containers.at(c);
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



void Problem::traceSolutionPath(){

    ShipNode temp = final_ship_state; 
    temp.moving_container = final_ship_state.moving_container;
    solution_path.push(final_ship_state);
    solution_path_vec.push_back(final_ship_state);
    //explore.back == final ship state
    solution_path_vec.push_back(explored_ship_states.back());
    //add the final state before crane is moved back
    solution_path.push(explored_ship_states.back());
    //set temp to the next node
    //int i=0;
    while (!(temp == initial_ship_state))
    {
        //cout << "node" << endl;
        temp = explored_ship_states.at(temp.parent);
        updatefinalSpots(temp);
        solution_path.push(temp);
        solution_path_vec.push_back(temp);
        //i++;
    }

    if (final_ship_state.cost == 0) {
        max_steps = 0;
    } else {
        max_steps = solution_path.size()-1;
    }

    reverse(solution_path_vec.begin(), solution_path_vec.end());
    for(int i = 0; i < solution_path_vec.size(); i++){
        cout << solution_path_vec.at(i).cost << endl;
    }

    cout << "solution_path.size(): " << solution_path.size() << endl;




};


string Problem::algo(ShipNode& node, ofstream& log_file, string filename) {

    time_t now = time(0);
    tm *local = localtime(&now);
    string verb = "are ";

    findContainers(node);
    findNAN(node);
    cout << "NAN size: " << NANcontainers.size() << endl;
    if (containers.size() == 1) verb = "is ";

    log_file << (local->tm_mon + 1) << " "
        << local->tm_mday << " "
        << (local->tm_year + 1900) << ": "
        << setw(2) << setfill('0') << local->tm_hour << ":"
        << setw(2) << setfill('0') << local->tm_min
        << " Manifest " << filename << " is opened, there " << verb 
        << containers.size() << " container(s) on the ship." << endl;

    searchSolutionPath();
    //loads the solution path stack with the trace to final solution
    traceSolutionPath();
    int total_cost = final_ship_state.cost;

    log_file << (local->tm_mon + 1) << " "
        << local->tm_mday << " "
        << (local->tm_year + 1900) << ": "
        << setw(2) << setfill('0') << local->tm_hour << ":"
        << setw(2) << setfill('0') << local->tm_min
        << " Balance Solution found, it will require " << max_steps 
        << " moves/" << total_cost << " minutes." << endl;
    

    return to_string(max_steps) + "," + to_string(total_cost);
   

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

void Problem::reportCommenttoLog(ofstream& log_file, string comment){
    time_t now = time(0);
    tm *local = localtime(&now);

    log_file << (local->tm_mon + 1) << " "
        << local->tm_mday << " "
        << (local->tm_year + 1900) << ": "
        << setw(2) << setfill('0') << local->tm_hour << ":"
        << setw(2) << setfill('0') << local->tm_min
        << "A comment was written to the log \"" << comment << "\""<< endl;

   
}

void Problem::setInitialNode() {
    ShipNode curr_node = solution_path.top();
    solution_path.pop();

    //formatting the info for 
    json shipData;
    for(int i = 0; i < containers.size(); i++){
        shipData[i]["name"] = containers.at(i).name;
        shipData[i]["x"] = get_x_coord(curr_node, containers.at(i));
        shipData[i]["y"] = get_y_coord(curr_node, containers.at(i));
    }

    for(int i = 0; i < NANcontainers.size(); i++){
        shipData[i+containers.size()]["name"] = "NAN";
        shipData[i+containers.size()]["x"] = get_x_coord(curr_node, NANcontainers.at(i));
        shipData[i+containers.size()]["y"] = get_y_coord(curr_node, NANcontainers.at(i));
    }

    // add crane info 
    Container crane = getCrane(curr_node);
    shipData[containers.size()+NANcontainers.size()]["name"] = crane.name;
    shipData[containers.size()+NANcontainers.size()]["x"] = 1;
    shipData[containers.size()+NANcontainers.size()]["y"] = 0;

    ofstream file("UI/data.json");
    file << shipData.dump(4); 
    file.close();

}

string Problem::setUI(ofstream& log_file) {
    // for each node in solution_path, reset the json file 
    // enter should go to the next node in solution_path
    // if you hit enter and the json file doesn't get updated ? then return finished solution 
    // screen ?

    cout << "SETTING UI" << endl;
    string log_sentence;
    //set time stamp for log file
    time_t now = time(0);
    tm *local = localtime(&now);
    
    if (solution_path.size() > 0) {
        ShipNode curr_node = solution_path.top();
        if (curr_node == final_ship_state) cout <<"IOSJDFIOSDFIJO"<< endl;
        
        solution_path.pop();

        //formatting the info for 
        json shipData;
        for(int i = 0; i < containers.size(); i++){
            shipData[i]["name"] = containers.at(i).name;
            shipData[i]["x"] = get_x_coord(curr_node, containers.at(i));
            shipData[i]["y"] = get_y_coord(curr_node, containers.at(i));
        }

        for(int i = 0; i < NANcontainers.size(); i++){
            shipData[i+containers.size()]["name"] = "NAN";
            shipData[i+containers.size()]["x"] = get_x_coord(curr_node, NANcontainers.at(i));
            shipData[i+containers.size()]["y"] = get_y_coord(curr_node, NANcontainers.at(i));
        }

        // add crane info 
        Container crane = getCrane(curr_node);
        shipData[containers.size()+NANcontainers.size()]["name"] = crane.name;
        shipData[containers.size()+NANcontainers.size()]["x"] = get_x_coord(curr_node, crane);
        shipData[containers.size()+NANcontainers.size()]["y"] = get_y_coord(curr_node, crane);

        //store which step we are at based on solution_path size
        int step = max_steps - solution_path.size();
    
        //if this is the first log file statement, print move crane to container
        if (step == 1){
            int final_y_grid = 9 - get_y_coord(curr_node, curr_node.moving_container);
            int final_x_grid = get_x_coord(curr_node, curr_node.moving_container)+1;
            log_file << (local->tm_mon + 1) << " "
                << local->tm_mday << " "
                << (local->tm_year + 1900) << ": "
                << setw(2) << setfill('0') << local->tm_hour << ":"
                << setw(2) << setfill('0') << local->tm_min
                << " " << step << " out of " << max_steps << ": Move from PARK to [" 
                << final_y_grid << ", " << 
                final_x_grid << "], " << 
                curr_node.cost << " minutes." << endl;

            log_sentence = to_string(step) + " out of " + to_string(max_steps) + ": Move from PARK to [" 
                + to_string(final_y_grid) +", " + 
                to_string(final_x_grid) + "], " + 
                to_string(curr_node.cost) + " minutes.\n";
        }
        else if(step < max_steps){

            //otherwise, default step print:
            //grab the next value and store it
            //cout << "SOL NODE SIZE: " << solution_path.size() << endl;
            ShipNode prev_node = solution_path_vec.at(step-1);
            cout <<"CURR NODE COST: " << curr_node.cost << endl;
            cout << "PREV NODE COST: "<< prev_node.cost << endl;
            //restore curr node after grabbing next value
            int final_y_grid = 9 - get_y_coord(curr_node, curr_node.moving_container);
            int final_x_grid = get_x_coord(curr_node, curr_node.moving_container)+1;

            int final_prev_y_grid = 9 - get_y_coord(prev_node, prev_node.moving_container);
            int final_prev_x_grid = get_x_coord(prev_node, prev_node.moving_container)+1;


            log_file << (local->tm_mon + 1) << " "
                << local->tm_mday << " "
                << (local->tm_year + 1900) << ": "
                << setw(2) << setfill('0') << local->tm_hour << ":"
                << setw(2) << setfill('0') << local->tm_min
                << " " << step << " out of " << max_steps << ": Move container in [" 
                << final_prev_y_grid << ", " << 
                final_prev_x_grid << "] to [" << 
                final_y_grid << ", " << 
                final_x_grid << "], "<<
                curr_node.cost << " minutes." << endl;

            log_sentence = to_string(step) + " out of " + to_string(max_steps) + ": Move container in [" 
                + to_string(final_prev_y_grid) + ", " + 
                to_string(final_prev_x_grid) + "] to [" + 
                to_string(final_y_grid) + ", " + 
                to_string(final_x_grid) + "], "+
                to_string(curr_node.cost) + " minutes.\n";
            
        }
        else if (step == max_steps){
            int final_y_grid = 9 - get_y_coord(curr_node, curr_node.moving_container);
            int final_x_grid = get_x_coord(curr_node, curr_node.moving_container)+1;
            //if we are at the final ship state
            log_file << (local->tm_mon + 1) << " "
                << local->tm_mday << " "
                << (local->tm_year + 1900) << ": "
                << setw(2) << setfill('0') << local->tm_hour << ":"
                << setw(2) << setfill('0') << local->tm_min
                << " " << step << " out of " << max_steps << ": Move from [" 
                << final_y_grid << ", " << 
                final_x_grid << "] to PARK, " << 
                curr_node.cost << " minutes." << endl;

            log_sentence = to_string(step) + " out of " + to_string(max_steps) + ": Move from [" 
                + to_string(final_y_grid) + ", " + 
                to_string(final_x_grid) + "] to PARK, " +
                to_string(curr_node.cost) + " minutes.\n";

            
        }

        ofstream file("UI/data.json");
        file << shipData.dump(4); 
        file.close();

        
    } else {
        
        log_sentence = "DONE";
    }

    return log_sentence;

}


