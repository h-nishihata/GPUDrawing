#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(false);
    ofBackground(0);
    ofDisableAlphaBlending();
    
    cam.setupPerspective();
    cam.setPosition(camPosX, camPosY, camPosZ);
    node.setPosition(200, 200, 0);
    
    render.load("shaders/shader");
    updatePos.load("", "shaders/update.frag");
    
    
    img.load("test.jpg");
    pixels = img.getPixels();

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            float r = (float)pixels[j * width*3 + i * 3] / 256.0;
            float g = (float)pixels[j * width*3 + i * 3+1] / 256.0;
            float b = (float)pixels[j * width*3 + i * 3+2] / 256.0;
            float brightness = (r + g + b) / 3.0f;
            
            myVerts[j*width+i] = ofVec3f(i,j,brightness*2560);
            myCoords[j*width+i] = ofVec2f(i,j);
            myColor[j*width+i] = ofFloatColor(r,g,b,1.0);
            
        }
    }
    
    vbo.setVertexData(myVerts, numParticles, GL_DYNAMIC_DRAW);
    vbo.setTexCoordData(myCoords, numParticles, GL_DYNAMIC_DRAW);
    vbo.setColorData(myColor, numParticles, GL_STATIC_DRAW);
    
    
    pingPong.allocate(width, height, GL_RGBA32F, 3);
    
    float * posAndAge = new float[width * height * 4];
//    float * initPos = new float[width * height * 4];
    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            int i = width * y + x;
            posAndAge[i*4 + 0] = x;
            posAndAge[i*4 + 1] = y;
            posAndAge[i*4 + 2] = ofRandom(-1.0, 1.0);
            posAndAge[i*4 + 3] = 0;
        }
    }
    pingPong.src -> getTexture(0).loadData(posAndAge, width, height, GL_RGBA);
//    pingPong.src -> getTexture(2).loadData(initPos, width, height, GL_RGBA);
    delete [] posAndAge;
//    delete [] initPos;
    
    
    float * velAndMaxAge = new float[width * height * 4];
    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            int i = width * y + x;
            velAndMaxAge[i*4 + 0] = 0.0;
            velAndMaxAge[i*4 + 1] = 0.0;
            velAndMaxAge[i*4 + 2] = 0.0;
            velAndMaxAge[i*4 + 3] = ofRandom(1,150);
        }
    }
    
    pingPong.src -> getTexture(1).loadData(velAndMaxAge, width, height, GL_RGBA);
    delete [] velAndMaxAge;
}

//--------------------------------------------------------------
void ofApp::update(){
    float time = ofGetElapsedTimef();
    cam.lookAt(node);
    
    pingPong.dst->begin();
    
        pingPong.dst->activateAllDrawBuffers();
        ofClear(0);
        
        updatePos.begin();
    
            updatePos.setUniformTexture("u_posAndAgeTex", pingPong.src->getTexture(0), 0);
            updatePos.setUniformTexture("u_velAndMaxAgeTex", pingPong.src->getTexture(1), 1);
            updatePos.setUniformTexture("u_initialTex", pingPong.src->getTexture(2), 2);
            updatePos.setUniform1f("u_time", ofGetElapsedTimef());
            updatePos.setUniform1f("u_timestep", 0.5);
            updatePos.setUniform1f("u_scale", 0.5);
            updatePos.setUniform3f("u_emitterPos", emitterPos.x, emitterPos.y, emitterPos.z);
            updatePos.setUniform3f("u_prevEmitterPos", prevEmitterPos.x, prevEmitterPos.y, prevEmitterPos.z);
    
            pingPong.src->draw(0, 0);
        
        
        updatePos.end();
    
    pingPong.dst->end();
    pingPong.swap();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofEnablePointSprites();
    
        cam.begin();
            render.begin();
                render.setUniformTexture("u_posAndAgeTex", pingPong.src->getTexture(0), 0);
                vbo.draw(GL_LINE_LOOP, 0, numParticles);
            render.end();
        cam.end();
    
        ofDisablePointSprites();
    ofPopStyle();
//    ofDrawBitmapString("FPS "+ofToString(ofGetFrameRate(), 0), 20, 40);

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
