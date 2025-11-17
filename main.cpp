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

using namespace std;
using namespace std::chrono;

int main(){

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
    //convert elements to numeric ?? <- not sure if this will make it easier 


    //close the file
    file.close();





    
    return 0; 

}