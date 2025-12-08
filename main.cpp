#include <ctime>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <float.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib> 
#include <thread>
#include "httplib.h"


#include "Problem.h"

using namespace std;
using namespace std::chrono;

//helper function to trim spacing for name
// string trim(const string& s) {
//     size_t start = s.find_first_not_of(" \t\r\n");
//     size_t end = s.find_last_not_of(" \t\r\n");
//     if (start == string::npos) return "";
//     return s.substr(start, end - start + 1);
// }

bool parseManifest(const string& name) {
    string log_file;
  
    time_t now = time(nullptr);
    tm *local = localtime(&now);

    ostringstream oss;
    oss << "KeoughsPort"
        << (local->tm_mon + 1) << "_"
        << local->tm_mday << "_"
        << (local->tm_year + 1900) << "_"
        << setw(2) << setfill('0') << local->tm_hour
        << setw(2) << setfill('0') << local->tm_min
        << ".txt";

    log_file = oss.str();

    ofstream log(log_file);
    if (!log.is_open()) {
        cout << "Could not open output file: " << log_file+".txt" << endl;
    }

    // // write: 10 18 2023: 01:14 Program was started.
    log << (local->tm_mon + 1) << " "
        << local->tm_mday << " "
        << (local->tm_year + 1900) << ": "
        << setw(2) << setfill('0') << local->tm_hour << ":"
        << setw(2) << setfill('0') << local->tm_min
        << " Program was started." << endl;
 

    //2D array 

    //when populating the vector, we need to make sure its [8][12] strict (9 x 12 for air)
    //Container ship[8][12];
    // 9 rows 12 columns, max 9 x and max 12 y
    vector<vector<Container>> initial_ship_state(9, vector<Container>(12));


    //Prompt user to Enter a Manifest

    //take in the file (manifest)

    //variable for the inputting the file
    string input_file = name;

    //take in file of coordinates as input (use locations.txt as example input file)
    // cin.ignore(numeric_limits<streamsize>::max(), '\n');
    //set the file name from commandline to file name variable
    //filename = input_file;

    //open the file + error handling if file can't open
    ifstream file(input_file+".txt");

    if(!file.is_open()){
        cout << "Errors occured when opening the file: \"" << input_file << endl;
        return 1;
    }


    //check if file is empty. If it is, exit program
    if (file.peek() == ifstream::traits_type::eof()) {
        cout << "The file is empty. Exiting program." << endl;
        return 1;
    }

    /* example file look

    [01,01], {00000}, NAN
    [01,02], {00000}, UNUSED
    [01,03], {00101}, Fish for Wendys
    [01,04], {00000}, UNUSED
    [01,05], {00000}, UNUSED
    [01,06], {00040}, Bikes
    [01,07], {00000}, UNUSED
    [01,08], {00000}, UNUSED
    [01,09], {00000}, UNUSED
    [01,10], {00000}, UNUSED
    [01,11], {00000}, UNUSED
    [01,12], {00000}, NAN
    .... remainder is the unused space
    
    */


    string filerow;
    string container_name;
    int inputfile_y;
    int inputfile_x;
    int inputfile_weight;
    string inputfile_name;
    while (getline(file, filerow)) {
        char discard_element;
        stringstream ss(filerow);

        ss >> discard_element >> inputfile_y >> discard_element >> inputfile_x >> discard_element >> discard_element;
        ss >> discard_element >> inputfile_weight >> discard_element >> discard_element;
        
        getline(ss, container_name);   
        
        // cout << "[" << inputfile_y << "," << inputfile_x << "], {" << inputfile_weight << "} " << container_name << endl;
        // [x,y]:[1,1] is actually [8,0]
        // row is y and col is x
        int row = inputfile_x-1;
        int col = 9 - inputfile_y;

        // cout << "[row,col]:" << "[" << row << "," << col << "]" << endl;

        Container object(col, row, inputfile_weight, container_name ); 
        initial_ship_state[col][row] = object; 

    }

    // adding the crane
    Container object(0, 0, 0, "CRANE"); 
    initial_ship_state[0][0] = object; 

    //close the file
    file.close();


    // start up the server & open it up
    // system("python3 -m http.server 5050 --directory UI &");
    // system("open http://localhost:5050/index.html");

    // use this for checking if it does the auto update (can remove later)

    //turn the 2D vector object into a Ship Node, and start the algooo
    ShipNode initial_node(initial_ship_state);

    //ShipNode small_initial_node(small_ship_state);
    Problem p(initial_node);

    string output_file;
    output_file = input_file+"OUTBOUND"+".txt";

    ofstream out(output_file);
    if (!out.is_open()) {
        cout << "Could not open output file: " << output_file+".txt" << endl;
        return 1;
    }

    // run the algorithm
    // p.algo(initial_node, log, input_file+".txt");
    // out << p.final_ship_state; 

    // out.close();
    // log.close();

}

int main(){

    // start up the server
    // register the value that gets into the test file
    // run the parsing
    cout.sync_with_stdio(true);
    httplib::Server server;


    server.Post("/run", [](const httplib::Request& req, httplib::Response& res){
        cout << "POST received with body: " << req.body << std::endl;
        cout.flush();

        bool success = parseManifest(req.body);

        cout << "parseManifest returned: " << success << std::endl;
        cout.flush();

        res.set_content("OK", "text/plain");
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    cout << "Server running on http://localhost:5050\n";
    server.listen("localhost", 5050);


    
    
    return 0; 

}
