//
//  RectMembrane.h
//  ModalAnalysisRender
//
//  Created by Che-Yuan Liang on 4/9/16.
//
//

#ifndef RECTMEMBRANE_H
#define RECTMEMBRANE_H

#include "constant.h"
#include "ofMain.h"
#include <cmath>

class RectMembrane
{
private:
    double Lx;
    double Ly;
    
    double naturalFreq[Nf * Nf];
    double invNaturalFreq[Nf * Nf];
    double freqResponse[Nf * Nf];
    double materialDecay; // material decay,
    double globalDecay;   // global decay
    double t;
    
    inline double modeFreq(int Nx, int Ny)
    {
        return SPEED_OF_SOUND / 2 * sqrt(pow(Nx/Lx,2)+pow(Ny/Ly,2));
    }
    
    inline double psi(double x, double y, int n_x, int n_y)
    {
        return sin(M_PI * n_x * x / Lx) * sin(M_PI * n_y * y / Ly);
    }

public:
    RectMembrane(double a, double b) :
    Lx(a), Ly(b), t(0)
    {
    }
    
    double computeSumPsiSquare(double x, double y);
    void computeNaturalFreq();
    void computeImpulseResponse(double x, double y);
    
    void updateMesh(int n_x, int n_y, ofMesh &mesh, double delta,
                    double scale, bool enableSuperposition);
    void synthesis();
    
    void resetTime(){t = 0;};
    void setLxLy(float x, float y){Lx = x; Ly = y;};
    void setMaterialDecay(float arg){materialDecay = arg;};
    void setGlobalDecay(float arg){globalDecay = arg;};
};
#endif /* RectMembrane.h */
