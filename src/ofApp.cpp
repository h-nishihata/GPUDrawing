#include "ofApp.h"

float* currPos;
float* currCol;
float* velocity;
float* nextCol;
float* nextPos;


void ofApp::setInitImage(){
    img.load("specter_00.png");
    pixels = img.getPixels();
    
    currPos = new float[width*height*4];
    currCol = new float[width*height*4];
    velocity = new float[width*height*4];
    
    int offsetX = width * 0.5;
    int offsetY = height * 0.5;
    
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            float r = (float)1.0-(pixels[j*width*4+i*4+0] * 0.0039);
            float g = (float)0.6-(pixels[j*width*4+i*4+1] * 0.0039);
            float b = (float)0.3-(pixels[j*width*4+i*4+2] * 0.0039);
            float brightness = (r+g+b) * 0.3333;
            
            if ((pixels[j*width*4+i*4+0] > 200) &&
                (pixels[j*width*4+i*4+1] > 90)){
                r = g = b = 0;
            }
            myCoords[j*width+i] = ofVec2f(i, j);
            myVerts[j*width+i] = ofVec3f(i, j, brightness * 256.0);
            myColor[j*width+i] = ofFloatColor(r, g, b, 1.0);
            
            currPos[j*width*4+i*4+0] = i - offsetX;
            currPos[j*width*4+i*4+1] = j - offsetY;
            currPos[j*width*4+i*4+2] = brightness * 256.0;
            currPos[j*width*4+i*4+3] = 1.0;
            
            currCol[j*width*4+i*4+0] = r;
            currCol[j*width*4+i*4+1] = g;
            currCol[j*width*4+i*4+2] = b;
            currCol[j*width*4+i*4+3] = 1.0;
            
            velocity[j*width*4+i*4+0] = 0.0;
            velocity[j*width*4+i*4+1] = 0.0;
            velocity[j*width*4+i*4+2] = 0.0;
            velocity[j*width*4+i*4+3] = 1.0;
        }
    }
    
    vbo.setTexCoordData(myCoords, numParticles, GL_DYNAMIC_DRAW);
    vbo.setVertexData(&myVerts[0], numParticles, GL_DYNAMIC_DRAW);
    vbo.setColorData(myColor, numParticles, GL_STATIC_DRAW);
    
    pingPong.src -> getTexture(0).loadData(currPos, width, height, GL_RGBA);
    delete [] currPos;
    pingPong.src -> getTexture(1).loadData(currCol, width, height, GL_RGBA);
    delete [] currCol;
    pingPong.src -> getTexture(2).loadData(velocity, width, height, GL_RGBA);
    delete [] velocity;
}

//--------------------------------------------------------------
void ofApp::setNextImage(){
    char str[20];
    
    if (imgID >= numImgs) {
        imgID = 0;
    }
    
    sprintf(str, "specter_%02d.png", imgID);
    
    img.load(str);
    pixels = img.getPixels();
    
    
    nextPos = new float[width*height*4];
    nextCol = new float[width*height*4];
    
    int offsetX = width * 0.5;
    int offsetY = height * 0.5;
    
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            float r = (float)1.0-(pixels[j*width*4+i*4+0] * 0.0039);
            float g = (float)0.6-(pixels[j*width*4+i*4+1] * 0.0039);
            float b = (float)0.3-(pixels[j*width*4+i*4+2] * 0.0039);
            float brightness = (r+g+b) * 0.3333;
            
            if ((pixels[j*width*4+i*4+0] > 200) &&
                (pixels[j*width*4+i*4+1] > 90)){
                r = g = b = 0;
            }
            nextPos[j*width*4+i*4+0] = i-offsetX;
            nextPos[j*width*4+i*4+1] = j-offsetY;
            nextPos[j*width*4+i*4+2] = brightness * 256.0;
            nextPos[j*width*4+i*4+3] = 1.0;
            
            nextCol[j*width*4+i*4+0] = r;
            nextCol[j*width*4+i*4+1] = g;
            nextCol[j*width*4+i*4+2] = b;
            nextCol[j*width*4+i*4+3] = 1.0;
        }
    }
    
    pingPong.src -> getTexture(3).loadData(nextPos, width, height, GL_RGBA);
    delete [] nextPos;
    pingPong.src -> getTexture(4).loadData(nextCol, width, height, GL_RGBA);
    delete [] nextCol;
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(0);
    
    ofBackground(0);
    
    cam.setupPerspective(); // set the image to the right direction
    cam.setPosition(0, 0, 720);
    cam.setParent(node[1]);
    
    for (int i=0; i<numNodes; i++) {
        if(i>0){
            node[i].setParent(node[i-1]);
        }
    }
    
    render.load("shaders/render");
    updatePos.load("", "shaders/update.frag");
    
    pingPong.allocate(width, height, GL_RGBA32F, 5);
    
    // Allocate the final
    renderFBO.allocate(ofGetWindowWidth(), ofGetWindowHeight(), GL_RGBA32F);
    renderFBO.begin();
    ofClear(0, 0, 0, 255);
    renderFBO.end();
    
    setInitImage();
    
    sender.setup(HOST, PORT);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // morphing
    float time = ofGetElapsedTimef();
    if(((int)time % lifeTime == 0) && ((int)time != 0)){
        startCount = ofGetElapsedTimef();
    }
    
    if((time - startCount <= morphingDuration) && ((int)time >= morphingDuration)){
        isMorphing = true;
    }else{
        isMorphing = false;
    }
    
    // ofxOscMessage pulse;
    // static int pulseLength;
    if(isMorphing){
        if(!imgUpdated){
            imgID++;
            setNextImage();
            imgUpdated = true;
            //            pulse.setAddress("/overdose");
            //            pulse.addIntArg(1);
            //            sender.sendMessage(pulse, false);
        }
        
        //        if(pulseLength < 5){
        //            pulseLength ++;
        //        }else{
        //            pulse.setAddress("/overdose");
        //            pulse.addIntArg(0);
        //            sender.sendMessage(pulse, false);
        //        }
        overdose = 1;
    }else{
        imgUpdated = false;
        //        pulseLength = 0;
        overdose = 0;
    }
    
    // nodes
    float freq = 0.2;
    for (int i=0; i<numNodes; i++) {
        float amp = i * 200;
        node[i].setPosition(ofVec3f(sin(time * freq) * amp, cos(time * freq) * amp, sin(time * freq) * amp));
        freq *= 0.5;
    }
    
    // dispatch to update shader
    pingPong.dst->begin();
    pingPong.dst->activateAllDrawBuffers();
    ofClear(0);
    
    updatePos.begin();
    updatePos.setUniformTexture("u_currPosTex", pingPong.src->getTexture(0), 0);
    updatePos.setUniformTexture("u_currColTex", pingPong.src->getTexture(1), 1);
    updatePos.setUniformTexture("u_velTex", pingPong.src->getTexture(2), 2);
    updatePos.setUniformTexture("u_nextPosTex", pingPong.src->getTexture(3), 3);
    updatePos.setUniformTexture("u_nextColTex", pingPong.src->getTexture(4), 4);
    updatePos.setUniform1f("u_time", time);
    updatePos.setUniform2f("u_resolution", ofGetWidth(), ofGetHeight());
    updatePos.setUniform1i("u_overdose", overdose);
    pingPong.src->draw(0, 0);
    updatePos.end();
    
    pingPong.dst->end();
    pingPong.swap();
    
    ofEnableAlphaBlending();
    renderFBO.begin();
    
    ofSetColor(0, 0, 0, 10);
    ofFill();
    ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    /*
     if(debugMode){
     for (int i=0; i<numNodes; i++) {
     if(i==0){
     ofSetColor(255, 0, 0);
     }else{
     ofSetColor(0, 0, 255);
     }
     node[i].draw();
     }
     }
     */
    render.begin();
    
    cam.lookAt(node[0]);
    cam.begin();
    
    render.setUniformTexture("u_currPosTex", pingPong.dst->getTexture(0), 0);
    render.setUniformTexture("u_currColTex", pingPong.dst->getTexture(1), 1);
    
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    vbo.draw(GL_POINTS, 0, numParticles);
    ofDisableBlendMode();
    
    glEnd();
    cam.end();
    
    render.end();
    renderFBO.end();
    ofPopStyle();
    
    /* osc
     // ID of messages is correspondent to that of variables of MAX multislider
     if((int)time % 7 == 0){
     if(!oscSent0){
     ofxOscMessage msg1, msg2, msg3;
     msg1.setAddress("/nodePos/x");
     msg1.addFloatArg(abs(node[1].getPosition().normalize().x));
     sender.sendMessage(msg1, false);
     msg2.setAddress("/nodePos/y");
     msg2.addFloatArg(abs(node[1].getPosition().normalize().y));
     sender.sendMessage(msg2, false);
     msg3.setAddress("/nodePos/z");
     msg3.addFloatArg(abs(node[1].getPosition().normalize().z));
     sender.sendMessage(msg3, false);
     oscSent0 = true;
     }
     }else{
     oscSent0 = false;
     }*/
    if((int)time % 10 == 0){
        static int i, j;
        if(!oscSent1){
            
            ofxOscMessage msg4, msg5, msg6, msg7;
            msg4.setAddress("/pixel/R");
            msg4.addFloatArg(pixels[j*width*4+i*4+0] * 0.0039);
            sender.sendMessage(msg4, false);
            msg5.setAddress("/pixel/G");
            msg5.addFloatArg(pixels[j*width*4+i*4+1] * 0.0039);
            sender.sendMessage(msg5, false);
            msg6.setAddress("/pixel/B");
            msg6.addFloatArg(pixels[j*width*4+i*4+2] * 0.0039);
            sender.sendMessage(msg6, false);
            
            oscSent1 = true;
            
            updatePos.begin();
            updatePos.setUniform1f("u_pixelR", currCol[j*width*4+i*4+0]);
            updatePos.setUniform1f("u_pixelG", currCol[j*width*4+i*4+1]);
            updatePos.setUniform1f("u_pixelB", currCol[j*width*4+i*4+2]);
            updatePos.end();
            
            if(i<width-1){
                i++;
                j++;
            }else{
                i = j = 0;
            }
        }
    }else{
        oscSent1 = false;
    }
}

//--------------------------------------------------------------
float ofApp::easeInOutQuad (float current, float init, float destination, float duration) {
    if ((current/=duration/2) < 1){
        return destination/2*current*current + init;
    }
    return -destination/2 * ((--current)*(current-2) - 1) + init;
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofHideCursor();
    ofBackground(0);
    
    ofSetColor(255,255,255);
    renderFBO.draw(0, 0);
    
    if(debugMode){
        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        pingPong.dst->getTexture(0).draw(0, 0, 288, 288);
        ofDrawBitmapStringHighlight("Current Position", 0, 14);
        pingPong.dst->getTexture(1).draw(288, 0, 288, 288);
        ofDrawBitmapStringHighlight("Current Color", 288, 14);
        pingPong.dst->getTexture(2).draw(576, 0, 288, 288);
        ofDrawBitmapStringHighlight("Vector Field", 576, 14);
        pingPong.dst->getTexture(3).draw(864, 0, 288, 288);
        ofDrawBitmapStringHighlight("Next Position", 864, 14);
        pingPong.dst->getTexture(4).draw(1152, 0, 288, 288);
        ofDrawBitmapStringHighlight("Next Color", 1152, 14);
        ofPopStyle();
        ofDrawBitmapStringHighlight("FPS : " + ofToString(ofGetFrameRate()), 0, ofGetHeight() - 2);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key==' '){
        debugMode = !debugMode;
    }else if(key=='i'){
        startCount = ofGetElapsedTimef();
    }
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
