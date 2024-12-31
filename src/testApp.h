#pragma once

#include "ofMain.h"
#include "photoboothFilter.h"
#include "CanonCameraWrapper.h"
#include "ofxXmlSettings.h"
#include "ofxHttpUtils.h"
#include "ofxCvHaarFinder.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void createPolaroids();
		void capture();
		void imageDownloaded(bool & d);
		void httpReqResponse(ofxHttpResponse & response);
		void setupArduino(const int & version);


		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		bool                    showGUI;

        photoboothFilter        imgFilter;
        bool                    doFilter;
        vector< ofImage >       filteredImg;
        ofImage                 polaroidImg;
        ofImage                 waitImg;


        CanonCameraWrapper      remoteCapture;
        bool                    hasCaptured;

        int                     shotStartMilli,shotEndMilli;
        int                     createStartMilli,createEndMilli;
        int                     shotCount;

        vector< string >        imgPaths;


        ofxXmlSettings          XMLPathSettings;
        string                  pathToProjectorMachine;
        string                  pathToUploadScript;

        float                   lastTimerSecs;
        bool                    hasReset;


        ofxHttpUtils            httpUtils;
        ofVideoGrabber 		    vidGrabber;
        ofImage                 vidImage;
        ofxCvHaarFinder         finder;

        int                     waitSecsAfterSession;
        int                     lastPolaroidTime;
        int                     minHeadSize;

        ofArduino	            arduino;
        bool                    arduinoSetupBool;

};
