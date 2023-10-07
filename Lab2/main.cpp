/*
Author: Bo Pang
Class:  ECE6122  2023-10-7
Description: This contains the implementation of Lab2.
*/
#include <iostream>
#include "ECE_ElectricField.h"
#include <omp.h>
#include <thread>
#include <vector>
#include <chrono>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <functional>
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
    
    //insert the value
    int thread_num;

    int M ;//= 100;
    int N ;//= 100;

    double x_dis ;//= 0.01;
    double y_dis ;//= 0.03;
    double q ;//= 0.02;

    double x_axis;
    double y_axis;
    double z_axis;

    string input;
    cout << "Please enter the number of concurrent threads to use: ";
    while(1) 
    {
        getline(cin, input);
        istringstream iss(input);
        if (!(iss >> thread_num) || thread_num <= 0 || iss.rdbuf()->in_avail() != 0) {
            cout<< "Enter again:";
            continue;
        }
        break;
    }
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

    double x_min = (N%2) ? -(N/2)*(x_dis) :-(N/2-0.5)*x_dis;
    double y_min = (M%2) ? -(M/2)*(y_dis) :-(M/2-0.5)*y_dis;
    double x_loc = x_min;
    double y_loc = y_min;
    double z_loc = 0;
    double E_mag = 0;

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
    
    double Ex_a=0;
    double Ey_a=0;
    double Ez_a=0;
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    
    #pragma omp parallel num_threads(thread_num) 
    {
    #pragma omp master
    {
     start_time = std::chrono::high_resolution_clock::now();
    }

    #pragma omp for reduction(+:Ex_a, Ey_a, Ez_a) schedule(static)
    for(int i=0;i<M*N;i++){
        ECE_ElectricField &current = Fields[i];
        //std::function<void()> func = [&current, &x_axis, &y_axis,&z_axis](){
        current.computeFieldAt(x_axis,y_axis,z_axis);// this step takes time.
        Ex_a+=current.Ex;
        Ey_a+=current.Ey;
        Ez_a+=current.Ez;
        //};
    }
    
    #pragma omp master
    {
     end_time = std::chrono::high_resolution_clock::now();
    }
    }
    E_mag = sqrt((Ex_a)*(Ex_a) + (Ey_a)*(Ey_a) + (Ez_a)*(Ez_a));
    
    cout<< "The electric field at (" <<x_axis<< ", " <<y_axis<< ", "<< z_axis << " ) in V/m is "<<endl;
    cout<< "Ex = " << formatScientificNotation(Ex_a) <<endl;
    cout<< "Ey = " << formatScientificNotation(Ey_a)<<endl;
    cout<< "Ez = " << formatScientificNotation(Ez_a)<<endl;
    cout<< "|E| = " << formatScientificNotation(E_mag)<<endl;

    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Time taken by function: " << duration_us.count() << " microseconds" << std::endl;
   
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
