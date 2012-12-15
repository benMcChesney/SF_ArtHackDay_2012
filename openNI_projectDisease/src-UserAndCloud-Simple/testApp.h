#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxTweenzor.h"
#include "ofxUI.h"
#include "JointRoute.h" 
#include "Agent.h"

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
    
    
    bool bDrawOpenNI ; 
    void saveRoiCalibration ( ) ;
    void loadRoiCalibration ( ) ;
    
    bool bDepthRegistration ; 
    ofxXmlSettings roiCalibration ;

    //ofxOpenNI openNIPlayer;
    
    bool bCalibrateSpace ;
       
    void customDraw ( ) ;
    
    //ofxUI stuff
    bool bShowGui ; 
    ofxUICanvas *gui;
    void guiEvent(ofxUIEventArgs &e);
    void init_ofxUI ( ) ;
    
    
    vector<JointRoute> jointRoutes ;

    vector<Agent> agents ; 
    int maxParticles ;
    float maxVelocity , maxForce ;
    float bufferDistance ;

    
    void setParticleParams() ;

    float lastSpawn ;
    float spawnDelay ;
    int spawnPerBeat ;
    
    //vector<ofPoint> fakeSkeleton ;
    vector<ofPoint> skeleton ;
    bool bSkeletonActive ; 
    
    float pointSize ;
    
    ofFbo fbo ;
    
    float fadeFboAmount ;
    
    vector<ofColor> colorPool ;
    ofColor getRandomColor ( ) ; 
    
};

#endif
