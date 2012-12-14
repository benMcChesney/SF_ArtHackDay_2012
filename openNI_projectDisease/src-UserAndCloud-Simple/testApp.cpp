#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    
    ofSetLogLevel(OF_LOG_VERBOSE);

//#ifndef USE_ONI_MOVIE
    openNIDevice.setup();
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(false);
    openNIDevice.addUserGenerator();
    openNIDevice.setMaxNumUsers(2);
    openNIDevice.start();
    
    // set properties for all user masks and point clouds
    //openNIDevice.setUseMaskPixelsAllUsers(true); // if you just want pixels, use this set to true
    openNIDevice.setUseMaskTextureAllUsers(true); // this turns on mask pixels internally AND creates mask textures efficiently
    openNIDevice.setUsePointCloudsAllUsers(false);
   // openNIDevice.setPointCloudDrawSizeAllUsers(2); // size of each 'point' in the point cloud
   // openNIDevice.setPointCloudResolutionAllUsers(2); // resolution of the mesh created for the point cloud eg., this will use every second depth pixel
    
    // you can alternatively create a 'base' user class
//    ofxOpenNIUser user;
//    user.setUseMaskTexture(true);
//    user.setUsePointCloud(true);
//    user.setPointCloudDrawSize(2);
//    user.setPointCloudResolution(2);
//    openNIDevice.setBaseUserClass(user);
//#endif

    
    openNIPlayer.setup();
    openNIPlayer.addImageGenerator();
    openNIPlayer.addDepthGenerator();
    openNIPlayer.setRegister(true);
    openNIPlayer.setMirror(false);
    openNIPlayer.addUserGenerator();
    openNIPlayer.setMaxNumUsers(2);
	openNIPlayer.start();
    
    openNIPlayer.setUseMaskTextureAllUsers(true); // this turns on mask pixels internally AND creates mask textures efficiently
    openNIPlayer.setUsePointCloudsAllUsers(false);
   // openNIPlayer.setPointCloudDrawSizeAllUsers(2); // size of each 'point' in the point cloud
   // openNIPlayer.setPointCloudResolutionAllUsers(2); // resolution of the mesh created for the point cloud eg., this will use every second depth pixel

    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
    
    roiArea = ofRectangle( ofRandomWidth() , ofRandomHeight() , ofRandomWidth() , ofRandomHeight() ) ;
    bUseRoi = false ;
    bFullscreen = false ;
    
    bCalibrateSpace = true ;
    bRecording = false ;
    
    loadRoiCalibration() ; 
}

//--------------------------------------------------------------
void testApp::update(){
    openNIDevice.update();
    openNIPlayer.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255, 255, 255);
    
    ofBackground( 0 , 0 , 0 ) ;
    customDraw() ;
    return ;
    
    ofPushMatrix() ; 
    //ofScale( 0.75 , 0.75 , 1 ) ;
    
    if ( bCalibrateSpace == true )
    {
        ofSetColor( 255 , 255 , 0 ) ;
        ofNoFill() ;
        ofSetLineWidth( 3 ) ;
        ofRect( roiArea ) ;
        ofSetLineWidth( 1 ) ;
        ofFill() ;
        string calibrateString = "CALIBRATING WORLD SPACE!!! \nRECORDING : " + ofToString ( bRecording )  ; 
        ofDrawBitmapStringHighlight( calibrateString , 15 , 700 ) ;
        
        ofSetColor( 255 , 255 , 255 ) ;
        ofPushMatrix();
            openNIDevice.drawDebug() ;
            openNIPlayer.drawDebug(0, 480 );
           // openNIDevice.drawMatchedDebug( 0 , 0 , openNIDevice.getWidth() , openNIDevice.getHeight() );
        ofPopMatrix();
    }
    
    ofPushMatrix();
    // use a blend mode so we can see 'through' the mask(s)
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    // get number of current users
    int numUsers = openNIPlayer.getNumTrackedUsers();
   
    ofPushMatrix() ;
    //ofTranslate( 0 , 480 ) ;
    // iterate through users
    for (int i = 0; i < numUsers; i++){
        ofxOpenNIUser & user = openNIPlayer.getTrackedUser(i);
        user.drawMask();
        user.drawSkeleton() ; 
    }
    ofPopMatrix() ; 
    
    numUsers = openNIDevice.getNumTrackedUsers();
    // iterate through users
    for (int i = 0; i < numUsers; i++){
        ofxOpenNIUser & user = openNIDevice.getTrackedUser(i);
        user.drawMask();
        user.drawSkeleton() ;
    }
     
    
    ofDisableBlendMode();
    ofPopMatrix();
    
    
    // draw some info regarding frame counts etc
	ofSetColor(0, 255, 0);
	string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate()) + " Device FPS: " + ofToString(openNIDevice.getFrameRate());
    
    ofDrawBitmapStringHighlight(msg, 15 , ofGetHeight() - 45 ) ; 
    //verdana.drawString(msg, 20, openNIPlayer.getNumDevices() * 480 - 20);
       
    if ( bFullscreen )
    {
        ofSetColor( 255 , 255 , 255 ) ;
        roiFbo.begin() ;
        ofPushMatrix();
        // draw debug (ie., image, depth, skeleton)
        ofTranslate( -roiArea.x , -roiArea.y ) ;
        openNIDevice.drawMatchedDebug( 0 , 0 , openNIDevice.getWidth() , openNIDevice.getHeight() );
        ofPopMatrix();
        roiFbo.end() ;
        
        ofSetColor( 255 , 255 , 255 ) ;
        float ratio =  9.0f /16.0f ;
        
        float h = ofGetWidth() * ratio ; 
        roiFbo.draw( 0 , 0 , ofGetWidth() , h ) ;
    }
    ofPopMatrix() ; 
}

void testApp::resetRoi( )
{
    roiFbo.allocate( roiArea.width , roiArea.height , GL_RGBA ) ;
    roiFbo.begin( ) ;
    ofClear( 1 , 1 , 1 , 0 ) ;
    roiFbo.end( ) ;
}

void testApp::saveRoiCalibration ( )
{
    roiCalibration.clear() ; 
    roiArea.x = roiCalibration.setValue( "x" , roiArea.x ) ;
    roiArea.y = roiCalibration.setValue( "y" , roiArea.y ) ;
    roiArea.width = roiCalibration.setValue( "width" , roiArea.width ) ;
    roiArea.height = roiCalibration.setValue( "height" , roiArea.height ) ;
    roiCalibration.saveFile( "roiCalibration.xml" ) ; 

}

void testApp::loadRoiCalibration ( )
{
    bool bLoadResult = roiCalibration.loadFile( "roiCalibration.xml" ) ;
    if ( bLoadResult == true )
    {
        ofLog( OF_LOG_WARNING , "ROI Calibration loaded OK!" ) ;
        roiArea.x = roiCalibration.getValue( "x" , 0.0f ) ;
        roiArea.y = roiCalibration.getValue( "y" , 0.0f ) ;
        roiArea.width = roiCalibration.getValue( "width" , 10.0f ) ;
        roiArea.height = roiCalibration.getValue( "height" , 10.0f ) ;
        bUseRoi = true  ;
    }
    else
    {
        ofLog( OF_LOG_WARNING , "XML could not be loaded !" ) ;
        bUseRoi = true  ;
    }
}

//--------------------------------------------------------------
void testApp::userEvent(ofxOpenNIUserEvent & event){
    // show user event messages in the console
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void testApp::exit(){
    openNIDevice.stop();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    cout << "key :" << key << endl ;
    
    
    
    /*
     key :356
     key :357
     key :358
     key :359
     key :97
     key :119
     key :100
     key :115
     */
    switch ( key)
    {
        case 'f':
        case 'F':
            ofToggleFullscreen() ;
            bFullscreen = !bFullscreen ; 
            break ;
        case 356:
            roiArea.x -=1 ;
            break ;
        case 357:
            roiArea.y -=1 ;
            break ;
        case 358:
            roiArea.x +=1 ;
            break ;
        case 359:
            roiArea.y +=1 ;
            break ;
            
        case 97:
            roiArea.width -=1 ;
            break ;
        case 119:
            roiArea.height -=1 ;
            break ;
        case 100:
            roiArea.width +=1 ;
            break ;
        case 115:
            roiArea.height +=1 ;
            break ;
            
        case 'l':
        case 'L':
            loadRoiCalibration( ) ;
            break ;
            
        case ' ':
            
            if(!openNIDevice.isRecording()){
                openNIDevice.startRecording(ofToDataPath("test.oni"));
                bRecording = true ; 
            }else{
                openNIDevice.stopRecording();
                bRecording = false ; 
            }
            break;
        case 'c':
        case 'C':
            bCalibrateSpace = !bCalibrateSpace ;
            break ; 
        case 'p':
        case 'P':
            openNIPlayer.startPlayer("keeper.oni");
            break;
        case '/':
            openNIPlayer.setPaused(!openNIPlayer.isPaused());
            break;
        case 'm':
        case 'M':
            openNIPlayer.firstFrame();
            break;
        case '<':
        case ',':
            openNIPlayer.previousFrame();
            break;
        case '>':
        case '.':
            openNIPlayer.nextFrame();
            break;
        case 'x':
        case 'X':
            openNIDevice.stop();
            openNIPlayer.stop();
            break;
        case 't':
        case 'T':
            openNIDevice.toggleRegister();
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

void testApp::customDraw ( )
{
    int numUsers = openNIDevice.getNumTrackedUsers();
    // iterate through users
    
    ofColor col1 = ofColor( 0 , 255 , 0 ) ;
    ofColor col2 = ofColor( 0 , 0 , 255 ) ;
    
    for (int i = 0; i < numUsers; i++)
    {
        ofxOpenNIUser & user = openNIDevice.getTrackedUser(i);
        user.drawMask();
        
        int numLimbs = user.getNumJoints() ;
        for ( int j = 0 ; j < user.getNumJoints() ; j++ )
        {
            float ratio = ( float ) j / ( float ) user.getNumJoints() ;
            
            ofxOpenNIJoint joint = user.joints[j] ;
            ofPoint p = joint.getProjectivePosition() ;
            //cout << " joint[" << j << "] : " << p << endl ;
            ofColor c = col1.lerp( col2, ratio ) ;
            ofSetColor( c ) ;
            ofCircle( p.x , p.y , 15 ) ;
            ofDrawBitmapStringHighlight( ofToString(j) , p.x , p.y - 15 ) ;
        }
        
        //user.drawSkeleton() ;
    }

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    roiArea.width = x - roiArea.x ;
    roiArea.height = y - roiArea.y ;
    bUseRoi = false ;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    roiArea.x = x ;
    roiArea.y = y ;
    bUseRoi = false ;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    //roiArea.x = x ;
    //roiArea.y = y ;
    
    float ratio = 9.0f  /16.0f  ; // 480.0f / 640.0f ;
    
    roiArea.width = x - roiArea.x ;
    roiArea.height = roiArea.width * ratio ;
    
    resetRoi( );
    
    bUseRoi = true ;
    
    saveRoiCalibration( ) ;
    

}

/*
 
 // iterate through users
 for (int i = 0; i < numUsers; i++){
 
 // get a reference to this user
 ofxOpenNIUser & user = curDevice->getTrackedUser(i);
 
 // draw the mask texture for this user
 user.drawMask();
 
 // you can also access the pixel and texture references individually:
 
 // TEXTURE REFERENCE
 //ofTexture & tex = user.getMaskTextureReference();
 // do something with texture...
 
 // PIXEL REFERENCE
 //ofPixels & pix = user.getMaskPixels();
 // do something with the pixels...
 
 // and point clouds:
 
 ofPushMatrix();
 // move it a bit more central
 ofTranslate(0 , 0 , 0 ) ;
 ofEnableAlphaBlending()  ;
 ofSetColor( 255 , 255 , 255 , 125 ) ;
 user.drawPointCloud();
 
 // you can also access the mesh:
 
 // MESH REFERENCE
 //ofMesh & mesh = user.getPointCloud();
 // do something with the point cloud mesh
 
 ofPopMatrix();
 
 }
 

 */

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}