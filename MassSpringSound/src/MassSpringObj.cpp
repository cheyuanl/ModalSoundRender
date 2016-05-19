//
//  MassSpringObj.cpp
//  MassSpringSound
//
//  Created by LiangKevin on 4/27/16.
//
//

#include "MassSpringObj.h"
#include <algorithm>
#include <Eigen/Eigenvalues>
#undef PI
#undef TWO_PI
#include <stk/SineWave.h>
#include <stk/FileWrite.h>

void MassSpringObj::init()
{
    if (!model.loadModel("/Users/LiangKevin/Google_Drive/CMU/ModalSoundRender/MassSpringSound/mesh/grid10.obj"))
    {
        printf("load model not success\n");
    }
    mesh = model.getMesh(0);
    numVertices = mesh.getVertices().size();
    
    for (int i = 0; i < mesh.getVertices().size(); i++)
        std::cout << "vertex " << mesh.getVertices()[i] << std::endl;
    std::cout << "num of Vertices: " << mesh.getVertices().size() << std::endl;
    // init K, Mass matrix
    MatrixXf tmp = MatrixXf::Zero(3 * mesh.getVertices().size(), 3 * mesh.getVertices().size());
    double m = 6.75 * 1E-8;
    Mass = MatrixXf::Identity(3 * mesh.getVertices().size(), 3 * mesh.getVertices().size());
    Mass *= m;
    
    double k = 2160;
    
    for (int i = 0; i < mesh.getIndices().size() /3; i++)
    {
        std::cout << "index: " << i << std::endl << mesh.getIndices()[3 * i] << ", "
        << mesh.getIndices()[3 * i + 1]  << ", "
        << mesh.getIndices()[3 * i + 2] << std::endl;
        std::vector<int> triInt;
        triInt.push_back(mesh.getIndices()[3 * i]);
        triInt.push_back(mesh.getIndices()[3 * i + 1]);
        triInt.push_back(mesh.getIndices()[3 * i + 2]);
        std::sort(triInt.begin(), triInt.end());
        std::cout << "tri: " << std::endl;
        for (auto j :  triInt)
        {
            std::cout << j << ", ";
        }
        std::cout << std::endl;
        tmp(3 * triInt[0], 3 * triInt[1]) = k;
        tmp(3 * triInt[0] + 1, 3 * triInt[1] + 1) = k;
        tmp(3 * triInt[0] + 2, 3 * triInt[1] + 2) = k;
        
        tmp(3 * triInt[0], 3 * triInt[2]) = k;
        tmp(3 * triInt[0] + 1, 3 * triInt[2] + 1) = k;
        tmp(3 * triInt[0] + 2, 3 * triInt[2] + 2) = k;
        
        tmp(3 * triInt[1], 3 * triInt[2]) = k;
        tmp(3 * triInt[1] + 1, 3 * triInt[2] + 1) = k;
        tmp(3 * triInt[1] + 2, 3 * triInt[2] + 2) = k;
    }
    
    K = tmp + tmp.transpose();
    
// test symmetric
//    for (int i = 0; i < 3 * mesh.getVertices().size(); i++)
//    {
//        for (int j = 0; j < 3 * mesh.getVertices().size(); j++)
//        {
//            if (K(i, j) != K(j, i))
//            {
//                std::cout << K(i, j) << "!=" << K(j, i) << std::endl;
//                getchar();
//            }
//        }
//    }

    std::cout << "num of indicies: " << mesh.getIndices().size() << std::endl;
//    std::cout << "M: " << Mass << std::endl;
    
}

void MassSpringObj::process()
{
    EigenSolver<MatrixXf>eigenSolver(K);
    D = eigenSolver.eigenvalues();
    G = eigenSolver.eigenvectors().real();
    Inv_G = G.inverse();
//    std::cout << G << std::endl;
    std::cout << D << std::endl;
    std::cout << "eigen vectors size: " << G.size() << std::endl;
    std::cout << "eigen value size: " << D.size() << std::endl;
    
    Wplus = VectorXcf(D.size());
    Wminus = VectorXcf(D.size());
    gamma = complex<double>(1E-8, 0);
    eta = complex<double>(1, 0);
    for (int i = 0; i < D.size(); i++)
    {
        complex<double> d = D[i];
        Wplus[i] = (-(gamma * d+eta)+sqrt(pow(gamma*d+eta, 2) - complex<double>(4, 0) * d))/complex<double>(2, 0);
        Wminus[i] = (-(gamma * d+eta)-sqrt(pow(gamma*d+eta, 2) - complex<double>(4, 0) * d))/complex<double>(2, 0);
    }
    std::cout << "Wplus" << std::endl;
    std::cout << Wplus << std::endl;
    std::cout << "Wminus" << std::endl;
    std::cout << Wminus << std::endl;
}

using namespace stk;
void MassSpringObj::synthesis(int vertexIndex)
{
    
    VectorXf f = VectorXf::Zero(numVertices);
    f(vertexIndex) = 1;
    VectorXf g = Inv_G * f;
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
    printf("computing audio...");
    double max = 0;
    for (int t = 0; t < time * fs; t++)
    {
        for (int i = 0; i < Wplus.size(); i++)
        {
            const complex<double> wp = Wplus[i];
            const complex<double> wm = Wminus[i];
            const complex<double> gi = g[i];
//            std::cout << "g: " << gi << std::endl;
            const complex<double> m = Mass(i, i);
            const complex<double> c = gi / m * (wp - wm) * exp(wp * complex<double>(0, 0)); //t0?
            const complex<double> c_star = conj(c);
            audioSamples[i] = (c * wp * exp(wp * complex<double>(t / fs, 0)) + c_star * wm * exp(wm * complex<double>(t / fs, 0))).real();
        }
    }
    
    
    printf("done.\n");
    for (int i = 0; i < fs * time; i++)
    {
        if (max < audioSamples[i])
        {
            max = audioSamples[i];
        }
    }
    // write frame
    for (int i = 0; i < fs * time; i++)
    {
        audioSamples[i] /= max;
        frames[i] = audioSamples[i];
        printf("%f\n", audioSamples[i]);
    }
    
    FileWrite fw;
    fw.open("/Users/LiangKevin/Desktop/output.wav");
    fw.write(frames);
    fw.close();
    delete [] audioSamples;
}