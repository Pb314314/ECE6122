/*
Author: Bo Pang
Class:  ECE6122  2023-10-7
Description: This contains the definitions of class: ECE_ElectricField.
*/
#include"ECE_ElectricField.h"
#include<iostream>
#include<cmath>
using namespace std;

ECE_ElectricField::ECE_ElectricField(double xval,double yval,double zval,double qval):
ECE_PointCharge( xval, yval, zval, qval), Ex(0), Ey(0), Ez(0), k(9000000000) {}

// Calculate the Electric field
void ECE_ElectricField::computeFieldAt(double xval, double yval,  double zval){

    double distance = sqrt((xval - x)*(xval-x) + (yval-y)*(yval-y) +(zval-z)*(zval-z));
    double E_mag = k*q/(distance*distance)*1e-6;
    //cout<< "Total field : "<<E_mag <<endl;
    this->Ex = (xval-x)/distance * E_mag;
    this->Ey = (yval-y)/distance * E_mag;
    this->Ez = (zval-z)/distance * E_mag;
    //cout << "Electric field: " << Ex << " "<<Ey <<" "<< Ez <<endl;
    return;
}

// Get the class varaibles
void ECE_ElectricField::getElectricField(double& Ex, double& Ey , double& Ez){
    Ex = this->Ex;
    Ey = this->Ey;
    Ez = this->Ez;
    return;
}