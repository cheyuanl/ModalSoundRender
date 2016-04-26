//
//  RectMembrane.h
//  ModalAnalysisRender
//
//  Created by LiangKevin on 4/9/16.
//
//

#ifndef RECTMEMBRANE_H
#define RECTMEMBRANE_H

#include <cmath>
#include <utility>
#include <vector>
#include "ofMain.h"

#include "constant.h"

class RectMembrane
{
private:
    double Lx;
    double Ly;
    
    double matrixNatralFreq[Nf * Nf];
    double modalMap[Nf * Nf];
    double freqResponse[2 * Nf * Nf];
    double material;
    double t;
    
    inline double modeFreq(int Nx, int Ny)
    {
        return SPEED_OF_SOUND / 2 * sqrt(pow(Nx/Lx,2)+pow(Ny/Ly,2));
    }

    
public:
    double audioSamples[44100*2];
    RectMembrane(double a, double b) :
    Lx(a), Ly(b), t(0)
    {
        ;
    }
    
    inline double psi(double x, double y, int n_x, int n_y)
    {
        return sin(M_PI * n_x * x / Lx) * sin(M_PI * n_y * y / Ly);
    }
    
    void computeNatrualFreq();
    void computeModalMap();
    void updateMesh(int n_x, int n_y, ofMesh &mesh, double delta, bool pureMode, double mag);
    void computeImpulseResponse(double x, double y, double amp);
    double computeQ(double x, double y);
    void synthesis();
    void reset(){t = 0;};
    
    void setLxLy(float x, float y){Lx = x; Ly = y;};


};
#endif /* RectMembrane.h */
