#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "ofxXmlSettings.h"


#define USE_ONI_MOVIE 2
class testApp : public ofBaseApp{

public:
    
	void setup();
	void update();
	void draw();
    void exit();
    
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

    void userEvent(ofxOpenNIUserEvent & event);
    void gestureEvent(ofxOpenNIGestureEvent & event);
    
	ofxOpenNI openNIDevice;
    
    ofTrueTypeFont verdana;
    
    ofFbo roiFbo ;
    ofRectangle roiArea ;
    bool bUseRoi ;
    bool bFullscreen ;
    void resetRoi( ) ; 
    
    void saveRoiCalibration ( ) ;
    void loadRoiCalibration ( ) ;
    
    ofxXmlSettings roiCalibration ;

    ofxOpenNI openNIPlayer;
    
    bool bCalibrateSpace ;
    bool bRecording ;
    
    void customDraw ( ) ; 
    
    
    
  
};

#endif
