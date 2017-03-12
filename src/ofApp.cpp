#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(false);
    ofSetFrameRate(0);
    /*
    ofSetVerticalSync(true); になっていると処理がどんなに早く終わってもディスプレイの垂直同期を待ってからの描画になるので60fpsで頭打ちになります。
    この場合十分に処理が早いといいのですが、何かの問題で1フレームあたりの処理にコンスタントに 1000ms/60 = 16.6666ms 以上かかってしまった場合、本来描画するはずの垂直同期に間にあわなくなり最悪次の垂直同期まで待つことになるので急にレートが30fpsまで落ちることがあります。
    なので見た目のブラッシュアップまでも行かないベースシステムの開発中の時は:
     
     ofSetVerticalSync(false);
     ofSetFrameRate(0);
    
    のような感じで垂直同期OFF、フレームレート上限無しのフリーランの状態で開発するのが処理が今どれぐらいの負荷になっているのかがfpsを見るだけで大体わかっていい。
    見た目的な部分に入ってきた場合には垂直同期が入ってないとティアリングがおきるので入れといてください。 by satoruhiga
    */
    
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
    
    
    img.load("test.jpg");
    pixels = img.getPixels();
    
    pingPong.allocate(width, height, GL_RGBA32F, 3);
    float* posAndAge = new float[width*height*4];
    float* initPos = new float[width*height*4];
    
    int offsetX = width * 0.5;
    int offsetY = height * 0.5;
    
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            float r = (float)pixels[j*width*3+i*3+0] / 256.0;
            float g = (float)pixels[j*width*3+i*3+1] / 256.0;
            float b = (float)pixels[j*width*3+i*3+2] / 256.0;
            float brightness = (r + g + b) / 3.0f;
            
            myCoords[j*width+i] = ofVec2f(i,j);
            myVerts[j*width+i] = ofVec3f(i,j,brightness*256.0);
            myColor[j*width+i] = ofFloatColor(r,g,b,1.0);
            
            initPos[j*width*4+i*4+0] = posAndAge[j*width*4+i*4+0] = i-offsetX;
            initPos[j*width*4+i*4+1] = posAndAge[j*width*4+i*4+1] = j-offsetY;
            initPos[j*width*4+i*4+2] = posAndAge[j*width*4+i*4+2] = brightness*256.0;
            posAndAge[j*width*4+i*4+3] = 1.0; // map alpha value
            initPos[j*width*4+i*4+3] = 30.0;  // particles life time
        }
    }
    
    vbo.setTexCoordData(myCoords, numParticles, GL_DYNAMIC_DRAW);
    vbo.setVertexData(&myVerts[0], numParticles, GL_DYNAMIC_DRAW);
    vbo.setColorData(myColor, numParticles, GL_STATIC_DRAW);
    
    pingPong.src -> getTexture(0).loadData(posAndAge, width, height, GL_RGBA);
    // バッファを増やすときはpingPongBuffer.hも変更が必要
    pingPong.src -> getTexture(2).loadData(initPos, width, height, GL_RGBA);
    delete [] posAndAge;
    delete [] initPos;
    
    
    float * velAndMaxAge = new float[width * height * 4];
    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            int i = width * y + x;
            velAndMaxAge[i*4+0] = 0.0;
            velAndMaxAge[i*4+1] = 0.0;
            velAndMaxAge[i*4+2] = 0.0;
            velAndMaxAge[i*4+3] = ofRandom(1,150);
        }
    }
    
    pingPong.src -> getTexture(1).loadData(velAndMaxAge, width, height, GL_RGBA);
    delete [] velAndMaxAge;
    
    showTex = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    float time = ofGetElapsedTimef();
    
    float freq = 0.2; // 周期
    float amp = 1;    // 振幅
    for (int i=0; i<numNodes; i++) {
        node[i].setPosition(ofVec3f(sin(time * freq)*amp, cos(time * freq)*amp, sin(time * freq)*amp));
//        freq *= 1.5;
        amp *= 200;
    }
    /*
    float distFromZero = abs(camPosZ);
    float velPct;
    velPct = ofMap(distFromZero, 0, camPosLmt, 1.0, 0.01);
    
    if (!zFlag) {        
        camPosZ -= 6 * velPct;
        if (camPosZ <= camPosLmt*-1) {
            zFlag = true;
        }
    }else if(zFlag) {
        camPosZ += 6 * velPct;
        if (camPosZ >= camPosLmt) {
            zFlag = false;
        }
    }
    cam.setGlobalPosition(camPosX, camPosY, camPosZ);
    */
    pingPong.dst->begin();
    
        pingPong.dst->activateAllDrawBuffers();
        ofClear(0);
        
        updatePos.begin();
    
            updatePos.setUniformTexture("u_posTex", pingPong.src->getTexture(0), 0);
            updatePos.setUniformTexture("u_velTex", pingPong.src->getTexture(1), 1);
            updatePos.setUniformTexture("u_initialTex", pingPong.src->getTexture(2), 2);
            updatePos.setUniform1f("u_time", time);
            updatePos.setUniform2f("u_resolution", ofGetWidth(), ofGetHeight());
            updatePos.setUniform3f("u_nodePos", node[0].getPosition());
    
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
    
        cam.lookAt(node[0]);
        cam.begin();
    
            for (int i=0; i<numNodes; i++) {
                if(i==0){
                    ofSetColor(255, 0, 0);
                }else{
                    ofSetColor(0, 0, 255);
                }
                node[i].draw();
            }
    
            render.begin();
                render.setUniformTexture("u_posTex", pingPong.src->getTexture(0), 0);
                vbo.draw(GL_POINTS, 0, numParticles);
            render.end();
        cam.end();
    
        ofDisablePointSprites();
    ofPopStyle();
    
    
    if(showTex){
        ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_ALPHA
                              );
            pingPong.dst->getTexture(0).draw(0, 0);
            ofDrawBitmapStringHighlight("Position", 0, 14);
        
            pingPong.dst->getTexture(1).draw(width, 0);
            ofDrawBitmapStringHighlight("Velocity", width, 14);
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key==' '){
        showTex = !showTex;
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
