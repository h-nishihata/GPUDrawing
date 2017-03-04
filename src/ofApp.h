#pragma once

#include "ofMain.h"
#include "pingPongBuffer.h"

#define numNodes 2

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
    
    ofShader render, updatePos;
    pingPongBuffer pingPong;    
    
    ofNode node[numNodes];
    
    ofCamera cam;
    float camPosX = 0;
    float camPosY = 0;
    float camPosZ = 600;
    bool xFlag, yFlag, zFlag;
    
    ofImage img;
    
    static const int width = 512;
    static const int height = 512;
    static const int numParticles = width * height;

    ofVbo vbo;
    ofPixels_<unsigned char> pixels;
    ofVec3f myVerts[numParticles];
    ofVec2f myCoords[numParticles];
    ofFloatColor myColor[numParticles];
    
    bool showTex;
};
