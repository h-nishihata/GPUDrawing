#include "ofApp.h"

float* position;
float* nextPos;
float* velocity;

void ofApp::setInitImage(){
    img.load("test_00.jpg");
    pixels = img.getPixels();
    
    position = new float[width*height*4];
    
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
            position[j*width*4+i*4+0] = i-offsetX;
            position[j*width*4+i*4+1] = j-offsetY;
            position[j*width*4+i*4+2] = brightness*256.0;
            position[j*width*4+i*4+3] = 1.0; // map alpha value
        }
    }
    
    vbo.setTexCoordData(myCoords, numParticles, GL_DYNAMIC_DRAW);
    vbo.setVertexData(&myVerts[0], numParticles, GL_DYNAMIC_DRAW);
    vbo.setColorData(myColor, numParticles, GL_STATIC_DRAW);
    
    pingPong.src -> getTexture(0).loadData(position, width, height, GL_RGBA);
    delete [] position;
    
    
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
    
    pingPong.src -> getTexture(1).loadData(velocity, width, height, GL_RGBA);
    delete [] velocity;
}

//--------------------------------------------------------------
void ofApp::setNextImage(int _imgID){
    char str[20];
    
    if (imgID < numImgs) {
        _imgID = imgID;
    }else{
        _imgID = imgID = 0;
    }
    
    sprintf(str, "test_%02d.jpg", _imgID);
    
    img.load(str);
    pixels = img.getPixels();
    
    nextPos = new float[width*height*4];
    
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
            
            nextPos[j*width*4+i*4+0] = i-offsetX;
            nextPos[j*width*4+i*4+1] = j-offsetY;
            nextPos[j*width*4+i*4+2] = brightness*256.0;
            nextPos[j*width*4+i*4+3] = lifeTime;  // particles life time
        }
    }
    
    vbo.setTexCoordData(myCoords, numParticles, GL_DYNAMIC_DRAW);
    vbo.setVertexData(&myVerts[0], numParticles, GL_DYNAMIC_DRAW);
    vbo.setColorData(myColor, numParticles, GL_STATIC_DRAW);
    
    pingPong.src -> getTexture(2).loadData(nextPos, width, height, GL_RGBA);
    delete [] nextPos;
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
    
    pingPong.allocate(width, height, GL_RGBA32F, 3);
    
    setInitImage();
    
    debugMode = false;
    
    sender.setup(HOST, PORT);
}

//--------------------------------------------------------------
void ofApp::update(){
    float time = ofGetElapsedTimef();
    int surplus = (int)time % (int)lifeTime;
    if(((surplus == 0) && ((int)time != 0)) || (debugSwapImages)){
        overdose = 1;
        if(!imgUpdated){
            imgID++;
            setNextImage(imgID);
            imgUpdated = true;
        }
    }else{
        overdose = 0;
        imgUpdated = false;
    }
    
    // debug
    if((time - startCount <= 1.0) && ((int)time != 0)){
        debugSwapImages = true;
    }else{
        debugSwapImages = false;
    }
    
    // nodes
    float freq = 0.2;
    float amp = 1.0;
    for (int i=0; i<numNodes; i++) {
        node[i].setPosition(ofVec3f(sin(time * freq)*amp, cos(time * freq)*amp, sin(time * freq)*amp));
        freq *= 0.5;
        amp *= 200;
    }
    
    // camera
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

    
    pingPong.dst->begin();
    
        pingPong.dst->activateAllDrawBuffers();
        ofClear(0);
        
        updatePos.begin();
    
            updatePos.setUniformTexture("u_posTex", pingPong.src->getTexture(0), 0);
            updatePos.setUniformTexture("u_velTex", pingPong.src->getTexture(1), 1);
            updatePos.setUniformTexture("u_nextPosTex", pingPong.src->getTexture(2), 2);
            updatePos.setUniform1f("u_time", time);
            updatePos.setUniform2f("u_resolution", ofGetWidth(), ofGetHeight());
            updatePos.setUniform3f("u_nodePos", node[0].getPosition());
            updatePos.setUniform1i("u_overdose", overdose);
    
            pingPong.src->draw(0, 0);
        
        updatePos.end();
    
    pingPong.dst->end();
    pingPong.swap();
    
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
                render.setUniformTexture("u_posTex", pingPong.src->getTexture(0), 0);
                vbo.draw(GL_POINTS, 0, numParticles);
            render.end();
        cam.end();
    
        ofDisablePointSprites();
    ofPopStyle();
    
    if(debugMode){
        ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
            pingPong.dst->getTexture(0).draw(0, 0, 350, 350);
            ofDrawBitmapStringHighlight("Position", 0, 14);
        
            pingPong.dst->getTexture(1).draw(350, 0, 350, 350);
            ofDrawBitmapStringHighlight("Velocity", 350, 14);
        ofPopStyle();
        ofDrawBitmapStringHighlight("FPS : " + ofToString(ofGetFrameRate()), 0,ofGetHeight() - 2);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key==' '){
        debugMode = !debugMode;
    }else if(key=='i'){
        startCount = ofGetElapsedTimef();
    }else if(key == 'a'){
        ofxOscMessage m;
        m.setAddress("/test");
        m.addIntArg(1);
        m.addFloatArg(3.5f);
        m.addStringArg("hello");
        m.addFloatArg(ofGetElapsedTimef());
        sender.sendMessage(m, false);
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
