#include "ofApp.h"
#include <boost/filesystem.hpp>
//--------------------------------------------------------------
/*
 */

void ofApp::setup()
{
    // setup gui
    gui.setup();
    gui.add(width.setup("width (m) ", 10, 0.0, 10));
    gui.add(height.setup("height (m) ", 10, 0.0, 10));
    gui.add(xMode.setup("Mode x ", 1, 1, Nf));
    gui.add(yMode.setup("Mode y ", 1, 1, Nf));
    gui.add(enableSuperPosition.setup("enable superposition", true));
    gui.add(xImpactLocation.setup("impact x (m)", 3, 0.0, 10));
    gui.add(yImpactLocation.setup("impact y (m)", 4, 0.0, 10));
    gui.add(scale.setup("scale", 200, 0.0, 400));
    gui.add(materialDecay.setup("material damping", M_PI/1000, 1/100000, M_PI/32));
    gui.add(globalDecay.setup("global damping", 1, 0.001, 10));
    gui.add(delta.setup("delta t (sec)", 1E-5, 1E-6, 1E-2));
    
    // setup lighting
    ofSetVerticalSync(true);
    ofSetSmoothLighting(true);
    pointLight.setDiffuseColor(ofColor::darkSlateBlue);
    pointLight.setSpecularColor(ofColor::darkSlateBlue);
    pointLight2.setDiffuseColor(ofColor::darkSlateBlue);
    pointLight2.setSpecularColor(ofColor::darkSlateBlue);
    pointLight.setPosition(100, 0, 100);
    pointLight.setPosition(0, 100, 0);
    
    // setup material
    material.setShininess(10);
    material.setSpecularColor(ofFloatColor(0.8, 0.8, 0.8));
    material.setAmbientColor(ofFloatColor(0.6 , 0,6, 0.6));
    material.setEmissiveColor(ofFloatColor(0.3, 0.3, 0.3));
    material.setDiffuseColor(ofFloatColor(0.3, 0.3, 0.3));

    // load rectMembrane
    rect = new RectMembrane(width, height);
    rect->resetTime();
    rect->setLxLy(width, height);
    ofLog(OF_LOG_NOTICE, "computing natrual freq matrix...");
    rect->computeNaturalFreq();
    ofLog(OF_LOG_NOTICE, "done");
    ofLog(OF_LOG_NOTICE, "computing impuse response...");
    rect->computeImpulseResponse(xImpactLocation, yImpactLocation);
    rect->updateMesh(xMode, yMode, mesh, delta, scale, enableSuperPosition);
    rect->synthesis();
    ofLog(OF_LOG_NOTICE, "done");
}

//--------------------------------------------------------------
void ofApp::update()
{
    rect->setLxLy(width, height);
    rect->setMaterialDecay(materialDecay);
    rect->setGlobalDecay(globalDecay);
    rect->updateMesh(xMode, yMode, mesh, delta, scale, enableSuperPosition);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackgroundGradient(ofColor::darkSlateBlue, ofColor::black, OF_GRADIENT_CIRCULAR);

    cam.begin();
    ofEnableDepthTest();
    ofEnableLighting();
    
    pointLight.enable();
    pointLight2.enable();
    
    material.begin();
    mesh.drawWireframe();
    material.end();
    
    ofDisableLighting();
    ofDisableDepthTest();
    cam.end();
    
    gui.draw();

}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == OF_KEY_RETURN)
    {
        xImpactLocation.setMax(width);
        yImpactLocation.setMax(height);
        ofLog(OF_LOG_NOTICE, "computing natrual freq matrix...");
        rect->computeNaturalFreq();
        ofLog(OF_LOG_NOTICE, "done");
        ofLog(OF_LOG_NOTICE, "computing impulse response...");
        rect->computeImpulseResponse(xImpactLocation, yImpactLocation);
        rect->resetTime();
        rect->synthesis();
        ofLog(OF_LOG_NOTICE, "done");
        xImpactLocation = width / 2;
        yImpactLocation = height / 2;
    }
    
    if (key == OF_KEY_BACKSPACE)
    {
        std::cout << boost::filesystem::current_path().string() << std::endl;
        soundPlayer.load(boost::filesystem::current_path().string() + "/../../../data/audiosample.wav");
        ofLog(OF_LOG_NOTICE, "playing...");
        soundPlayer.play();
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


