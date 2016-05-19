#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "RectMembrane.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

private:
    ofMesh mesh;
    ofEasyCam cam;
    ofLight pointLight;
    ofLight pointLight2;
    ofMaterial material;
    ofxPanel gui;
    ofSoundPlayer soundPlayer;
    
    ofxIntSlider xMode;
    ofxIntSlider yMode;
    ofxFloatSlider width;
    ofxFloatSlider height;
    ofxFloatSlider xImpactLocation;
    ofxFloatSlider yImpactLocation;
    ofxFloatSlider scale;
    ofxFloatSlider globalDecay;
    ofxFloatSlider materialDecay;
    ofxFloatSlider delta;
    
    ofxToggle enableSuperPosition;
    
    RectMembrane* rect;
};
