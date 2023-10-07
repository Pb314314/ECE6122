/*
Author: Bo Pang
Class:  ECE6122  2023-09-23
Description: This contains the implementation of Lab1.
*/
#include "ECE_PointCharge.h"

class ECE_ElectricField : public ECE_PointCharge{
    protected:
        double Ex ; // Electric field in the x - direction. 
        double Ey ; // Electric field in the y - direction. 
        double Ez ; // Electric field in the z - direction. 
    public:
        const double k;
        ECE_ElectricField(double xval,double yval,double zval,double qval);
        void computeFieldAt(double xval, double yval,  double zval); 
        void getElectricField(double& Ex, double& Ey , double& Ez); 
};