#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofBackground(0,0,0);
	ofSetFrameRate(60);

	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();

	kinect.setLed(ofxKinect::LED_OFF);
	
	ofSetFrameRate(60);
	
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
	displayDebug = true;

	nearThreshold = 200;
	farThreshold = 1250;

	w = 640;
	h = 480;
	step = 4;
	counter = 0;
	camDistane = 0;

	pix = new unsigned char[(w*h) / step];

	rgbaFbo.allocate(1280, 800, GL_RGBA);

	#ifdef TARGET_OPENGLES
		rgbaFboFloat.allocate(1280, 800, GL_RGBA ); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
        ofLogWarning("ofApp") << "GL_RGBA32F_ARB is not available for OPENGLES.  Using RGBA.";	
	#else
        rgbaFboFloat.allocate(1280, 800, GL_RGBA32F_ARB); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
	#endif

	rgbaFbo.begin();
		ofClear(0);
    rgbaFbo.end();
	
	rgbaFboFloat.begin();
		ofClear(0);
    rgbaFboFloat.end();
}

//--------------------------------------------------------------
void ofApp::update() {
	
	kinect.update();

	ofEnableAlphaBlending();
	
	//lets draw some graphics into our two fbos
    rgbaFbo.begin();
		drawPointCloud();
    rgbaFbo.end();
	  	
    rgbaFboFloat.begin();	
		drawPointCloud();		
	rgbaFboFloat.end();
}

//--------------------------------------------------------------
void ofApp::draw() {

	rgbaFbo.draw(0,0);
    rgbaFboFloat.draw(0,0);
	
	// draw instructions
	if (displayDebug) {
		ofSetColor(255, 255, 255);
		stringstream reportStream;
	        
		reportStream << "press p to switch between debug mode" << endl
		<< "set near threshold " << nearThreshold << " (press: + -)" << endl
		<< "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
		<< ", fps: " << ofGetFrameRate() << endl
		<< "distance: " << easyCam.getDistance() << ", drag: " << easyCam.getDrag() << " , angle: " << angle << endl
		<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl;

		ofDrawBitmapString(reportStream.str(), 20, 632);
	}
}

/**
	Creates a char array from depth data
	quicker, and ready for opencv analysis
**/
void ofApp::drawPointCloud() {

	int z = 0;

	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_LINE_STRIP);

	// y axis first to match char array
	for(int x = 0; x < w; x += step) {
		for(int y = 0; y < h; y += step) {

			z = kinect.getDistanceAt(x, y);
			pix[counter] = 0;

			// check within threshold
			if (z > 0 && z > nearThreshold && z < farThreshold) {

				pix[counter] = 255;

				mesh.addVertex(kinect.getWorldCoordinateAt(x,y));
				mesh.addColor(ofColor(ofMap(z, nearThreshold, farThreshold, 0, 255), ofMap(z, 0, nearThreshold, 0, 255), 101));
			}

			counter++;
		}
	}

	counter = 0;

	ofFill();
	ofSetColor(255,255,255, 10);
	ofRect(0,0,1280,800);

	//2 - Draw graphics
	ofNoFill();
	ofSetColor(255,255,255);

	easyCam.begin();
		glPointSize(3);
		ofPushMatrix();
			// ofEnableBlendMode(OF_BLENDMODE_ADD);
			// the projected points are 'upside down' and 'backwards' 
			ofScale(1, -1, -1);
			ofTranslate(0, 0, -1000); // center the points a bit
			// ofEnableDepthTest();
			mesh.drawVertices();
			// ofDisableDepthTest();
		ofPopMatrix();
	easyCam.end();
}

//--------------------------------------------------------------
void ofApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {
		case'p':
			displayDebug = !displayDebug;
			break;
			
		case '>':
		case '.':
			farThreshold +=50;
			if (farThreshold > 5000) farThreshold = 5000;
			break;
			
		case '<':
		case ',':
			farThreshold -=50;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold +=50;
			if (nearThreshold > 2000) nearThreshold = 2000;
			break;
			
		case '-':
			nearThreshold -=50;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
		
		case '[':
			camDistane-=0.1;
			if (camDistane < 0) camDistane = 0;
			easyCam.setDistance(camDistane);
			break;

		case ']':
			camDistane+=0.1;
			if (camDistane > 1) camDistane = 1;
			easyCam.setDistance(camDistane);
			break;

		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{}
