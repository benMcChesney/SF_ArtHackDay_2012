//
//  ShaderEffect.h
//  ofxKinectExample
//
//  Created by Ben McChesney on 12/15/12.
//
//

#ifndef ofxKinectExample_ShaderEffect_h
#define ofxKinectExample_ShaderEffect_h

#include "ofMain.h"
#include "ofxSimpleMask.h"

class ShaderEffect
{
    public :
        ShaderEffect ( ) { }
        ~ShaderEffect ( ) { }
    
        bool bActive ; 
        void setup ( string shaderPath )
        {
            shader.load("shaders/Basic.vert" , shaderPath );
            shaderFbo.allocate( ofGetWidth() , ofGetHeight() ) ;
            
            shaderFbo.begin() ;
            ofClear( 1 , 1 , 1 , 0 ) ;
            shaderFbo.end() ;
            
            maskFbo.allocate( ofGetWidth() , ofGetHeight() ) ;
            maskFbo.begin() ;
            ofClear( 1 , 1 , 1, 0 ) ;
            maskFbo.end() ;
            
            mask.setup("shaders/composite_rgb", ofRectangle( 0 , 0, ofGetWidth() , ofGetHeight() ) ) ;
            bActive = false ; 
        }
    
        void update ( )
        {
            
        }
    
        void beginFbo ( )
        {
            maskFbo.begin() ;
            ofClear( 1 , 1 , 1 , 0 ) ;
            bActive = true ; 
            
        }
    
        void endFbo ( )
        {
            maskFbo.end( ) ; 
        }
        void draw ( )
        {
            if ( bActive == false ) return ;
            
            shaderFbo.begin() ;
                ofClear( 1 , 1 , 1 , 0 ) ;
                shader.begin() ;
                
                //we want to pass in some varrying values to animate our type / color
                shader.setUniform1f("time", ofGetElapsedTimef() );
                shader.setUniform2f("resolution", ofGetWidth() , ofGetHeight() );
            
                ofSetColor( 255 ,255 , 255 ) ;
                ofRect ( 0 , 0 , ofGetWidth() , ofGetHeight()) ;
                shader.end();
            shaderFbo.end() ;
            
            ofSetColor( 255 , 255 , 255 ) ;
            mask.drawMask(shaderFbo.getTextureReference(), maskFbo.getTextureReference(), 0, 0, 255.0f ) ;

        }
    
        ofShader shader ;
        ofFbo shaderFbo ;
        
        ofxSimpleMask mask ;
        ofFbo maskFbo ;
}; 

#endif
