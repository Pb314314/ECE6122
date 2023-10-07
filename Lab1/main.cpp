/*
Author: Bo Pang
Class:  ECE6122  2023-09-23
Description: This contains the implementation of Lab1.
*/
#include <iostream>
#include "ECE_ElectricField.h"
#include "threadpool.h"
#include <thread>
#include <vector>
#include <chrono>
#include <cmath>
#include <sstream>
#include <iomanip>
using namespace std;

// This is the formatScientificNotation of Lab1;
// Input is a double number;
// Output the ScientificNotation of that number;
std::string formatScientificNotation(double number) {
    // Check if the number is zero
    if (number == 0.0) {
        return "0";
    }

    // Convert the number to a string in scientific notation.
    std::ostringstream oss;
    oss << std::scientific << std::setprecision(4) << number;

    // Get the string representation of the number.
    std::string formattedNumber = oss.str();

    // Replace 'e+' with ' * 10^' and 'e-' with ' * 10^-' to format the exponent
    size_t posE = formattedNumber.find("e");
    if (posE != std::string::npos) {
        char sign = formattedNumber[posE + 1]; // Get the sign of the exponent
        formattedNumber.replace(posE, 2, " * 10^" + std::string(1, sign));
    }

    return formattedNumber;
}

// This is the main function
// Ask user about the information and show the output in standard output.
int main(){
    int max_threads = std::thread::hardware_concurrency();
    cout << "Your computer supports " << max_threads << " concurrent threads." << endl; 
    threadpool thread_pool(max_threads);
    //insert the value

    int M ;//= 100;
    int N ;//= 100;

    double x_dis ;//= 0.01;
    double y_dis ;//= 0.03;
    double q ;//= 0.02;

    double x_axis;
    double y_axis;
    double z_axis;

    string input;
    cout << "Please enter the number of rows and columns in the N x M array: ";
    while(1) 
    {
        getline(cin, input);
        istringstream iss(input);
        if (!(iss >> N >> M) || N < 0 || M < 0 || iss.rdbuf()->in_avail() != 0) {
            cout<< "Enter again:";
            continue;
        }
        break;
    }
    cout << "Please enter the x and y separation distances in meters: ";
    while(1) 
    {
        getline(std::cin, input);
        std::istringstream iss(input);
        if (!(iss >> x_dis >> y_dis) || (x_dis < 0.0 || y_dis < 0.0) || iss.rdbuf()->in_avail() != 0)
        {
            cout<< "Enter again:";
            continue;
        }    
        break;
    }
    cout << "Please enter the common charge on the points in micro C: ";
    while(1) 
    {
        getline(std::cin, input);
        istringstream iss(input);
        if (!(iss >> q) || q == 0 ||iss.rdbuf()->in_avail() != 0)
        {
            cout<< "Enter again:";
            continue;
        }
        break;
    }
    cout << endl;
    while(1){
    cout << "Please enter the location in space to determine the electric field ( x y z ) in meters :";
    while(1) 
    {
        getline(cin, input);
        istringstream iss(input);
        if (!(iss >> x_axis >> y_axis >> z_axis) ||iss.rdbuf()->in_avail() != 0)
        {
            //cout<< "Enter again:";
            continue;
        }
        break;
    }
    double x_min = (M%2) ? -(M/2)*(x_dis) :-(M/2-0.5)*x_dis;
    double y_min = (M%2) ? -(M/2)*(y_dis) :-(M/2-0.5)*y_dis;
    double x_loc = x_min;
    double y_loc = y_min;
    double z_loc = 0;
    double Ex_a=0;
    double Ey_a=0;
    double Ez_a=0;
    double E_mag = 0;
    double Ex=0;
    double Ey=0;
    double Ez=0;

    vector<ECE_ElectricField> Fields;
    for(int i = 0;i<M;i++){
        for(int j=0;j<N;j++){
            ECE_ElectricField current(x_loc,y_loc,z_loc,q);
            Fields.emplace_back(current);
            y_loc+=y_dis;
        }
        y_loc = y_min;
        x_loc+=x_dis;
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for(int i=0;i<M*N;i++){
        ECE_ElectricField &current = Fields[i];
        std::function<void()> func = [&current, &x_axis, &y_axis,&z_axis](){
            current.computeFieldAt(x_axis,y_axis,z_axis);// this step takes time.
        };
        thread_pool.add_task(func);
    }

    while (!thread_pool.task_empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Sleep to avoid busy waiting
    }
    
    for(int i=0;i<M*N;i++){
        Fields[i].getElectricField(Ex,Ey,Ez);
        Ex_a+=Ex;
        Ey_a+=Ey;
        Ez_a+=Ez;
    }
    E_mag = sqrt((Ex_a)*(Ex_a) + (Ey_a)*(Ey_a) + (Ez_a)*(Ez_a));
    
    cout<< "The electric field at (" <<x_axis<< ", " <<y_axis<< ", "<< z_axis << " ) in V/m is "<<endl;
    cout<< "Ex = " << formatScientificNotation(Ex_a) <<endl;
    cout<< "Ey = " << formatScientificNotation(Ey_a)<<endl;
    cout<< "Ez = " << formatScientificNotation(Ez_a)<<endl;
    cout<< "|E| = " << formatScientificNotation(E_mag)<<endl;


    //cout<< "final: " << Ex_a<< " "<<Ey_a << " " << Ez_a << E_mag <<endl;
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Time taken by function: " << duration.count() << " milliseconds" << std::endl;
   
    cout<< "Do you want to enter a new location (Y/N)?";
    cin>>input;
    if(input == "y" || input == "Y"){continue;}
    else{
        cout<< "Bye!"<<endl;
        break;
    }    
}
return 0;
}
