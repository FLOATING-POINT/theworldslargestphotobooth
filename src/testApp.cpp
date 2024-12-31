#include "testApp.h"
#include "ofxSimpleGuiToo.h"

//--------------------------------------------------------------
void testApp::setup(){

    //ofSetLogLevel(OF_LOG_VERBOSE);
	ofBackground(0);
	ofSetColor(255);


	finder.setup("assets/cascades/haarcascade_frontalface_default.xml");

	vidGrabber.setDeviceID(0);
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(320,240);


    remoteCapture.setup(0);
    remoteCapture.setDeleteFromCameraAfterDownload(true);
    remoteCapture.setDownloadPath( ofToDataPath("cameraImages"));
    remoteCapture.enableDownloadOnTrigger();
    remoteCapture.openSession();

    ofAddListener(remoteCapture.imageDownloaded,this,&testApp::imageDownloaded);

    hasCaptured     = false;
    doFilter        = false;
    showGUI         = false;

    shotStartMilli      = 0;
    shotEndMilli        = 0;

    createStartMilli    = 0;
    createEndMilli      = 0;
    shotCount           = 0;

    waitSecsAfterSession   = 10;// the num of secs to wait after polaroids have been created for next sessions
    lastPolaroidTime       = 0;

    waitImg.loadImage("assets/images/wait.png");


   // ofAddListener(httpUtils.newResponseEvent,this,&testApp::httpReqResponse);
    httpUtils.setTimeoutSeconds(200000);
    httpUtils.start();

    minHeadSize = 65;
	imgFilter.setup();

	filteredImg.resize(4);
	imgPaths.resize(4);


    pathToProjectorMachine  = "";
	pathToUploadScript      = "";

    XMLPathSettings.loadFile("paths.xml");


    arduino.connect("COM3", 57600);
    arduinoSetupBool        = false;


    hasReset                = false;
    lastTimerSecs           = 0;

    pathToProjectorMachine  = XMLPathSettings.getValue("paths:prmachine", "", 0);
    pathToUploadScript      = XMLPathSettings.getValue("paths:uploadscript", "", 0);


    printf("pathToProjectorMachine %s \n",pathToProjectorMachine.c_str());
    printf("pathToUploadScript %s \n",pathToUploadScript.c_str());

	gui.addSlider("vignetteSize",imgFilter.vignetteSize,0.0f,imgFilter.img.width*1.5);// should be width but the loaded image (below) gets rotated
	gui.addSlider("Colour gain ",imgFilter.noiseAmount,0.0f,2.0f);
	gui.addSlider("Min Head Size ",minHeadSize,0,320);

    gui.loadFromXML();
	gui.show();

}

//--------------------------------------------------------------
void testApp::update(){


    arduino.update();

    if(arduino.isArduinoReady() && !arduinoSetupBool) {
        printf("ARDUINO CONNECTED AND SETUP \n");

        arduinoSetupBool = true;

        for (int i = 0; i < 13; i++){
            arduino.sendDigitalPinMode(i, ARD_OUTPUT);
        }
        arduino.sendDigitalPinMode(11, ARD_PWM);

    }

    if(hasCaptured && arduinoSetupBool){
        //arduino.sendPwm(11, 255);
    } else {
        arduino.sendPwm(11, 1);
    }


    if(ofGetFrameNum() % 10 ==0){
        vidGrabber.grabFrame();

        if(!hasCaptured && vidGrabber.isFrameNew()  && ofGetElapsedTimef() > (lastPolaroidTime + waitSecsAfterSession) ) {

            vidImage.setFromPixels(vidGrabber.getPixels(), 320,240, OF_IMAGE_COLOR,true);
            finder.findHaarObjects(vidImage,minHeadSize,minHeadSize);
            //printf("nblobs 1 %i \n",finder.blobs.size());
            if(finder.blobs.size() > 0){

                printf("nblobs %i \n",finder.blobs.size());
                for(int i =0; i < finder.blobs.size(); i++ ){
                    ofRectangle cur = finder.blobs[i].boundingRect;

                    if(cur.width > minHeadSize && cur.height > minHeadSize){
                        capture();
                        shotStartMilli  = ofGetElapsedTimeMillis();
                    }
                }

            }

        }
    }

    ofSetWindowTitle("FR: "+ofToString(ofGetFrameRate()));

    imgFilter.update();

    if(ofGetElapsedTimef() - lastTimerSecs >= 1500.0f){
        lastTimerSecs = ofGetElapsedTimef();
        remoteCapture.keepCameraAlive();
    }
}

//--------------------------------------------------------------
void testApp::draw(){

    if(createEndMilli>10){

        ofPushMatrix();
        ofScale(0.35,0.35,1.0);
            for(int i =0; i < 4; i++){
                filteredImg[i].draw(i*filteredImg[i].width,0);
            }
        ofPopMatrix();
    }

    ofPushMatrix();
    ofTranslate(500,500);
    ofScale(2.0,2.0,1.0);

        vidGrabber.draw(0,0);
        ofSetColor(255,255,255);
        ofNoFill();
        for(int i = 0; i < finder.blobs.size(); i++) {
            ofRectangle cur = finder.blobs[i].boundingRect;
            ofRect(cur.x, cur.y, cur.width, cur.height);
        }

	ofPopMatrix();


    if(arduinoSetupBool) ofDrawBitmapString("Arduino Ready: ", 20, 780);
    ofDrawBitmapString("Photo shoot / download time: "+ofToString(float(shotEndMilli-shotStartMilli)/1000.0f), 20, 800);
    ofDrawBitmapString("Img load time: "+ofToString(float(imgFilter.imgLoadEndMilli -imgFilter.imgLoadStartMilli)/1000.0f), 20, 820);
    ofDrawBitmapString("Img resize time: "+ofToString(float(imgFilter.imgResizeEndMilli-imgFilter.imgResizeStartMilli )/1000.0f), 20, 840);
    ofDrawBitmapString("Shader time: "+ofToString(float(imgFilter.shaderEndMilli-imgFilter.shaderStartMilli)/1000.0f), 20, 860);
    ofDrawBitmapString("LUT Filter time: "+ofToString(float(imgFilter.lutEndMilli-imgFilter.lutStartMilli)/1000.0f), 20, 880);
    ofDrawBitmapString("Last complie time: "+ofToString(float(imgFilter.drawEndMilli-imgFilter.drawStartMilli)/1000.0f), 20, 900);
    ofDrawBitmapString("Create and output time: "+ofToString(float(createEndMilli-createStartMilli)/1000.0f), 20, 920);

    if(showGUI) gui.draw();

}
//--------------------------------------------------------------
void testApp::imageDownloaded(bool & d){

    string p    = remoteCapture.getLastImagePath();
    printf(" p %s \n",p.c_str());
    imgPaths[shotCount] = p;

    shotCount++;
    if(shotCount<4) {
        capture();
    } else {
        arduino.sendPwm(11, 255);
        createPolaroids();
        shotEndMilli    = ofGetElapsedTimeMillis();
    }
}
//--------------------------------------------------------------
void testApp::createPolaroids(){

    createStartMilli = ofGetElapsedTimeMillis();

    for(int i = 0; i < 4; i++){
        ofImage img = imgFilter.apply(imgPaths[i]);
        draw();
        filteredImg[i] = img;
    }

    createEndMilli  = ofGetElapsedTimeMillis();


    shotCount       = 0;

    // save images to projector machine

    ofDisableDataPath();
    printf("Uploading to projector machine\n");
    string dateStampStr = "";

    for(int i = 0; i < 4; i++){

        dateStampStr = +"-"+ofToString(ofGetMonth())+"-"+ofToString(ofGetDay())+"-"+ofToString(ofGetHours())+"-"+ofToString(ofGetMinutes())+"-"+ofToString(ofGetSeconds())+"-"+ofToString(i);
        filteredImg[i].saveImage(pathToProjectorMachine+"/photobooth_"+dateStampStr+".png");

    }


    ofEnableDataPath();

    // start upload to Flickr and Twitter
    hasCaptured     = false;

    printf("Uploading to Flickr\n");
    ofSleepMillis(500);

    //httpUtils.stop();
    //httpUtils.start();
    ofxHttpForm form;
	form.action = pathToUploadScript;
	form.method = OFX_HTTP_GET;
	httpUtils.addForm(form);

	lastPolaroidTime    = ofGetElapsedTimef();
    hasCaptured         = false;

    printf("IMAGE CREATION COMPLETE \n");

}

//--------------------------------------------------------------
void testApp::capture(){

    bool taken      = remoteCapture.takePicture();
    hasCaptured     = true;

}
//--------------------------------------------------------------
void testApp::keyPressed(int key){


    switch(key){
        case ' ':
            capture();
            shotStartMilli  = ofGetElapsedTimeMillis();
        break;

        case 'q':
            if (arduinoSetupBool) {
                printf("ARD_HIGH \n");
                //arduino.sendPwm(11, 255);
            }
        break;

        case 'w':
            if (arduinoSetupBool) {
                printf("ARD_LOW \n");
               // arduino.sendPwm(11, 1);
            }
        break;
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

    switch (key){
        case ' ' :
            doFilter = true;
        break;

        case 'd':
            showGUI = !showGUI;
        break;
    }


}
//--------------------------------------------------------------
void testApp::httpReqResponse(ofxHttpResponse & response){

    printf("Uploading to Flickr COMPLETE \n");
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

   	doFilter = true;

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
