//
//  RectMembrane.cpp
//  ModalAnalysisRender
//
//  Created by Che-Yuan Liang on 4/16/16.
//
//

#include "RectMembrane.h"
#include <libiomp/omp.h>
#include <boost/filesystem.hpp>
#undef PI
#undef TWO_PI
#include <stk/SineWave.h>
#include <stk/FileWrite.h>

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
        
        mesh.getNormals()[ia] += no;
        mesh.getNormals()[ib] += no;
        mesh.getNormals()[ic] += no;
    }
}

void RectMembrane::computeNaturalFreq()
{
    #pragma omp parallel
    for (int j = 0; j < Nf ; j++)
    {
        for (int i = 0; i < Nf; i++)
        {
            naturalFreq[i + j*Nf] = modeFreq(i+1, j+1);
            invNaturalFreq[i + j*Nf] = 1 / naturalFreq[i + j*Nf];
        }
    }
}

double RectMembrane::computeSumPsiSquare(double x, double y)
{
    double sumPsiSquare = 0;
    for (int n_x = 1; n_x <= Nf; n_x++)
    {
        for (int n_y = 1; n_y <= Nf; n_y++)
        {
            sumPsiSquare += pow(psi(x, y, n_x, n_y), 2);
        }
    }
    return sqrt(sumPsiSquare);
}

void RectMembrane::computeImpulseResponse(double x, double y)
{
    double Q = computeSumPsiSquare(x, y);
    #pragma omp parallel for schedule(static, 16)
    for (int i = 0; i < Nf * Nf; i++)
    {
        freqResponse[i] = invNaturalFreq[i] * psi(x, y, i % Nf + 1, i / Nf + 1) / Q;
    }
    
}
void RectMembrane::updateMesh(int n_x, int n_y, ofMesh &mesh, double delta,
                              double scale, bool enableSuperposition)
{
    mesh.clear();

    double delta_x = Lx / NUM_OF_DX;
    double delta_y = Ly / NUM_OF_DY;

    static double grid[NUM_OF_DX * NUM_OF_DY];
    double tan_phi = tan(materialDecay);
    
    t += delta;
    double damp_t = exp(-t / globalDecay);
    
    if (enableSuperposition)
    {
        for (int j = 0; j < NUM_OF_DY; j++)
        {
            for (int i = 0; i < NUM_OF_DX; i++)
            {
                double result = 0;
                result = scale / 40 * psi(i*delta_x, j*delta_y, n_x, n_y) * cos(2 * M_PI * naturalFreq[n_x-1 + (n_y-1) * Nf] * t);
                mesh.addVertex(ofPoint(i*delta_x, result, j*delta_y));
            }
        }
    }
    else
    {
        // clear grid
        for (int i = 0; i < NUM_OF_DX * NUM_OF_DY; i++)
        {
            grid[i] = 0;
        }

        for (int l = 0; l < Nf; l++)
        {
            for (int k = 0; k < Nf; k++)
            {
                double consine_t = cos(2 * M_PI * naturalFreq[k + l*Nf] * t);
                double materialDamp_t = exp(-t * naturalFreq[k + l*NUM_OF_DX] * M_PI * tan_phi);
                double fr = freqResponse[k+l*Nf];
                #pragma omp parallel for
                for (int i = 0; i < NUM_OF_DX * NUM_OF_DX; i++)
                {
                    grid[i] += damp_t * materialDamp_t * fr *psi(i%NUM_OF_DX*delta_x, i/NUM_OF_DX*delta_y, k+1, l+1) * consine_t;
                }
            }
        }
        
        for (int j = 0; j < NUM_OF_DY; j++)
        {
            for (int i = 0; i < NUM_OF_DX; i++)
            {
                mesh.addVertex(ofPoint(i*delta_x, scale * grid[i + j*NUM_OF_DX], j*delta_y));
            }
        }
    }
    
    for (int j = 0; j < NUM_OF_DY - 1; j++)
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
    double time = 5;
    double* audioSamples = new double [(int)fs * (int)time];
    Stk::setSampleRate(fs);
    Stk::showWarnings(true);
    SineWave sine;
    stk::StkFrames frames(fs * time, 1);
    
    // reset audio sample
    for (int i = 0; i < fs * time; i++)
    {
        audioSamples[i] = 0;
    }
    
    double max = 0;
    double tan_phi = tan(materialDecay);
    
    for (int i = 0; i < Nf * Nf; i++)
    {
        if (naturalFreq[i] > fs / 2) continue; // anti-aliasing
        sine.reset();
        sine.setFrequency(naturalFreq[i]);

        for (int j = 0; j < fs * time; j++)
        {
            double amp = freqResponse[i];
            double decay = exp(-j/fs/globalDecay);
            double material = exp(-j/fs*naturalFreq[i]*M_PI*tan_phi);
            audioSamples[j] += decay * amp * material * sine.tick();
        }
    }
    for (int i = 0; i < fs * time; i++)
    {
        double absSample = abs(audioSamples[i]);
        if (max < absSample)
        {
            max = absSample;
        }
    }
    // write frame
    for (int i = 0; i < fs * time; i++)
    {
        audioSamples[i] /= max;
        frames[i] = audioSamples[i];
    }
    
    FileWrite fw;
    fw.open(boost::filesystem::current_path().string() + "/../../../data/audiosample.wav");
    fw.write(frames);
    fw.close();
    delete [] audioSamples;
}
