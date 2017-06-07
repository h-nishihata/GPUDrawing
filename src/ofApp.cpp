#include "ofApp.h"

float* currPos;
float* nextPos;
float* currCol;
float* nextCol;
float* velocity;


void ofApp::setInitImage(){
    img.load("test_00.jpg");
    pixels = img.getPixels();
    
    currPos = new float[width*height*4];
    currCol = new float[width*height*4];
    
    int offsetX = width * 0.5;
    int offsetY = height * 0.5;
    
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            float r = (float)1.0-(pixels[j*width*3+i*3+0] / 256.0);
            float g = (float)0.6-(pixels[j*width*3+i*3+1] / 256.0);
            float b = (float)0.3-(pixels[j*width*3+i*3+2] / 256.0);
            float brightness = (r+g+b) * 0.3333;
            
            myCoords[j*width+i] = ofVec2f(i,j);
            myVerts[j*width+i] = ofVec3f(i,j,brightness*256.0);
            myColor[j*width+i] = ofFloatColor(r,g,b,1.0);
            currPos[j*width*4+i*4+0] = i-offsetX;
            currPos[j*width*4+i*4+1] = j-offsetY;
            currPos[j*width*4+i*4+2] = brightness*256.0;
            currPos[j*width*4+i*4+3] = 1.0; // map alpha value
            
            currCol[j*width*4+i*4+0] = r;
            currCol[j*width*4+i*4+1] = g;
            currCol[j*width*4+i*4+2] = b;
            currCol[j*width*4+i*4+3] = 1.0;
        }
    }
    
    vbo.setTexCoordData(myCoords, numParticles, GL_DYNAMIC_DRAW);
    vbo.setVertexData(&myVerts[0], numParticles, GL_DYNAMIC_DRAW);
    vbo.setColorData(myColor, numParticles, GL_STATIC_DRAW);
    
    pingPong.src -> getTexture(0).loadData(currPos, width, height, GL_RGBA);
    delete [] currPos;
    pingPong.src -> getTexture(1).loadData(currCol, width, height, GL_RGBA);
    delete [] currCol;
    
    
    velocity = new float[width * height * 4];
    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            int i = width * y + x;
            velocity[i*4+0] = 0.0;
            velocity[i*4+1] = 0.0;
            velocity[i*4+2] = 0.0;
            velocity[i*4+3] = ofRandom(1,150);
        }
    }
    
    pingPong.src -> getTexture(2).loadData(velocity, width, height, GL_RGBA);
    delete [] velocity;
}

//--------------------------------------------------------------
void ofApp::setNextImage(){
    char str[20];
    
    if (imgID >= numImgs) {
        imgID = 0;
    }
    
    sprintf(str, "test_%02d.jpg", imgID);
    
    img.load(str);
    pixels = img.getPixels();
    
    nextPos = new float[width*height*4];
    nextCol = new float[width*height*4];
    
    int offsetX = width * 0.5;
    int offsetY = height * 0.5;
    
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            float r = (float)1.0-(pixels[j*width*3+i*3+0] / 256.0);
            float g = (float)0.6-(pixels[j*width*3+i*3+1] / 256.0);
            float b = (float)0.3-(pixels[j*width*3+i*3+2] / 256.0);
            float brightness = (r+g+b) * 0.3333;
            
            nextPos[j*width*4+i*4+0] = i-offsetX;
            nextPos[j*width*4+i*4+1] = j-offsetY;
            nextPos[j*width*4+i*4+2] = brightness*256.0;
            nextPos[j*width*4+i*4+3] = lifeTime;  // particles life time
            
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
    ofDisableAlphaBlending();
    
    cam.setupPerspective(); // set the image to the right direction
    cam.setPosition(camPosX, camPosY, camPosZ);
    cam.setParent(node[1]);
    
    for (int i=0; i<numNodes; i++) {
        if(i>0){
            node[i].setParent(node[i-1]);
        }
    }
    
    render.load("shaders/render");
    updatePos.load("", "shaders/update.frag");
    
    pingPong.allocate(width, height, GL_RGBA32F, 5);
    setInitImage();
    
    debugMode = true;
    
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
    
    if(isMorphing){
        if(!imgUpdated){
            imgID++;
            setNextImage();
            imgUpdated = true;
        }
        overdose = 1;
    }else{
        overdose = 0;
        imgUpdated = false;
    }
    
    
    // nodes
    float freq = 0.2;    
    for (int i=0; i<numNodes; i++) {
        float amp = i*200;
        node[i].setPosition(ofVec3f(sin(time * freq)*amp, cos(time * freq)*amp, sin(time * freq)*amp));
        freq *= 0.5;
    }
    
    
    /* camera
    static float zCount;
    zCount += 1.0e-3; // 1.0e-3 = 0.001, 1.0e+3 = 1000
    
    float camEasingZ;
    camEasingZ = easeInOutQuad(zCount, 0.0, camZLength, 1.0);
    
    if (!zFlag) {
        camPosZ = ((camZLength-camEasingZ)-(camZLength/2));
        if (camPosZ <= camZLength/2*-1) {
            zFlag = true;
            zCount = 0;
        }
    }else if(zFlag) {
        camPosZ = (camEasingZ-camZLength/2);
        if (camPosZ >= camZLength/2) {
            zFlag = false;
            zCount = 0;
        }
    }
    cam.setPosition(camPosX, camPosY, camPosZ);
    */
    
    
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
    updatePos.setUniform3f("u_nodePos", node[0].getPosition());
    updatePos.setUniform1i("u_overdose", overdose);
    
    pingPong.src->draw(0, 0);
    
    updatePos.end();
    
    pingPong.dst->end();
    pingPong.swap();
    
    
    // osc
    if((int)time % 7 == 0){
        if(!oscSent){
            ofxOscMessage msg0, msg1, msg2;
            msg0.setAddress("/nodePos/x");
            msg0.addFloatArg(abs(node[1].getPosition().normalize().x));
            sender.sendMessage(msg0, false);
            msg1.setAddress("/nodePos/y");
            msg1.addFloatArg(abs(node[1].getPosition().normalize().y));
            sender.sendMessage(msg1, false);
            msg2.setAddress("/nodePos/z");
            msg2.addFloatArg(abs(node[1].getPosition().normalize().z));
            sender.sendMessage(msg2, false);
            oscSent = true;
        }
    }else{
        oscSent = false;
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
    
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofEnablePointSprites();
    
    cam.lookAt(node[0]);
    cam.begin();
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
    
    render.begin();
    
    render.setUniformTexture("u_currPosTex", pingPong.src->getTexture(0), 0);
    render.setUniformTexture("u_currColTex", pingPong.src->getTexture(1), 1);
    
    vbo.draw(GL_POINTS, 0, numParticles);
    
    render.end();
    cam.end();
    
    ofDisablePointSprites();
    ofPopStyle();
    
    if(debugMode){
        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        pingPong.dst->getTexture(0).draw(0, 0, 288, 288);
        ofDrawBitmapStringHighlight("Current Position", 0, 14);
        pingPong.dst->getTexture(1).draw(288, 0, 288, 288);
        ofDrawBitmapStringHighlight("Current Color", 288, 14);
        pingPong.dst->getTexture(2).draw(576, 0, 288, 288);
        ofDrawBitmapStringHighlight("Velocity", 576, 14);
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
