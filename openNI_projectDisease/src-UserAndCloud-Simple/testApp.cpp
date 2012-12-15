#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    
    Tweenzor::init( ) ; 
    ofSetLogLevel(OF_LOG_WARNING);

        bDrawOpenNI = false;
     
//#ifndef USE_ONI_MOVIE
    openNIDevice.setup();
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(false);
    openNIDevice.addUserGenerator();
    openNIDevice.setMaxNumUsers(4);
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

    /*
    openNIPlayer.setup();
    openNIPlayer.addImageGenerator();
    openNIPlayer.addDepthGenerator();
    openNIPlayer.setRegister(true);
    openNIPlayer.setMirror(false);
    openNIPlayer.addUserGenerator();
    openNIPlayer.setMaxNumUsers(2);
	openNIPlayer.start();
    
    openNIPlayer.setUseMaskTextureAllUsers(true); // this turns on mask pixels internally AND creates mask textures efficiently
    openNIPlayer.setUsePointCloudsAllUsers(false);*/
   // openNIPlayer.setPointCloudDrawSizeAllUsers(2); // size of each 'point' in the point cloud
   // openNIPlayer.setPointCloudResolutionAllUsers(2); // resolution of the mesh created for the point cloud eg., this will use every second depth pixel
    
    colorPool.push_back( ofColor( 242, 56 , 90 ) ) ;
    colorPool.push_back( ofColor( 245, 165 , 3 ) ) ;
    colorPool.push_back( ofColor( 233, 241 , 223 ) ) ;
    colorPool.push_back( ofColor( 74, 217 , 217 ) ) ;
    colorPool.push_back( ofColor( 54, 177 , 191 ) ) ;
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
    
    roiArea = ofRectangle( ofRandomWidth() , ofRandomHeight() , ofRandomWidth() , ofRandomHeight() ) ;
    bUseRoi = false ;
    bFullscreen = false ;
    
    bCalibrateSpace = true ;
       
    loadRoiCalibration() ;
    
    init_ofxUI ( ) ;
    
    for ( int i = 0 ; i < 15 ; i++ )
    {
        JointRoute jr ;
        jointRoutes.push_back( jr ) ;
        skeleton.push_back ( ofPoint() ) ; 
    }
    
    jointRoutes[0].addIndex(1) ;
    jointRoutes[0].addIndex(9) ;
    jointRoutes[0].addIndex(12) ;
    
    jointRoutes[1].addIndex(3) ;
    jointRoutes[1].addIndex(6) ;
    jointRoutes[1].addIndex(2) ;
    jointRoutes[2].addIndex(-1) ;
    
    jointRoutes[3].addIndex(4) ;
    jointRoutes[3].addIndex(4) ;
    jointRoutes[4].addIndex(5) ;
    jointRoutes[4].addIndex(5) ;
    jointRoutes[5].addIndex(-1) ;
    
    jointRoutes[6].addIndex(7) ;
    jointRoutes[6].addIndex(7) ;
    jointRoutes[7].addIndex(8) ;
    jointRoutes[7].addIndex(8) ;
    jointRoutes[8].addIndex(-1) ;
    jointRoutes[9].addIndex(10) ;
    jointRoutes[10].addIndex(11) ;
    jointRoutes[11].addIndex(-1) ;
    
    jointRoutes[12].addIndex(13) ;
    jointRoutes[13].addIndex(14) ;
    jointRoutes[14].addIndex(-1) ;

    
    lastSpawn = 0 ;
    spawnDelay = 1.5f ;
    bDepthRegistration = false ;
      
    fbo.allocate( ofGetWidth() , ofGetHeight() , GL_RGBA ) ;
    fbo.begin() ;
        ofClear( 1 , 1 , 1, 0 ) ; 
    fbo.end() ;

    bSkeletonActive = false ;

}

ofColor testApp::getRandomColor ( )
{
    return colorPool[ (int) ofRandom( colorPool.size() ) ] ; 
}

//--------------------------------------------------------------
void testApp::update(){
    
    
    ofSetWindowTitle( "FPS : " + ofToString( ofGetFrameRate() ) ) ; 
    Tweenzor::update( ofGetElapsedTimeMillis() ) ; 
    openNIDevice.update();
    //openNIPlayer.update();
    
    /*
    vector<Agent>::iterator it = agents.begin() ;
    while ( it != agents.end() )
    {
//        float dist = (*it).position.distanceSquared( (*it).target ) ;
        (*it).update( ) ; 
        it++ ; 
    }
    */
    float xRatio = (float)ofGetWidth() / (float)roiArea.width;

    if ( bSkeletonActive == true )
    {
        vector<Agent>::iterator it = agents.begin() ;
        while ( it != agents.end() )
        {
            (*it).target = skeleton[(*it).targetIndex] ;
           // (*it).target.x *= xRatio ;
            //(*it).target.y *= xRatio ;
            
            if ( (*it).changeTarget == true )
            {
                (*it).changeTarget = false ; 
                int newIndex = jointRoutes[(*it).targetIndex].getRandomTargetIndex() ;
                //cout << "change target :: currentIndex : " << (*it).targetIndex << endl ;
                //cout << "NEW TARGET : " << newIndex << endl ;
                if ( newIndex == -1 )
                {
                    it = agents.erase( it ) ;
                }
                else
                {
                    (*it).targetIndex = newIndex ;
                    (*it).force = ofVec3f( ofRandom ( 0 , TWO_PI )  ,  ofRandom ( 0 , TWO_PI )  ,  ofRandom ( 0, TWO_PI )  ) ;

                    it++ ;
                }
                
            }
            else
                it++ ;
            
           
            (*it).update( ) ;
        
        }
    }
   // }
    

}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255, 255, 255);
    
    ofBackground( 0 , 0 , 0 ) ;
       
    string status = "# of agents : " + ofToString( agents.size() ) + " bSkeletonActive : " + ofToString( bSkeletonActive ) ;
    ofDrawBitmapStringHighlight( status , 15 , ofGetHeight() - 50 ) ; 
       
     

    if ( bCalibrateSpace == true )
    {
        string calibrateString = "CALIBRATING WORLD SPACE!!!" ;
        ofDrawBitmapStringHighlight( calibrateString , 15 , 700 ) ;
        
        ofSetColor( 255 , 255 , 255 ) ;
        ofPushMatrix();
            if ( bDrawOpenNI == true ) 
                openNIDevice.drawDebug() ;
        
        ofPopMatrix();
       // if ( bUseRoi )
        //{
                  // }

    }
    
    
    
    ofPushMatrix();
    // use a blend mode so we can see 'through' the mask(s)
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    int numUsers = openNIDevice.getNumTrackedUsers();
    if ( numUsers > 0 )
        bSkeletonActive = true ;
    else
        bSkeletonActive = false;
     
    
    ofDisableBlendMode();
    ofPopMatrix();
    
    if ( bSkeletonActive == true )
        customDraw() ;
    
     ofDisableBlendMode();
    ofEnableAlphaBlending() ;
    ofSetColor( 255 , 255 , 255 ) ;
  
    
    ofSetColor( 255 , 255 , 0 ) ;
    ofNoFill() ;
    ofSetLineWidth(5 ) ;
    ofRect( roiArea ) ;
    ofSetLineWidth( 1 ) ;
    ofFill() ;
        
    fbo.begin() ;
       
        ofSetColor( 0 , 0 , 0, fadeFboAmount ) ;
        ofRect( 0 , 0, ofGetWidth() , ofGetHeight() ) ;
    if ( bSkeletonActive == true )
    {
        //ofTranslate( -roiArea.x , -roiArea.y ) ;
        ofMesh mesh ;
        
        for ( int i = 0 ; i < agents.size() ; i++ )
        {
            mesh.addVertex( agents[i].position ) ;
            mesh.addColor ( agents[i].color ) ;
        }
        
        glPointSize( pointSize ) ;
        mesh.setMode( OF_PRIMITIVE_POINTS ) ;
        ofSetColor( 255 , 255 , 255 ) ;
        mesh.draw( ) ;
    }
    fbo.end() ;
  
    
    ofSetColor( 255 , 255 , 255 ) ;
    float ratio =  9.0f /16.0f ;
     float xRatio = (float)ofGetWidth() / (float)roiArea.width;
    float h = ofGetWidth() * ratio ;
    ofPushMatrix() ;
        ofTranslate( -roiArea.x , -roiArea.y ) ;
        fbo.draw( 0 , 0  , fbo.getWidth() * xRatio , fbo.getHeight()  ) ;
    ofPopMatrix() ;
    return ;
   
    if ( bFullscreen )
    {
        ofSetColor( 255 , 255 , 255 ) ;
        roiFbo.begin() ;
        ofPushMatrix();
        // draw debug (ie., image, depth, skeleton)
        ofTranslate( -roiArea.x , -roiArea.y ) ;
        openNIDevice.drawMatchedDebug( 0 , 0  , -1 , -1 );
        ofPopMatrix();
        roiFbo.end() ;
        
        ofSetColor( 255 , 255 , 255 ) ;
        float ratio =  9.0f /16.0f ;
        
        float h = ofGetWidth() * ratio ; 
        roiFbo.draw( 0 , 0 , ofGetWidth() , h ) ;
    }
    ofPopMatrix() ; 
    
    ofEnableAlphaBlending() ; 
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
    
    bUseRoi = true ; 

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
        bUseRoi = false  ;
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
            agents.clear() ;
            /*
            if(!openNIDevice.isRecording()){
                openNIDevice.startRecording(ofToDataPath("test.oni"));
                bRecording = true ; 
            }else{
                openNIDevice.stopRecording();
                bRecording = false ; 
            }*/
            break;
        case 'c':
        case 'C':
            bCalibrateSpace = !bCalibrateSpace ;
            break ; 
        case 'p':
        case 'P':
          //  openNIPlayer.startPlayer("keeper.oni");
            break;
        case '/':
            //openNIPlayer.setPaused(!openNIPlayer.isPaused());
            break;
        case 'm':
        case 'M':
            //openNIPlayer.firstFrame();
            break;
        case '<':
        case ',':
            //openNIPlayer.previousFrame();
            break;
        case '>':
        case '.':
            //openNIPlayer.nextFrame();
            break;
        case 'x':
        case 'X':
            //openNIDevice.stop();
            //openNIPlayer.stop();
            break;
        case 't':
        case 'T':
            openNIDevice.toggleRegister();
            break;
        case 'g':
        case 'G':
            
            bShowGui = !bShowGui ;
            gui->toggleVisible();
            /*
            if ( bShowGui )
                gui->disable() ;
            else
                gui->enable() ;*/
            break ; 
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
    
    /*
 
    for ( int i = 0 ; i < fakeSkeleton.size() ; i++ )
    {
        float ratio = ( float ) i / ( float ) fakeSkeleton.size() ;
        ofPoint p = fakeSkeleton[i] ;
        ofColor c = col1.lerp( col2, ratio ) ;
        ofSetColor( c ) ;
       
        //ofCircle( p.x , p.y , 15 ) ;
        ofDrawBitmapStringHighlight( ofToString(i) , p.x , p.y - 15 ) ;
    }
  */
     //   return ;
    
    
    float xRatio = (float)ofGetWidth() / (float)roiArea.width;
    //xRatio *= .5 ;

    for (int i = 0; i < numUsers; i++)
    {
        ofxOpenNIUser & user = openNIDevice.getTrackedUser(i);
       // user.drawMask();
               
        int numLimbs = user.getNumJoints() ;
        for ( int j = 0 ; j < user.getNumJoints() ; j++ )
        {
            float ratio = ( float ) j / ( float ) user.getNumJoints() ;
            
            ofxOpenNIJoint joint = user.joints[j] ;
            ofPoint p = joint.getProjectivePosition() ;
            p.z = 0 ;
            p.x *= xRatio ;
            p.y *= xRatio ;
            skeleton[j] = p ;
            //cout << " joint[" << j << "] : " << p << endl ;
            ofColor c = col1.lerp( col2, ratio ) ;
            ofSetColor( c ) ;
            ofCircle( p.x , p.y , 15 ) ;
            ofDrawBitmapStringHighlight( ofToString(j) , p.x , p.y - 15 ) ;
        }
        
        //user.drawSkeleton() ;
    }
    
    if ( ofGetElapsedTimef() > ( lastSpawn + spawnDelay ) )
    {
        for ( int i = 0 ; i < spawnPerBeat ; i++ )
        {
            if ( agents.size() < maxParticles )
            {
                Agent a ;
                int index = (int)jointRoutes[0].getRandomTargetIndex() ;
                
                ofPoint randomOffset = ofPoint ( ofRandom ( -25 , 25 ) , ofRandom ( -25 , 25 ) ) ;
                ofPoint p = skeleton[0] + randomOffset ;

                a.setup( p ,  skeleton[index] , index ) ;
                ofColor col = getRandomColor() ;
                a.color = col ; 
                agents.push_back( a )  ;
            }
            
        }
        
        lastSpawn = ofGetElapsedTimef() ; 
        setParticleParams() ; 
    }

}

void testApp::setParticleParams()
{
    for ( int i = 0 ; i < agents.size() ; i++ )
    {
        agents[i].bufferDistance = ofRandom( bufferDistance * .5 , bufferDistance * 1.5 ) ;
        agents[i].maxVelocity  = ofRandom( maxVelocity * .5 , maxVelocity * 1.5 )  ;
        agents[i].maxForce = ofRandom( maxForce * .5 , maxForce * 1.5 )  ;
    }
}


void testApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
	int kind = e.widget->getKind();
	//cout << "got event from: " << name << endl;
	
    /*
	if(name == "RED")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		cout << "RED " << slider->getScaledValue() << endl;
		//red = slider->getScaledValue();
	}
     */
    
   
    if ( name == "NUM PARTICLES")
    {
        maxParticles = ( (ofxUISlider *) e.widget)->getScaledValue() ;
    }
    
    if ( name == "MAX VELOCITY")
    {
        maxVelocity = ( (ofxUISlider *) e.widget)->getScaledValue() ;
        setParticleParams( ) ; 
    }
    
    if ( name == "MAX FORCE")
    {
        maxForce = ( (ofxUISlider *) e.widget)->getScaledValue() ;
        setParticleParams( ) ; 
    }
    
    if ( name == "BUFFER DISTANCE")
    {
        bufferDistance = ( (ofxUISlider *) e.widget)->getScaledValue() ;
        setParticleParams( ) ;
    }
    
    if ( name == "POINT SIZE")
    {
        pointSize = ( (ofxUISlider *) e.widget)->getScaledValue() ;
    }
    
    if ( name == "FBO FADE AMOUNT" )
    {
        fadeFboAmount = ( (ofxUISlider *) e.widget)->getScaledValue() ;
    }
    
    if ( name == "SPAWN DELAY" )
    {
        spawnDelay = ( (ofxUISlider *) e.widget)->getScaledValue() ;
    }
    
    if ( name == "SPAWN PER BEAT" )
    {
        spawnPerBeat = ( (ofxUISlider *) e.widget)->getScaledValue() ;
    }
    
    if ( name == "CALIBRATE SPACE" )
    {
        bCalibrateSpace = (( ofxUIToggle *) e.widget )->getValue( );
    }
    
    if ( name == "DRAW OPENNI" )
    {
        bDrawOpenNI = (( ofxUIToggle *) e.widget )->getValue( );
    }
    
    if ( name == "DEPTH REGISTER" )
    {
        openNIDevice.setRegister( (( ofxUIToggle *) e.widget )->getValue( ) ) ;
    }
    //"DEPTH REGISTER"
    
    /*
     
        openNIPlayer.setRegister(true);
     */
    gui->saveSettings("GUI/guiSettings.xml");
    
}

void testApp::init_ofxUI ( )
{
    bShowGui = true ;
    
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    
    gui = new ofxUICanvas( 0, 0, length, ofGetHeight() ) ;
     gui->setDrawBack(false);
    gui->addSlider("NUM PARTICLES", 10, 50000 , maxParticles , length , dim ) ;
    gui->addSlider("MAX VELOCITY", 0.001f, 12.0f , maxVelocity , length , dim ) ;
    gui->addSlider("MAX FORCE", 0.001f , 2.0f , maxForce , length , dim ) ;
    gui->addSlider("BUFFER DISTANCE", 5.0f , 250.0f , bufferDistance , length , dim ) ;
    gui->addSlider("POINT SIZE", 1.0f , 7.0f , pointSize , length , dim ) ;
    gui->addSlider("FBO FADE AMOUNT", 0.0f , 75.0f , fadeFboAmount , length , dim ) ;
    gui->addSlider("SPAWN DELAY", 0.0001f , 4.0f , spawnDelay , length , dim ) ;
    gui->addSlider("SPAWN PER BEAT", 1.0f , 400.0f , spawnPerBeat , length , dim ) ;
    gui->addToggle("CALIBRATE SPACE", bCalibrateSpace , dim * 2 , dim * 2 ) ; 
    gui->addToggle("DRAW OPENNI", bDrawOpenNI , dim * 2 , dim * 2 ) ;
    gui->addToggle("DEPTH REGISTER", bDepthRegistration , dim * 2 , dim * 2 ) ;
    //bDepthRegistration
    //
       ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("GUI/guiSettings.xml" )  ;
    
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    if ( x < 300 ) return ; 
    roiArea.width = x - roiArea.x ;
    roiArea.height = y - roiArea.y ;
    //bUseRoi = false ;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    if ( x < 300 ) return ; 
    roiArea.x = x ;
    roiArea.y = y ;
    //bUseRoi = false ;
    
    cout << " x : " << x << " y : " << y << endl ;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    //roiArea.x = x ;
    //roiArea.y = y ;
    
    if ( x < 300 ) return ; 
    
    float ratio = 9.0f  /16.0f  ; // 480.0f / 640.0f ;
    

    roiArea.width = x - roiArea.x ;
    roiArea.x -= 640 ; 
    roiArea.height = roiArea.width * ratio ;
    
    resetRoi( );
    
    bUseRoi = true ;
    
    saveRoiCalibration( ) ;
    loadRoiCalibration() ; 
    

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