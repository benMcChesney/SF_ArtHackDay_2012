#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxUI.h"
#include "ofxSimpleMask.h"
//#include "ShaderEffect.h"

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void drawPointCloud();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	ofxKinect kinect;
	
#ifdef USE_TWO_KINECTS
	ofxKinect kinect2;
#endif
	
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage;      // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar;  // the far thresholded image
	
	ofxCvContourFinder contourFinder;
	
	bool bThreshWithOpenCV;
	bool bDrawPointCloud;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
    
    int maxBlobs ;
    int minBlobSize ;
    int maxBlobSize ;
    
    bool bFindContourHoles ; 
	
	// used for viewing the point cloud
	ofEasyCam easyCam;
    
    ofxUICanvas *gui;
	void guiEvent(ofxUIEventArgs &e);
    void ofxUI_init() ;
    
    ofRectangle roiArea ;
    bool bRoiDragging ; 
    bool bRoiReady ;
    
    int drawOffsetX , drawOffsetY ;
    
    ofxXmlSettings calibrateXml ;
    void loadCalibrationXml() ;
    void saveCalibrationXml() ;
    
    
    bool bFullscreen ;
    
    ofFbo fbo ;
    float fboFade ;
    
    float slideDelay ;
    float lastSlideTime ; 
    float outlineScale ;
    float outlineSmoothing ;
    float percPerLayer ;
    int numOutlineLayers ;
    
    ofShader shader ;
    ofFbo shaderFbo ;
    ofxSimpleMask simpleMask ;
    
    vector<ofShader*> shaders ;
    int currentShader ; 
    
    void incrementShader ( ) ; 
    /*
    vector<ofxSimpleMask> masks ;
    vector<ofFbo> shaderFbos ;
    vector<ofFbo> maskFbos ;
    vector<ofShader> shaders ;
    */
   // vector<ShaderEffect> effects ;
//    ShaderEffect shader1 ;
    

};