//#include <cmath>
#include "constant.h"
#include "ofApp.h"
#include "../lib/stk/header/RtWvOut.h"



//--------------------------------------------------------------

void ofApp::setup()
{

    
    gui.setup();
    
    gui.add(width.setup("width (m)", 10, 0.0, 10));
    gui.add(height.setup("height (m)", 10, 0.0, 10));
    gui.add(xMode.setup("xMode (natrual number)", 1, 1, Nf));
    gui.add(yMode.setup("yMode (natrual number)", 1, 1, Nf));
    gui.add(xImpactLocation.setup("impact location x (m)", 0.3, 0.0, 10));
    gui.add(yImpactLocation.setup("impact location y (m)", 0.4, 0.0, 10));
    gui.add(pureMode.setup("pureMode", true));
    gui.add(mag.setup("magnify", 100, 0.0, 10000));
    gui.add(delta.setup("delta t", 0.000001, 0.000000001, 0.001));
    

    ofSetVerticalSync(true);
    
    // set lighting
    ofSetSmoothLighting(true);
    pointLight.setDiffuseColor(ofColor::darkSlateBlue);
    pointLight.setSpecularColor(ofColor::darkSlateBlue);
    pointLight2.setDiffuseColor(ofColor::darkSlateBlue);
    pointLight2.setSpecularColor(ofColor::darkSlateBlue);
    pointLight.setPosition(100, 0, 100);
    pointLight.setPosition(0, 100, 0);
    
    material.setShininess(100);
    material.setSpecularColor(ofFloatColor(0.8, 0.8, 0.8));
    material.setAmbientColor(ofFloatColor(0.3 , 0,3, 0.3));
    material.setEmissiveColor(ofFloatColor(0.3, 0.3, 0.3));
    material.setDiffuseColor(ofFloatColor(0.3, 0.3, 0.3));
    

    // Load rectMembrane
    rect = new RectMembrane(width, height);
    rect->setLxLy(width, height);
    ofLog(OF_LOG_NOTICE, "computing natrual freq matrix...");
    rect->computeNatrualFreq();
    ofLog(OF_LOG_NOTICE, "done");
    ofLog(OF_LOG_NOTICE, "computing Modal Map...");
    rect->computeModalMap();
    ofLog(OF_LOG_NOTICE, "done");
    ofLog(OF_LOG_NOTICE, "computing Impuse Response...");
    rect->computeImpulseResponse(xImpactLocation, yImpactLocation, 1);
    rect->reset();
    rect->synthesis();
    ofLog(OF_LOG_NOTICE, "done");
    if (!model.loadModel("/Users/LiangKevin/Google_Drive/CMU/ModalSoundRender/ModalAnalysisRender/mesh/grid10.obj"))
    {
        printf("load model not succeed\n");
    }
    mesh = model.getMesh(0);
    for (auto v : mesh.getVertices())
    {
        std::cout << "mesh vertex: " <<  v << std::endl;
    }
    
    for (int i = 0; i < mesh.getIndices().size() / 3; i++)
    {
        std::cout << "mesh indices " << ofVec3f(mesh.getIndices()[3 * i], mesh.getIndices()[3 * i + 1], mesh.getIndices()[3 * i + 2]) << std::endl;
    }
    std::cout << "num of vertcies : " << mesh.getVertices().size() << std::endl;
    std::cout << "num of edges : " << mesh.getIndices().size() << std::endl;
    
    

}

//--------------------------------------------------------------
void ofApp::update()
{
    rect->updateMesh(xMode, yMode, mesh, delta, pureMode, mag);

}

//--------------------------------------------------------------
void ofApp::draw()
{

    
    ofBackgroundGradient(ofColor::darkSlateBlue, ofColor::black, OF_GRADIENT_CIRCULAR);

    cam.begin();
    ofEnableDepthTest();
//    model.drawWireframe();

    ofEnableLighting();
    pointLight.enable();
    pointLight2.enable();
    material.begin();
    ofFill();
    
    
    mesh.drawWireframe();
//    mesh.drawFaces();
//    mesh.drawWireframe();
//    model.drawFaces();
    material.end();
    ofDisableDepthTest();
    ofDisableLighting();
    
    cam.end();
    gui.draw();

}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == OF_KEY_RETURN)
    {
        rect->setLxLy(width, height);
        ofLog(OF_LOG_NOTICE, "computing natrual freq matrix...");
        rect->computeNatrualFreq();
        ofLog(OF_LOG_NOTICE, "done");
        ofLog(OF_LOG_NOTICE, "computing Modal Map...");
        rect->computeModalMap();
        ofLog(OF_LOG_NOTICE, "done");
        ofLog(OF_LOG_NOTICE, "computing Impuse Response...");
        rect->computeImpulseResponse(xImpactLocation, yImpactLocation, 1);
        rect->reset();
        rect->synthesis();
        ofLog(OF_LOG_NOTICE, "done");
        
    }
    if (key == OF_KEY_BACKSPACE)
    {
        double fs = 44100;
        ofLog(OF_LOG_NOTICE, "playing...");
        stk::StkFrames frames(fs * 2, 2);
        
        for (int i = 0; i < fs * 2; i++)
        {
            frames[2*i] = rect->audioSamples[i];
            frames[2*i+1] = frames[2*i];
        }
        stk::RtWvOut dac(2);
        dac.tick(frames);
        ofLog(OF_LOG_NOTICE, "done");
        
        
    }
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    rect->setLxLy(width, height);

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


