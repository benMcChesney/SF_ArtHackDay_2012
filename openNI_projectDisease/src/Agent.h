//
//  User.h
//  scavengerMap
//
//  Created by Ben McChesney on 6/20/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _AGENT
#define _AGENT



#include "ofMain.h"
#include <vector>

class Agent {
    
	public:

        ofVec2f target, position, velocity, force, acceleration, lastPosition ; 
    
        float maxVelocity , maxForce ; 
        bool changeTarget ;
		int prevIndex ; 
    	int targetIndex ; 
		float bufferDistance ; 

		unsigned char r,g,b ; 
        ofColor color ;
    
        Agent()
        {
            
        }
    
        void setup( ofVec2f pos , ofVec2f tar, int _targetIndex )
        {
			targetIndex = _targetIndex ;
			prevIndex = 0 ; 
			bufferDistance = 5.0f ; 
            velocity = ofVec3f( (ofRandom(0.1f, 0.4f), ofRandom(0.1f, 0.4f), 0.0f) );
            acceleration = ofVec3f( .4 , .4 , .4 ) ; 
            position = pos ; 
            target = tar ; 
            force = ofVec3f ( 0 , 0 , 0 ) ; 
            maxVelocity = ofRandom( 3, 6 )  ;
            maxForce = ofRandom ( .05 , .1 )  ;
            
            force = ofVec3f( ofRandom ( 0 , TWO_PI )  ,  ofRandom ( 0 , TWO_PI )  ,  ofRandom ( 0, TWO_PI )  ) ;
			r = ofRandom ( 0 , 255 ) ; 
			g = ofRandom ( 0 , 255 ) ; 
			b = ofRandom ( 0 , 255 ) ;
  
        }
    
        void update( ) 
        {
			lastPosition = position ; 
            force.limit(maxForce);
            velocity += force;
            
            velocity.limit(maxVelocity);
            position += velocity;
            
            seek ( target ) ; 
            /*
            speed = target - position ; 
            speed.limit( maxSpeed ) ; 
            position += speed ;
            */
            float dist = position.distance( target ) ; 
            if ( dist < bufferDistance ) 
            {
             //   max
                changeTarget = true ;
            }
			
        }
    
    
    void addForce ( ofVec2f _force ) 
    {
        force += _force ; 
    }
    
    void seek(ofVec2f oTarget) {
        // This is how it works.
        // 1. We calculate the change in position which would bring
        // use directly to the target. This is "desired_vel". Though we don't
        // want to jump directly to this target, but slowly move to it.
        ofVec2f desiredVelocity = oTarget - position;
        
        // 2. Therefore we want to move to this position at the maximum
        // allowed speed. We do this by normalizing the vector and then
        // multiplying it with the maximum allowed speed. This is exactly
        // what ofxVec3f.scale() does.
        desiredVelocity.scale(maxVelocity);
        
        // 3. Now we have the maximum desired velocity at the maximum speed.
        // Though we need to adjust this speed as we want to go into that direction
        // at the best we are allowed, which is the remaining velocity.
        desiredVelocity -= velocity;
        
        addForce(desiredVelocity);
        
        //cout << "Desired vel: " << desired_vel.x << ", " << desired_vel.y << endl;
    }

	void flee(ofVec2f oTarget) {
        // This is how it works.
        // 1. We calculate the change in position which would bring
        // use directly to the target. This is "desired_vel". Though we don't
        // want to jump directly to this target, but slowly move to it.
        ofVec2f desiredVelocity =  position + oTarget ;
        
        // 2. Therefore we want to move to this position at the maximum
        // allowed speed. We do this by normalizing the vector and then
        // multiplying it with the maximum allowed speed. This is exactly
        // what ofxVec3f.scale() does.
        desiredVelocity.scale(maxVelocity);
        
        // 3. Now we have the maximum desired velocity at the maximum speed.
        // Though we need to adjust this speed as we want to go into that direction
        // at the best we are allowed, which is the remaining velocity.
        desiredVelocity -= velocity;
        
        addForce(desiredVelocity);
        
        //cout << "Desired vel: " << desired_vel.x << ", " << desired_vel.y << endl;
    }
    
    void draw() 
    {
        ofEnableSmoothing();
        glPushMatrix();
        glTranslatef(position.x, position.y, 1 );
        
        //img.draw(0,0);
        glPopMatrix();
        ofSetLineWidth(3);
        
        float arrow_len = 14;
        ofVec3f arrow_end = position +(velocity.normalized() * arrow_len);
        ofFill();
        ofEnableSmoothing();
        //ofSetHexColor ( hexColor ) ; 

        ofLine(position.x, position.y, arrow_end.x, arrow_end.y);
        //ofCircle(arrow_end.x, arrow_end.y, 3);
        
        //ofSetColor(0xFFFFFF);
        ofCircle(position.x, position.y, 5);
        ofNoFill();
        
        ofEnableSmoothing();
        
        //ofSetColor(0xFFFFFF);
        //ofSetLineWidth(20);
        ofCircle(position.x, position.y, 5);
        
        
        ofSetColor(0x00);

        ofCircle(position.x, position.y, 7);
        
        
        ofDisableSmoothing();
        
        
        ofEnableAlphaBlending() ; 
        
        ofNoFill() ; 
        //printf( locations.size() ) ;
        ofCircle( position.x , position.y , 2 ) ; 
    }
};

#endif 