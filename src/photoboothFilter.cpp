#include "photoboothFilter.h"

//--------------------------------------------------------------
void photoboothFilter::setup(){

    myLUT.load("assets/LUTS/Lord Kelvin.cube");

    shader.load("assets/shaders/shader.vert", "assets/shaders/shader.frag");

	img.loadImage("assets/images/default.jpg");
	imgNoise.loadImage("assets/images/noiseText.jpg");
	//polaroidImg.loadImage("assets/images/polaroid-template.png");
	polaroidImg.loadImage("assets/images/polaroid-template-750.png");

	tex2.allocate(imgNoise.width,imgNoise.height,GL_RGB);
	tex2 = imgNoise.getTextureReference();

	noiseTextureSize.x  = -imgNoise.width;
	noiseTextureSize.y  = -imgNoise.height;

	noiseAmount         = 1.0f;
    colorFilterAmount   = 0.0f;

    vignetteSize        = img.width;

    startMilli          = 0;
    endMilli            = 0;
    imgLoadStartMilli   = 0;
    imgLoadEndMilli     = 0;
    imgResizeStartMilli = 0;
    imgResizeEndMilli   = 0;
    lutStartMilli       = 0;
    lutEndMilli         = 0;
    drawStartMilli      = 0;
    drawEndMilli        = 0;
    shaderStartMilli    = 0;
    shaderEndMilli      = 0;

    savePhotoNumFile    = false;

    doSave              = false;
    ofBuffer buffer     = ofBufferFromFile("assets/pc.nc");
    string s            = buffer.getText();

    photoNumber         = ofToInt(s);


    msgs.push_back("What a lovely time we had!");
    msgs.push_back("Wonderful models");
    msgs.push_back("What a fantastic day");
    msgs.push_back("A day not to forget");
    msgs.push_back("The summer of 2012");
    msgs.push_back("Escaping the weather");
    msgs.push_back("Exploring ModelMe at \n Nottingham Contemporary");
    msgs.push_back("We should do this more often");
    msgs.push_back("Stunning");
    msgs.push_back("Beautiful");
    msgs.push_back("Rave On");

    msgID = 0;


    font.loadFont("assets/fonts/PhontPhreaksHandwriting.ttf", 13, true, true, false, 1.0, 300);
    font.setLineHeight(50.0f);

    fontStamp.loadFont("assets/fonts/veteran typewriter.ttf", 40, true, true, false, 1.0, 72);


}
//--------------------------------------------------------------
void photoboothFilter::update(){

    if(savePhotoNumFile){
        string pNumStr = ofToString(photoNumber);
        ofBuffer databuffer;
        databuffer.set(pNumStr.c_str(), pNumStr.size());
        bool fileWritten = ofBufferToFile("assets/pc.nc", databuffer);
        savePhotoNumFile = false;
    }

}
//--------------------------------------------------------------
ofImage photoboothFilter::apply(string filepath){

    startMilli = ofGetElapsedTimeMillis();

    imgLoadStartMilli = ofGetElapsedTimeMillis();
    img.loadImage(filepath);
    imgLoadEndMilli = ofGetElapsedTimeMillis();

    //------------------------------------------------------

    imgResizeStartMilli  = ofGetElapsedTimeMillis();
    float resizePct = 0.857f;
    float resizedH  = polaroidImg.width*resizePct;
    float resizedW  = (resizedH/img.height)*img.width; // polaroid is in a differne orientation to image

    img.resize(resizedW, resizedH);
    float newW = resizedH*1.1;
    img.crop((img.width-resizedH)*.5,0, newW, resizedH);

    imgResizeEndMilli  = ofGetElapsedTimeMillis();


    //------------------------------------------------------

    float centerX         = ((polaroidImg.width*1.15)-img.width)*.5;
    float centerY         = (polaroidImg.width-img.height)*.5; // approx

    textureSize.x       = img.height + (polaroidImg.height*.1f); // allow for border of polaroid
	textureSize.y       = img.height; // flipped because they get rotated on the card

    //------------------------------------------------------

    lutStartMilli   = ofGetElapsedTimeMillis();

    myLUT.applyLUT(img.getPixelsRef());
    lutEndMilli     = ofGetElapsedTimeMillis();

    //------------------------------------------------------

    drawStartMilli = ofGetElapsedTimeMillis();
    ofSetColor(255,255,255,255);
    ofFill();
    ofRect(0,0,polaroidImg.width,polaroidImg.height);

    ofBackground(0,0,0);

    float vignetteSizeAplied = vignetteSize + ofRandom(-130,130);
    //if(vignetteSizeAplied > img.width) vignetteSizeAplied = img.width;

    shaderStartMilli    = ofGetElapsedTimeMillis();

	shader.begin();

        shader.setUniformTexture("tex0", myLUT.img.getTextureReference(), 0);
        shader.setUniformTexture("noiseTexture", tex2, 0);
        shader.setUniform2f("textureSize", textureSize.x, textureSize.y);
        shader.setUniform1f("vignetteSize",vignetteSizeAplied);
        shader.setUniform1f("colorFilterAmount",colorFilterAmount);
        shader.setUniform1f("noiseAmount",noiseAmount+ofRandom(-0.2f,0.2f));
        shader.setUniform2f("noiseTextureSize", noiseTextureSize.x, noiseTextureSize.y);

        ofPushMatrix();
        ofTranslate(0,img.height,0);
        ofRotateZ(-90);
        ofTranslate(-centerX,centerY,0);
            myLUT.draw(0,0);
        ofPopMatrix();

    shader.end();

    shaderEndMilli    = ofGetElapsedTimeMillis();

    ofEnableAlphaBlending();
    polaroidImg.draw(0,0);

    drawEndMilli = ofGetElapsedTimeMillis();

    //------------------------------------------

    msgID = ofRandom(0, msgs.size()-1);

    ofSetColor(20,20,60,155);
    ofRectangle r = font.getStringBoundingBox(msgs[msgID],0,0);
    ofPushMatrix();
    ofTranslate((((polaroidImg.width-150) - r.width)*.5)+150 + (r.width*.5), (((polaroidImg.height-800) - r.height)*.5)+800);
    ofRotateZ(ofRandom(-4,4));
        font.drawString(msgs[msgID], (r.width*.5)*-1,0);
    ofPopMatrix();


    //------------------------------------------------------


    photoNumber++;

    ofPushMatrix();
    ofTranslate(polaroidImg.width*0.965f,polaroidImg.height*0.04f);
    ofRotateZ(ofRandom(89.0,91.0));
    //
    ofSetColor(0,0,0,55);
        fontStamp.drawString("Nottingham Contemporary 40"+ofToString(photoNumber),0,0);
    ofPopMatrix();

    ofSetColor(255,255,255,255);
    outputImg.grabScreen(0,0,polaroidImg.width,polaroidImg.height);

    //------------------------------------------------------

    ofPixels pixPol = polaroidImg.getPixelsRef();

    outputImg.setImageType(OF_IMAGE_COLOR_ALPHA);
    pix = outputImg.getPixelsRef();

    float borderPct = 0.015f;
    int border = int(polaroidImg.width*borderPct);


    for(int y = 0; y < pix.getHeight(); y++){

        for(int x = 0; x < pix.getWidth(); x++){

            if(y< border || x < border || y> pix.getHeight()-border || x > pix.getWidth()-border){
                ofColor c = pixPol.getColor(x,y);
                pix.setColor(x,y,c );
            }
        }
    }

    outputImg.setFromPixels(pix);

    ofDisableDataPath();
    string dateStampStr = +"-"+ofToString(ofGetMonth())+"_"+ofToString(ofGetDay())+"_"+ofToString(ofGetHours())+"_"+ofToString(ofGetMinutes())+"_"+ofToString(ofGetSeconds());

    string fileName = "C:/wamp/www/photobooth3/out/photobooth3_"+dateStampStr+"_"+ofToString(photoNumber)+".png";
    ofSaveImage(pix,fileName,OF_IMAGE_QUALITY_BEST);

    ofEnableDataPath();
    endMilli = ofGetElapsedTimeMillis();

    savePhotoNumFile = true;

    return outputImg;

}
