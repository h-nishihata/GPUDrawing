#pragma once

#include "ofMain.h"

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
    
    ofShader shader;
    ofVbo vbo;
    ofEasyCam cam;
    
    ofImage img;
    static const int width = 512;
    static const int height = 512;
    static const int numParticles = width * width;

    ofPixels_<unsigned char> pixels;
    ofVec3f myVerts[numParticles];
//    ofVec3f myNormals[numParticles];
    ofFloatColor myColor[numParticles];
    
};
