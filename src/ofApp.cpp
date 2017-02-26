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
    
    render.load("shaders/render");
    updatePos.load("", "shaders/update.frag");
    
    
    img.load("test.jpg");
    pixels = img.getPixels();
    
    pingPong.allocate(width, height, GL_RGBA32F, 2);
    float* posAndAge = new float[width*height*4];
    
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            float r = (float)pixels[j*width*3+i*3+0] / 256.0;
            float g = (float)pixels[j*width*3+i*3+1] / 256.0;
            float b = (float)pixels[j*width*3+i*3+2] / 256.0;
            float brightness = (r + g + b) / 3.0f;
            
            myCoords[j*width+i] = ofVec2f(i,j);
            myVerts[j*width+i] = ofVec3f(i,j,brightness*256.0);
            myColor[j*width+i] = ofFloatColor(r,g,b,1.0);
            
            posAndAge[j*width*4+i*4+0] = i;
            posAndAge[j*width*4+i*4+1] = j;
            posAndAge[j*width*4+i*4+2] = brightness*256.0;
            posAndAge[j*width*4+i*4+3] = 0;
            
        }
    }
    
    vbo.setTexCoordData(myCoords, numParticles, GL_DYNAMIC_DRAW);
    vbo.setVertexData(&myVerts[0], numParticles, GL_DYNAMIC_DRAW);
    vbo.setColorData(myColor, numParticles, GL_STATIC_DRAW);
    
    pingPong.src -> getTexture(0).loadData(posAndAge, width, height, GL_RGBA);
    // バッファを増やすときはpingPongBuffer.hも変更が必要
//    pingPong.src -> getTexture(2).loadData(initPos, width, height, GL_RGBA);
    delete [] posAndAge;
//    delete [] initPos;
    
    
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
    cam.lookAt(node[1]);
    float time = ofGetElapsedTimef();
    
    float freqMult = 0.5;
    float amp = 5;
    
    for (int i=0; i<3; i++) {
        node[i].setPosition(ofVec3f(sin(time * freqMult)*amp + 256*i, cos(time * freqMult)*amp + 256*i, sin(time * freqMult)*amp));
        
        freqMult *= 0.8;
        amp *= 2;
    }
    
    
    pingPong.dst->begin();
    
        pingPong.dst->activateAllDrawBuffers();
        ofClear(0);
        
        updatePos.begin();
    
            updatePos.setUniformTexture("u_posAndAgeTex", pingPong.src->getTexture(0), 0);
            updatePos.setUniformTexture("u_velAndMaxAgeTex", pingPong.src->getTexture(1), 1);
//            updatePos.setUniformTexture("u_initialTex", pingPong.src->getTexture(2), 2);
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
    
    ofSetColor(255,0,0);
    //    for (int i=0; i<3; i++) {
//    node[0].setScale(10);
    node[1].draw();
    //    }
    
        cam.begin();
            render.begin();
                render.setUniformTexture("u_posAndAgeTex", pingPong.src->getTexture(0), 0);
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
