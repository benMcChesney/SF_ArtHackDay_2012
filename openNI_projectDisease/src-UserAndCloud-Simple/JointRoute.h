//
//  JointRoute.h
//  openNiSample007
//
//  Created by Ben McChesney on 12/14/12.
//
//

#ifndef openNiSample007_JointRoute_h
#define openNiSample007_JointRoute_h

#include "ofMain.h"

class JointRoute
{
    public :
        JointRoute ( ) { }
        ~JointRoute ( ) { }
    
        vector<int> targetIndicies ;
        void addIndex ( int _index )
        {
            targetIndicies.push_back ( _index ) ;
        }
    
        int getRandomTargetIndex ( )
        {
            /*
            cout << " <<<<<<<<<< " << endl ;
            for ( int i = 0 ; i < targetIndicies.size() ; i++ )
            {
                cout << "target Indicies @ " << i << targetIndicies[i] << endl ; 
            }*/
            int index = targetIndicies[ (int) ofRandom( targetIndicies.size() ) ] ;
           // cout << "CHOSEN INDEX : " << index << endl ;
            return index ;
        }
};

#endif
