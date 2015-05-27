#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

// Windows users:
// You MUST install the libfreenect kinect drivers in order to be able to use
// ofxKinect. Plug in the kinect and point your Windows Device Manager to the
// driver folder in:
//
//     ofxKinect/libs/libfreenect/platform/windows/inf
//
// This should install the Kinect camera, motor, & audio drivers.
//
// You CANNOT use this driver and the OpenNI driver with the same device. You
// will have to manually update the kinect device to use the libfreenect drivers
// and/or uninstall/reinstall it in Device Manager.
//
// No way around the Windows driver dance, sorry.

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class ofApp : public ofBaseApp {
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

	unsigned char * pix;
	
	ofxCvContourFinder contourFinder;
	
	bool bThreshWithOpenCV;
	bool displayDebug;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
	int w;
	int h;
	int step;
	int counter;

	float camDistane;

	//8 bits red, 8 bits green, 8 bits blue, from 0 to 255 in 256 steps
	ofFbo rgbaFbo; // with alpha
		
	//32 bits red, 32 bits green, 32 bits blue, from 0 to 1 in 'infinite' steps	
	ofFbo rgbaFboFloat; // with alpha

	
	// used for viewing the point cloud
	ofEasyCam easyCam;
};
