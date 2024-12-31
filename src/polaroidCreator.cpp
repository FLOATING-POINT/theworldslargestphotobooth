#include "polaroidCreator.h"

//--------------------------------------------------------------
void polaroidCreator::setup(){

    polaroidImg.loadImage("sourceImages/polaroid-template.png");

}

//--------------------------------------------------------------
ofImage polaroidCreator::create(){

    ofEnableAlphaBlending();

    polaroidImg.draw(0,0);
    outputImg.grabScreen(0,0,polaroidImg.width, polaroidImg.height);

    //string fileName = "snapshot_"+ofToString(ofGetFrameNum())+".jpg";
    //outputImg.saveImage(fileName);

    return outputImg;

}
