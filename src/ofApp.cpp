#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(false);
    ofBackground(0);
    ofDisableAlphaBlending();
    
    shader.load("shaders/shader");
    /*
    particles.setMode(OF_PRIMITIVE_POINTS);
    particles.addVertex(ofVec3f(0,0,0));
    particles.addTexCoord(ofVec2f(0,0));
    particles.addColor(ofFloatColor(1.0,0.0,0.0,0.5));
    */
    /*
     mesh.addVertex(ofVec3f(-100,100,0));
     mesh.addVertex(ofVec3f(100,100,0));
     mesh.addVertex(ofVec3f(100,-100,0));
     mesh.addVertex(ofVec3f(-100,-100,0));
     vbo.setMesh(mesh, GL_STATIC_DRAW);
     */
    img.load("test.jpg");
    pixels = img.getPixels();
    
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            float r = (float)pixels[j * width*3 + i * 3] / 256.0;
            float g = (float)pixels[j * width*3 + i * 3+1] / 256.0;
            float b = (float)pixels[j * width*3 + i * 3+2] / 256.0;
            float brightness = (r + g + b) / 3.0f;
            
            myVerts[j*width+i] = ofVec3f(i,j,brightness*256.0);
            myColor[j*width+i] = ofFloatColor(r,g,b,1.0);
        }
    }
    
    vbo.setVertexData(myVerts, numParticles, GL_DYNAMIC_DRAW);
//    vbo.setNormalData(myNormals, numParticles, GL_DYNAMIC_DRAW);
    vbo.setColorData(myColor, numParticles, GL_STATIC_DRAW);

}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofEnablePointSprites();
    
        cam.begin();
            shader.begin();
    
                shader.setUniform1f("time", ofGetElapsedTimef());
//                particles.draw();
//                vbo.draw(GL_QUADS, 0, 4);
                vbo.draw(GL_LINE_LOOP, 0, numParticles);

            shader.end();
        cam.end();
    
        ofDisablePointSprites();
    ofPopStyle();
    ofDrawBitmapString("FPS "+ofToString(ofGetFrameRate(), 0), 20, 40);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
