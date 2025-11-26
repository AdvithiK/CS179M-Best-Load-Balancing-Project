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

#include "Problem.h"

using namespace std;
using namespace std::chrono;


int main(){

    //2D array 

    //lol i lied we can't have a 2D array of type object -> we shall make it a vector
    //when populating the vector, we need to make sure its [8][12] strict
    //Container ship[8][12];
    vector<vector<Container>> initial_ship_state(8, vector<Container>(12));

   //vector<vector<Container>> small_ship_state(2, vector<Container>(4));

    //Prompt user to Enter a Manifest

    //take in the file (manifest)

    //variable for the inputting the file
    string input_file;

    //take in file of coordinates as input (use locations.txt as example input file)
    cout << "Enter the name of file: ";
    cin >> input_file;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    //set the file name from commandline to file name variable
    //filename = input_file;

    //open the file + error handling if file can't open
    ifstream file(input_file);

    if(!file.is_open()){
        cout << "Errors occured when opening the file: \"" << input_file << endl;
        
        //second attempt to get correct file name
        cout << "Enter the correct file name. Make sure it has '.txt' at the end." << endl;
        cin >> input_file;
        file.open(input_file);

        if(!file.is_open()){
            cout << "Second attempt to open file failed. Exiting program." << endl;
            return 1;
        }
    }

    //validate the file is open

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

    //populate 2D array to reflect the file's content (96 cells, 8 x 12 object)
    
    //get each row from file to populate each Container object
    string filerow;



    //feed in the values for container object
    int inputfile_y;
    int inputfile_x;
    int inputfile_weight;
    string inputfile_name;

    //get a validation check for 8 rows in here later !! -> ur welcome isabelle <3

    //ISSUE WITH PRINTING, needs to be fixed so I can validate the data (containers) in each ShipNode object is right

    //cause the the y is lwk upside down, top row is row 8 (index 7), and bottom row is 1 (index 0)
    for(int i = 7; i >= 0; i--){
        //there should be 12 objects in every tempVec, then tempVect should get reset
        //vector<Container> tempVec;
        
        for(int j = 0; j < 12; j++){

            //validation check needed here !!
            // function made to read in each line of the file
            //readInFile(file, inputfile_y, inputfile_x, inputfile_weight, inputfile_name);
                    //feed in the [] , {} into this char
            char discard_element;
            

            //take in the [y,x],
            file >> discard_element >> inputfile_y >> discard_element >> inputfile_x >> discard_element >> discard_element;

            //take in the {weight}
            file >> discard_element >> inputfile_weight >> discard_element >> discard_element;
            
            //take in the NAME (getline b/c it can have spaces)
            getline(file, inputfile_name);

            //create container with given info
            Container container_object(inputfile_y, inputfile_x, inputfile_weight, inputfile_name); 

            if(inputfile_name == "UNUSED"){
                container_object.free_spot = true;
            }

            //put the container object into a vector<vector<Container>> shipNode
            //tempVec.push_back(container_object);
            initial_ship_state[i][j] = container_object;
        }
        //append each tempVec to the 2D vector 
        //initial_ship_state.push_back(tempVec);
    }




    //close the file
    file.close();

    /*
    //output validation check for ship state (guys i kinda ate with the output stuff, logfile bout to look firee)
    for (size_t i = 0; i < initial_ship_state.size(); ++i) { 
        for (size_t j = 0; j < initial_ship_state[i].size(); ++j) { 
            cout << initial_ship_state[i][j] << " "; 
        }
        cout << endl;
    }
    */

    //turn the 2D vector object into a Ship Node, and start the algooo
    ShipNode initial_node(initial_ship_state);
    //ShipNode small_initial_node(small_ship_state);
    Problem p(initial_node);

    //find all the containers in the initial ship
    p.findContainers(initial_node);

    //calculate all weights for the ship
    p.calculateShipNode(initial_node);

    // if(p.balanceCheck(initial_node)){
    //     cout << "BALANCED";
    // }
    // else{
    //     cout << "NOT BALANCED";
    // }


    /* ---------------------------------------- TESTINGGGG -------------------------------------------------*/

    //cout << "ur a rat" << endl;

    //output the node to terminal when needed: THIS WORKKSS AHAHAHAHHAHAHAHHHA
    //cout << initial_node;
    //p.findContainers(initial_node);

    //p.calculateShipNode(initial_node);
    //p.printCalculations(initial_node);

    /* ------------------------------------ END OF TESTINGGGG -----------------------------------------------*/




    /* unlock when necessary: writing to output file */

    // string output_file;
    // cout << "Enter output file name: ";
    // cin >> output_file;

    // ofstream out(output_file);
    // if (!out.is_open()) {
    //     cout << "Could not open output file: " << output_file << endl;
    //     return 1;
    // }

    
    // out << initial_node; 

    // out.close();



    
    return 0; 

}
