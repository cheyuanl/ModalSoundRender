//
//  MassSpringObj.h
//  ModalAnalysisRender
//
//  Created by LiangKevin on 4/26/16.
//
//
#include "ofxAssimpModelLoader.h"
#include <Eigen/Dense>


#ifndef MassSpringObj_h
#define MassSpringObj_h
using namespace Eigen;
class MassSpringObj
{
private:
    MatrixXf Mass;
    MatrixXf K;
    MatrixXf G;
    MatrixXf Inv_G;
    VectorXcf D;
    
    VectorXcf Wplus;
    VectorXcf Wminus;
    
    complex<double> gamma;
    complex<double> eta;
    double thickness;
    double young_modulus;
    double density;
    int numVertices;
    
    ofxAssimpModelLoader model;
    ofMesh mesh;
    
public:
    MassSpringObj()
    {
    }
    ~MassSpringObj()
    {
    }
    
    void init();        // load objfile to mesh, construct M, K matrix
    void process();     // solve eigen value
    void synthesis(int vertexIndex);   // impulse
    void getMesh(ofMesh& mesh) {mesh = model.getMesh(0);};
    int getNumVertcies(){return numVertices;};
};

#endif /* MassSpringObj_h */
