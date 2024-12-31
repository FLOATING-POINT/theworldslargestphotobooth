#pragma once

#include "ofMain.h"
#include "ofxLUT.h"

class photoboothFilter{

	public:
		void setup();
		ofImage apply(string filepath);
		void update();

        ofFbo               fbo;

        ofImage             outputImg;
        ofImage             polaroidImg;
        ofxLUT              myLUT;

        ofShader            shader;
        ofTexture           tex1;
        ofTexture           tex2;
        ofVec2f             dim;
        ofVec2f             texcoord0;

        int                 dirLoadIndex;
        int                 startMilli, endMilli;
        int                 imgLoadStartMilli, imgLoadEndMilli;
        int                 imgResizeStartMilli, imgResizeEndMilli;
        int                 lutStartMilli, lutEndMilli;
        int                 drawStartMilli, drawEndMilli;
        int                 shaderStartMilli, shaderEndMilli;

        bool                savePhotoNumFile;

        ofDirectory         dir;

        ofImage             img;
        ofImage             imgNoise;

        ofVec2f             textureSize;
        ofVec2f             noiseTextureSize;

        float               vignetteSize;
        float               colorFilterAmount;
        float               noiseAmount;

        bool                doSave;
        ofPixels            pix;

        ofTrueTypeFont      font;
        ofTrueTypeFont      fontStamp;

        vector< string>     msgs;
        int                 msgID;
        int                 photoNumber;




};

