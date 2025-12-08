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



string outbound_name;

string parseManifest(Problem& p, const string& name, ofstream& log) {
    vector<vector<Container>> initial_ship_state(9, vector<Container>(12));


    //variable for the inputting the file
    string input_file = name;


    //open the file + error handling if file can't open
    ifstream file(input_file+".txt");

    if(!file.is_open()){
        cout << "Errors occured when opening the file: \"" << input_file << endl;
        return "";
    }


    //check if file is empty. If it is, exit program
    if (file.peek() == ifstream::traits_type::eof()) {
        cout << "The file is empty. Exiting program." << endl;
        return "";
    }


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
        
        int row = inputfile_x-1;
        int col = 9 - inputfile_y;

        Container object(col, row, inputfile_weight, container_name ); 
        initial_ship_state[col][row] = object; 

    }

    // adding the crane
    Container object(0, 0, 0, "CRANE"); 
    initial_ship_state[0][0] = object; 

    //close the file
    file.close();

    //turn the 2D vector object into a Ship Node, and start the algooo
    ShipNode initial_node(initial_ship_state);

    //ShipNode small_initial_node(small_ship_state);
    p.initial_ship_state = initial_node;

    string output_file;
    output_file = input_file+"OUTBOUND"+".txt";

    outbound_name = output_file;

    ofstream out(output_file);
    if (!out.is_open()) {
        cout << "Could not open output file: " << output_file+".txt" << endl;
        return "";
    }

    string final_values = p.algo(initial_node, log, input_file+".txt");
    out << p.final_ship_state; 

    out.close();
    

    return final_values;

}

string finalizeLog(ofstream &log_file) {
    time_t now = time(0);
    tm *local = localtime(&now);
    log_file << (local->tm_mon + 1) << " "
            << local->tm_mday << " "
            << (local->tm_year + 1900) << ": "
            << setw(2) << setfill('0') << local->tm_hour << ":"
            << setw(2) << setfill('0') << local->tm_min
            << " Finished a Cycle. Manifest " << outbound_name << 
            " was written to desktop, and a reminder pop-up to operator to send file was displayed." << endl;
     
    return outbound_name;
}


int main(){

    cout.sync_with_stdio(true);
    httplib::Server server;
    server.set_base_dir("./UI"); 
    system("open http://localhost:8080/index.html");
    ShipNode emptyNode;
    Problem p(emptyNode);

    string log_file;
  
    time_t now = time(nullptr);
    tm *local = localtime(&now);

    ostringstream oss;
    oss << "KeoghsPort"
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

    log << (local->tm_mon + 1) << " "
        << local->tm_mday << " "
        << (local->tm_year + 1900) << ": "
        << setw(2) << setfill('0') << local->tm_hour << ":"
        << setw(2) << setfill('0') << local->tm_min
        << " Program was started." << endl;

    server.Post("/run", [&p, &log](const httplib::Request& req, httplib::Response& res){
        string data = parseManifest(p, req.body, log);
        p.setInitialNode();

        if (!data.empty()) {
            res.set_content(data, "text/plain");
        } else {
            res.set_content("ERROR", "text/plain");
        }

        
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    server.Post("/nextmove", [&p, &log](const httplib::Request& req, httplib::Response& res){

        string sentence = p.setUI(log); 

        if (sentence == "DONE") {
            sentence = finalizeLog(log);
        }
        res.set_content(sentence, "text/plain");
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    server.Post("/exit", [&](const httplib::Request& req, httplib::Response& res){
        time_t now = time(0);
        tm *local = localtime(&now);
        log << (local->tm_mon + 1) << " "
        << local->tm_mday << " "
        << (local->tm_year + 1900) << ": "
        << setw(2) << setfill('0') << local->tm_hour << ":"
        << setw(2) << setfill('0') << local->tm_min
        << " Program was shut down.";
        log.close();

        res.set_content("OK", "text/plain");
        res.set_header("Access-Control-Allow-Origin", "*");

        std::thread([&](){ server.stop(); }).detach();
    });

    cout << "Server running on http://localhost:8080/index.html\n";

    server.listen("localhost", 8080);


    
    return 0; 

}
