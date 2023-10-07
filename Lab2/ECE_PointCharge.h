/*
Author: Bo Pang
Class:  ECE6122  2023-10-07
Description: This contains the implementation of Lab1.
*/
class ECE_PointCharge{
    protected:
        double x ; // x - coordinate. 
        double y ; // y - coordinate. 
        double z ; // z - coordinate. 
        double q ; // charge of the point. 
    public:
        ECE_PointCharge(double xval,double yval,double zval,double qval);
        void setLocation(double x , double y , double z ); 
        void setCharge(double q ); 
};
