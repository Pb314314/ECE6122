/*
Author: Bo Pang
Class:  ECE6122  2023-10-07
Description: This contains the definitions of class: ECE_PointCharge.
*/
#include<iostream>
#include"ECE_PointCharge.h"
using namespace std;
// Constructor
ECE_PointCharge::ECE_PointCharge(double xval,double yval,double zval,double qval):
x(xval), y(yval), z(zval), q(qval) {}

// Set the location for the class
void ECE_PointCharge::setLocation(double x , double y , double z ){
    this->x = x;
    this->y = y;
    this->z = z;
    return;
}
// Set the charge
void ECE_PointCharge::setCharge(double q ){
    this->q = q;
    return;
}