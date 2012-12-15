#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
    maxBlobs = 4 ;
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
#ifdef USE_TWO_KINECTS
	kinect2.init();
	kinect2.open();
#endif
	lastSlideTime = 0.0f ;
    
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 230;
	farThreshold = 70;
	bThreshWithOpenCV = true;
	
	ofSetFrameRate(60);
	
	// start from the front
	bDrawPointCloud = false;
    
    ofxUI_init() ;
    
    drawOffsetX = 10 ;
    drawOffsetY = 10 ;
    //loadCalibrationXml( ) ;
    
    bFullscreen = false ;
    
    shaderFbo.allocate( ofGetWidth() , ofGetHeight() , GL_RGBA ) ; 
    fbo.allocate( ofGetWidth() , ofGetHeight() , GL_RGBA ) ;
    
    //shader.load("shaders/Basic.vert", "shaders/QuasiCrystal.frag");
    //Wiggles
    shader.load("shaders/Basic.vert" , "shaders/YingYang.frag");
    
    fbo.begin() ;
    ofClear( 1 , 1 , 1 , 0 ) ;
    fbo.end() ;
    
    shaderFbo.begin() ;
    ofClear( 1 , 1 , 1, 0 ) ;
    shaderFbo.end() ;
    
    simpleMask.setup("shaders/composite_rgb", ofRectangle( 0 , 0, ofGetWidth() , ofGetHeight() ) ) ;
    
    currentShader = 0 ;
    
    ofShader * shader1 = new ofShader()  ;
    shader1->load("shaders/LED_lights.vert" , "shaders/LED_lights.frag");
    shaders.push_back( shader1 ) ;
    
    ofShader * shader2 = new ofShader()  ;
    shader2->load("shaders/Wiggles.vert" , "shaders/Wiggles.frag");
    shaders.push_back( shader2 ) ;
    
    ofShader * shader3 = new ofShader()  ;
    shader3->load("shaders/YingYang.vert" , "shaders/YingYang.frag");
    shaders.push_back( shader3 ) ;
    
    ofShader * shader4 = new ofShader()  ;
    shader4->load("shaders/Basic.vert" , "shaders/BW_Quasicrystal.frag");
    shaders.push_back( shader4 ) ;
    
    bRoiDragging = false ;
    bRoiReady = false ;
}

void testApp::incrementShader ( )
{
    currentShader++ ;
    lastSlideTime = ofGetElapsedTimef() ; 
    if ( currentShader >= shaders.size() )
    {
        currentShader = 0 ;
    }
}

//--------------------------------------------------------------
void testApp::update() {
	
	ofBackground(100, 100, 100);
	
    if ( ofGetElapsedTimef() > ( lastSlideTime + slideDelay ))
    {
        incrementShader() ; 
    }
    
   
	kinect.update();
	
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        if ( bRoiReady == true )
        grayImage.setROI( roiArea ) ;
		//grayImage.setROI( roiArea ) ;
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
             if ( bRoiReady == true )
            grayThreshNear.setROI( roiArea ) ;
			grayThreshFar = grayImage;
             if ( bRoiReady == true )
            grayThreshFar.setROI( roiArea ) ;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
            //grayImage.setROI( roiArea ) ;
		} else {
			
			// or we do it ourselves - show people how they can work with the pixels
			unsigned char * pix = grayImage.getPixels();
			
			int numPixels = grayImage.getWidth() * grayImage.getHeight();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < nearThreshold && pix[i] > farThreshold) {
					pix[i] = 255;
				} else {
					pix[i] = 0;
				}
			}
		}
		
		// update the cv images
		grayImage.flagImageChanged();
		
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayImage, minBlobSize, maxBlobSize, maxBlobs , bFindContourHoles );
	}
	
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
}

//--------------------------------------------------------------
void testApp::draw() {
	
   
    ofBackground( 0 , 0 , 0 ) ;
    if ( bFullscreen == false )
    {
        //Still calibrating
        ofSetColor(255, 255, 255);
        
        ofPushMatrix() ;
            ofTranslate( drawOffsetX , drawOffsetY , 0 ) ;
            ofSetColor( 255 , 255 ,255 ) ;
            grayImage.draw( 0 , 0 , grayImage.width , grayImage.height );
            //kinect.drawDepth( drawOffsetX , drawOffsetY , kinect.getWidth() , kinect.getHeight()  );
            ofEnableAlphaBlending() ;
            ofSetColor( 255 , 255 ,255 , 125 ) ;
            
            kinect.draw(0 , 0 , grayImage.width , grayImage.height );
            contourFinder.draw( roiArea.x , roiArea.y , kinect.width , kinect.height );
            ofSetColor( 255 , 255 , 0 ) ;
            ofNoFill ( ) ;
            ofRect( roiArea ) ;
            ofFill() ;
        ofPopMatrix() ;
        
        // draw instructions
        ofSetColor(255, 255, 255);
        stringstream reportStream;
        reportStream << " 'g' to toggle UI " << endl ;
        ofDrawBitmapString(reportStream.str(),20,652);
    }
    
    
    else
    {
        fbo.begin() ;
        
        ofSetColor( 0 , 0 , 0, fboFade ) ;
        ofRect( 0 , 0 , ofGetWidth() , ofGetHeight() ) ; 
        float _scale = kinect.getWidth() / roiArea.width ; 
        //Trying to overlap our objects
        ofPushMatrix( ) ;
            //ofScale( _scale , _scale , 1 ) ;
            //contourFinder.draw(  0 , 0 , kinect.width * _scale , kinect.width * _scaleddfd );
            // or, instead we can draw each blob individually,
            // this is how to get access to them
            int w = 640 ;
            int h = 480 ;
          //  ofColor col = ofColor::fromHsb( (int)( ofGetElapsedTimef() * 12.0f ) % 255 , 255 , 255 ) ;
        
            for (int i = 0; i < contourFinder.nBlobs; i++)
            {
                int nPts = contourFinder.blobs[i].pts.size() ;
                ofPath projectedShape ;
                for ( int p = 1 ; p < nPts ; p++ )
                {
                                       ofPoint p1 = contourFinder.blobs[i].pts[p-1] ;
                    p1.x /= roiArea.width ;
                    p1.y /= roiArea.height ;
                    p1.x *= ofGetWidth() ;
                    p1.y *= ofGetHeight() ;
                    
                    ofPoint p2 = contourFinder.blobs[i].pts[p] ;
                    p2.x /= roiArea.width ;
                    p2.y /= roiArea.height ;
                    p2.x *= ofGetWidth() ;
                    p2.y *= ofGetHeight() ;
                    
                    projectedShape.lineTo( p1 ) ;
                    projectedShape.lineTo( p2 ) ; 
                }
                //contourFinder.blobs[i].draw( 0 , 0 );
                projectedShape.close() ;
                projectedShape.setFilled( true ) ;
                projectedShape.setFillColor( ofColor::white ) ;
                projectedShape.draw( ) ;
            }
        
        float areaRatio = (float) roiArea.height  / (float) roiArea.width ;
         
        ofPopMatrix() ;
        fbo.end() ;
        
        ofSetColor( 255 , 255 , 255 ) ;
        fbo.draw( 0 , 0 ) ;
    }
    
    ofColor col ;
    col = ofColor::fromHsb( (int)( ofGetElapsedTimef() * 12.0 ) % 255 , 255, 255) ;

    
    shaderFbo.begin() ;
    
    shaders[currentShader]->begin();
    //we want to pass in some varrying values to animate our type / color
    shaders[currentShader]->setUniform1f("time", ofGetElapsedTimef() );
    shaders[currentShader]->setUniform2f("resolution", ofGetWidth() , ofGetHeight() );
    shaders[currentShader]->setUniform1f("r", (float)col.r/ 255.0f );
    shaders[currentShader]->setUniform1f("g", (float)col.g/ 255.0f );
    shaders[currentShader]->setUniform1f("b", (float)col.b/ 255.0f );
    
    /*
     uniform float r ;
     uniform float g ;
     uniform float b ;
     */
  //  ofColor col ;
  //  col = ofColor::fromHsb( (int)( ofGetElapsedTimef() * 8.0 ) % 255 , 255, 255) ;
    ofSetColor( col ) ;
    
    ofRect ( 0 , 0 , ofGetWidth() , ofGetHeight()) ;
    shaders[currentShader]->end();
    shaderFbo.end() ;
    
    ofSetColor( 255 , 255 , 255 ) ;
    
    simpleMask.drawMask(shaderFbo.getTextureReference(), fbo.getTextureReference(), 0, 0, 255.0f ) ; 
}

void testApp::drawPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				mesh.addColor(kinect.getColorAt(x,y));
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	glEnable(GL_DEPTH_TEST);
	mesh.drawVertices();
	glDisable(GL_DEPTH_TEST);
	ofPopMatrix();
    
    
  }

void testApp::loadCalibrationXml()
{
    calibrateXml.loadFile( "calibration.xml" ) ;
    roiArea.x = calibrateXml.getValue( "x" , 0.0f ) ;
    roiArea.y = calibrateXml.getValue( "y" , 0.0f ) ;
    roiArea.width = calibrateXml.getValue( "width" , 0.0f ) ;
    roiArea.height = calibrateXml.getValue( "height" , 0.0f ) ;
    
    bRoiReady = true ; 
}

void testApp::saveCalibrationXml()
{
    calibrateXml.clear() ;
    calibrateXml.setValue( "x" , roiArea.x ) ;
    calibrateXml.setValue( "y" , roiArea.y ) ;
    calibrateXml.setValue( "width" , roiArea.width ) ;
    calibrateXml.setValue( "height" , roiArea.height ) ;
    
    calibrateXml.saveFile( "calibration.xml" ) ;
}

//--------------------------------------------------------------
void testApp::exit() {
	//kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

void testApp::ofxUI_init()
{
    float dim = 24;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;
    
    gui = new ofxUICanvas(ofGetWidth() - length , 0, length+xInit, ofGetHeight());
	
    gui->addWidgetDown(new ofxUILabel("KINECT SLIDERS", OFX_UI_FONT_MEDIUM));
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 1000);
    gui->addWidgetDown(new ofxUIRangeSlider(length-xInit,dim, 0.0, 255.0, nearThreshold, farThreshold, "DEPTH RANGE"));
    gui->addSlider("KINECT MOTOR ANGLE", -30 , 30 , angle, length-xInit,dim);
    //gui->addSlider("MAX NUM BLOBS", 0.0, 25, maxBlobs, length-xInit,dim);
    gui->addWidgetDown(new ofxUIRangeSlider(length-xInit,dim,10, kinect.getWidth() * kinect.getHeight() , minBlobSize , maxBlobSize  , "BLOB SIZE"));
    gui->addToggle( "OPENCV THRESHOLD", bThreshWithOpenCV , dim, dim);
    gui->addToggle( "DRAW POINT CLOUD", bDrawPointCloud, dim, dim);
    gui->addToggle( "FIND CONTOUR HOLES", bFindContourHoles, dim, dim);
    
    gui->addSlider("FBO FADE", 0.0, 255.0f , fboFade , length-xInit,dim);
    gui->addSlider("OUTLINE SCALE", 0.5, 3.0f , outlineScale , length-xInit,dim);
    gui->addSlider("OUTLINE SMOOTHING", 1.0, 100.0f , outlineSmoothing , length-xInit,dim);
    gui->addSlider("PERCENT SCALE PER LAYER", 0.0f, 1.0f , percPerLayer , length-xInit,dim);
    gui->addSlider("NUM LAYERS", 0.0f, 35.0f , numOutlineLayers , length-xInit,dim);

    gui->addSlider("SLIDE DELAY", 0.15f, 15.0f , slideDelay , length-xInit,dim);
    
    gui->toggleVisible() ; 
  
  
    ofAddListener( gui->newGUIEvent,this,&testApp::guiEvent );
    gui->loadSettings( "GUI/kinectSettings.xml" ) ;
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();

	//Sliders
	if(name == "KINECT MOTOR ANGLE" )
    {
        angle =  ( (ofxUISlider *) e.widget )->getScaledValue() ;
        kinect.setCameraTiltAngle( angle ) ;
    }
    if(name == "MAX NUM BLOBS" ) maxBlobs =  ( (ofxUISlider *) e.widget )->getScaledValue() ;
    if(name == "FBO FADE" ) fboFade =  ( (ofxUISlider *) e.widget )->getScaledValue() ;
    if(name == "OUTLINE SCALE" ) outlineScale =  ( (ofxUISlider *) e.widget )->getScaledValue() ;
    if(name == "OUTLINE SMOOTHING" ) outlineSmoothing =  ( (ofxUISlider *) e.widget )->getScaledValue() ;
    if(name == "PERCENT SCALE PER LAYER" ) percPerLayer =  ( (ofxUISlider *) e.widget )->getScaledValue() ;
    if(name == "NUM LAYERS" ) numOutlineLayers =  ( (ofxUISlider *) e.widget )->getScaledValue() ;
    if(name == "SLIDE DELAY") slideDelay =  ( (ofxUISlider *) e.widget )->getScaledValue() ;

    //Toggles
    if(name == "OPENCV THRESHOLD" ) bThreshWithOpenCV = ( (ofxUIToggle *) e.widget )->getValue() ;
	if(name == "DRAW POINT CLOUD" ) bDrawPointCloud = ( (ofxUIToggle *) e.widget )->getValue() ;
    if(name == "FIND CONTOUR HOLES" ) bFindContourHoles = ( (ofxUIToggle *) e.widget )->getValue() ;
    
    //Range Sliders
    if(name == "DEPTH RANGE" )
    {
        farThreshold = ( (ofxUIRangeSlider *) e.widget )->getScaledValueLow() ;
        nearThreshold = ( (ofxUIRangeSlider *) e.widget )->getScaledValueHigh() ;
    }
    
    if(name == "BLOB SIZE" )
    {
        minBlobSize = ( (ofxUIRangeSlider *) e.widget )->getScaledValueLow() ;
        maxBlobSize = ( (ofxUIRangeSlider *) e.widget )->getScaledValueHigh() ;
    }

    gui->saveSettings( "GUI/kinectSettings.xml" ) ; 
    //gui->addSlider("SLIDE DELAY", 0.0f, 15.0f , slideDelay , length-xInit,dim);

}


//--------------------------------------------------------------
void testApp::keyPressed (int key) {
    
    cout << "keyPressed :: " << key << endl ; 
	switch (key)
    {
		case 'g':
        case 'G':
            gui->toggleVisible() ;
            break ;
            
        case 'f':
        case 'F':
            bFullscreen = !bFullscreen ;
            ofToggleFullscreen() ;
            break ;
        /*
         356
         keyPressed :: 358
         */
        case 358 :
            incrementShader() ;
            break ; 
	}
}



//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
    
    if ( gui->isVisible() == true ) return ;
    
    bRoiDragging = true ;
    
    roiArea.width = x - roiArea.x - drawOffsetX ;
    roiArea.height = y - roiArea.y - drawOffsetY ;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    if ( gui->isVisible() == true ) return ;
    
    bRoiReady = false ;
    bRoiDragging = true ; 
    roiArea = ofRectangle( x - drawOffsetX , y - drawOffsetY  , 0 , 0 ) ;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
    if ( gui->isVisible() == true ) return ;
    
    bRoiReady = true ;
    bRoiDragging = false ; 
    roiArea.width = x - roiArea.x - drawOffsetX ;
    roiArea.height = y - roiArea.y - drawOffsetY ;
    saveCalibrationXml() ;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}