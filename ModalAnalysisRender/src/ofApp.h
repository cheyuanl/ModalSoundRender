#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"

#undef PI
#undef TWO_PI
#include "../lib/stk/header/SineWave.h"
#include "MassSpringObj.h"
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

    
    ofxIntSlider xMode;
    ofxIntSlider yMode;
    ofxFloatSlider width;
    ofxFloatSlider height;
    ofxFloatSlider xImpactLocation;
    ofxFloatSlider yImpactLocation;
    ofxFloatSlider mag;
    ofxFloatSlider delta;
    ofxToggle pureMode;
    
    
    RectMembrane* rect;
    ofxAssimpModelLoader model;
    MassSpringObj msObj;

    

		
};
