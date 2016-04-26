//
//  RectMembrane.cpp
//  ModalAnalysisRender
//
//  Created by LiangKevin on 4/16/16.
//
//
#include <omp.h>
#include "RectMembrane.h"
#undef PI
#undef TWO_PI
#include "../lib/stk/header/SineWave.h"

static void calcNormals(ofMesh &mesh) {
    for( int i=0; i < mesh.getVertices().size(); i++ )
    {
        mesh.addNormal(ofPoint(0,0,0));
    }
    
    for( int i=0; i < mesh.getIndices().size(); i+=3 )
    {
        const int ia = mesh.getIndices()[i];
        const int ib = mesh.getIndices()[i+1];
        const int ic = mesh.getIndices()[i+2];
        
        ofVec3f e1 = mesh.getVertices()[ia] - mesh.getVertices()[ib];
        ofVec3f e2 = mesh.getVertices()[ic] - mesh.getVertices()[ib];
        ofVec3f no = -e2.cross( e1 );
        
        // depending on your clockwise / winding order, you might want to reverse the e2 / e1 above if your normals are flipped.
        mesh.getNormals()[ia] += no;
        mesh.getNormals()[ib] += no;
        mesh.getNormals()[ic] += no;
    }
}

void RectMembrane::computeNatrualFreq()
{
    for (int j = 0; j < Nf ; j++)
    {
        for (int i = 0; i < Nf; i++)
        {
            matrixNatralFreq[i + Nf*j] = modeFreq(i+1, j+1);
        }
    }
}

void RectMembrane::computeModalMap()
{
    for (int j = 0; j < Nf; j++)
    {
        for (int i = 0; i < Nf; i++)
        {
            modalMap[i + Nf * j] = 1 / (AMP_CONST * matrixNatralFreq[i + Nf * j]);
        }
    }
}

double RectMembrane::computeQ(double x, double y)
{
    double Q = 0;
    for (int n_x = 1; n_x <= Nf; n_x++)
    {
        for (int n_y = 1; n_y <= Nf; n_y++)
        {
            Q += pow(psi(x, y, n_x, n_y), 2);
        }
    }
    return sqrt(Q);
}

void RectMembrane::computeImpulseResponse(double x, double y, double amp)
{
    double Q = computeQ(x, y);
    for (int i = 0; i < Nf * Nf; i++)
    {
        freqResponse[i] = amp * modalMap[i] * abs(psi(x, y, i % Nf + 1, i / Nf + 1)) / Q;
        printf("freq: %f,  ", matrixNatralFreq[i]);
        printf("resp: %f\n", freqResponse[i] * 1000);
    }
    
}
void RectMembrane::updateMesh(int n_x, int n_y, ofMesh &mesh, double delta, bool pureMode, double mag)
{
    mesh.clear();
    // draw mesh
    double xBase = 0;
    double yBase = 0;
    double delta_x = Lx / NUM_OF_DX;
    double delta_y = Ly / NUM_OF_DY;

    static double tmpGrid[NUM_OF_DX * NUM_OF_DY];
    
    t += delta;
    if (pureMode)
    {
        for (int j = 0; j < NUM_OF_DY; j++)
        {
            for (int i = 0; i < NUM_OF_DX; i++)
            {
                double result = 0;
                result = psi(xBase + i*delta_x, yBase + j*delta_y, n_x, n_y) * cos(2 * M_PI * matrixNatralFreq[n_x-1 + (n_y-1) * Nf] * t);
                mesh.addVertex(ofPoint(xBase + i*delta_x, result, yBase + j*delta_y));
            }
        }
    }
    else
    {
        // clear tempGrid
        for (int i = 0; i < NUM_OF_DX * NUM_OF_DY; i++)
        {
            tmpGrid[i] = 0;
        }
        
        for (int l = 0; l < Nf; l++)
        {
            for (int k = 0; k < Nf; k++)
            {
                const double consine_t = cos(2 * M_PI * matrixNatralFreq[k + l*Nf] * t);
                #pragma omp parallel for
                for (int j = 0; j < NUM_OF_DY; j++)
                {
                    
                    for (int i = 0; i < NUM_OF_DX; i++)
                    {

                        tmpGrid[i + NUM_OF_DX*j] += freqResponse[k+l*Nf]*psi(xBase + i*delta_x, yBase + j*delta_y, k+1, l+1) * consine_t;
                    }
                }
            }
        }
        
        for (int j = 0; j < NUM_OF_DY; j++)
        {
            for (int i = 0; i < NUM_OF_DX; i++)
            {
                mesh.addVertex(ofPoint(xBase + i*delta_x, mag * tmpGrid[i + NUM_OF_DX * j], yBase + j*delta_y));
            }
        }
    }
    
    for (int j= 0; j < NUM_OF_DY - 1; j++)
    {
        for (int i = 0; i < NUM_OF_DX - 1; i++)
        {
            mesh.addIndex(i+j*NUM_OF_DX);               // 0
            mesh.addIndex((i+1)+j*NUM_OF_DX);           // 1
            mesh.addIndex(i+(j+1)*NUM_OF_DX);           // 10
            
            mesh.addIndex((i+1)+j*NUM_OF_DX);           // 1
            mesh.addIndex((i+1)+(j+1)*NUM_OF_DX);       // 11
            mesh.addIndex(i+(j+1)*NUM_OF_DX);           // 10
        }
    }
    calcNormals(mesh);
}
using namespace stk;
void RectMembrane::synthesis()
{
    double fs = 44100;
    Stk::setSampleRate(fs);
    Stk::showWarnings(true);
    
    SineWave sine;
    // reset
    for (int i = 0; i < fs * 2; i++)
    {
        audioSamples[i] = 0;
    }

    printf("computing audio...");
    double max = 0;

    for (int i = 0; i < Nf * Nf; i++)
    {
        if (matrixNatralFreq[i] > fs / 2) continue;
        sine.reset();
        sine.setFrequency(matrixNatralFreq[i]);
        for (int j = 0; j < fs * 2; j++)
        {
            double amp = freqResponse[i];
            double decay = exp(-j/fs/1);
            double material = exp(-j/fs*matrixNatralFreq[i]*M_PI*tan(M_PI/1000));
            audioSamples[j] += decay * amp * material * sine.tick();
            if (max < audioSamples[i])
            {
                max = audioSamples[i];
            }
        }
    }
    printf("done.\n");
    printf("max = %f\n", max);
    for (int i = 0; i < fs * 2; i++)
    {
        audioSamples[i] /= max;
    }
    
//    FileWrite fw;

//    fw.open("/Users/LiangKevin/Desktop/output.wav");
//    fw.write(frames);
//    fw.close();
    

    
    
    
}
