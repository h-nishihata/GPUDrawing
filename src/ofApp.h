#pragma once

#include "ofMain.h"
#include "pingPongBuffer.h"
#define numNodes 2
#define numImgs 5

#include "ofxOsc.h"
#define HOST "localhost"
#define PORT 1234


class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void setInitImage();
    void setNextImage();
    float easeInOutQuad (float current, float init = 0, float destination = 100, float duration = 1);
    
    ofShader render, updatePos;
    pingPongBuffer pingPong;
    
    ofFbo renderFBO;
    
    ofNode node[numNodes];
    ofCamera cam;
    
    ofImage img;
    int imgID;
    
    static const int width = 1080;
    static const int height = 1080;
    static const int numParticles = width * height;
    
    ofVbo vbo;
    ofPixels_<unsigned char> pixels;
    ofVec3f myVerts[numParticles];
    ofVec2f myCoords[numParticles];
    ofFloatColor myColor[numParticles];
    
    int lifeTime = 30;
    int overdose;
    float startCount;
    float morphingDuration = 5.0;
    bool isMorphing;
    bool imgUpdated;
    
    ofxOscSender sender;
    bool oscSent0, oscSent1;
    bool debugMode;
};
