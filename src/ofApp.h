#pragma once

#include "ofMain.h"
#include "pingPongBuffer.h"
#define numNodes 2
#define numImgs 3
#define numVbos 2

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
        void setNextImage(int imgID);
        float easeInOutQuad (float current, float init = 0, float destination = 100, float duration = 1);
    
    ofShader render, updatePos;
    pingPongBuffer pingPong;    
    
    ofNode node[numNodes];
    
    ofCamera cam;
    float camPosX = 0;
    float camPosY = 0;
    float camPosZ = 512;
    bool xFlag, yFlag, zFlag;
    float camZLength = 600;
    
    ofImage img;
    int imgID;
    
    static const int width = 512;
    static const int height = 512;
    static const int numParticles = width * height;

    ofVbo vbo[numVbos];
    ofPixels_<unsigned char> pixels;
    ofVec3f myVerts[numVbos][numParticles];
    ofVec2f myCoords[numVbos][numParticles];
    ofFloatColor myColor[numVbos][numParticles];
    
    float lifeTime = 30.0;
    int overdose;
    bool imgUpdated;
    bool debugSwapImages;
    float startCount;
    
    bool debugMode;
    
    ofxOscSender sender;
};
