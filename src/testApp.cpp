#include "testApp.h"
#include "maximilian.h"/* include the lib */

using namespace ofxCv;

//-------------------------------------------------------------
testApp::~testApp() {
	delete ts;
}


void testApp::setup() {
    samp.load(ofToDataPath("sounds/PanamaAlwaysClassixxRemix3.wav"));

    px = 300;
	py = 300;
	vx = 0;
	vy = 0;
    
    canvas_width = 640;
    canvas_height = 480;
	
	nBandsToGet = 128;
    
    ts = new maxiPitchStretch<grainPlayerWin>(&samp);
	stretches.push_back(ts);
	speed = 1;
    rate = 2;
	grainLength = 0.05;
	current=0;
    
    sampleRate 			= 44100; /* Sampling Rate */
	initialBufferSize	= 512;	/* Buffer Size. you have to fill this buffer with sound*/
	lAudioOut			= new float[initialBufferSize];/* outputs */
	rAudioOut			= new float[initialBufferSize];
	lAudioIn			= new float[initialBufferSize];/* inputs */
	rAudioIn			= new float[initialBufferSize];
    
    fftSmoothed = new float[8192];
	for (int i = 0; i < 8192; i++){
		fftSmoothed[i] = 0;
	}
    
    ofSetVerticalSync(true);
	ofSetFrameRate(120);
	finder.setup("haarcascade_frontalface_alt2.xml");
	finder.setPreset(ObjectFinder::Fast);
	finder.getTracker().setSmoothingRate(.3);
    cam.initGrabber(canvas_width, canvas_height);
	sunglasses.loadImage("images/sun_glasses.png");
	ofEnableAlphaBlending();
    
    
    /* This is a nice safe piece of code */
	memset(lAudioOut, 0, initialBufferSize * sizeof(float));
	memset(rAudioOut, 0, initialBufferSize * sizeof(float));
	
	memset(lAudioIn, 0, initialBufferSize * sizeof(float));
	memset(rAudioIn, 0, initialBufferSize * sizeof(float));
	
	fft.setup(1024, 512, 256);
	oct.setup(44100, 1024, 10);
	
	ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
	ofSoundStreamSetup(2,0, this, maxiSettings::sampleRate, initialBufferSize, 4);/* Call this last ! */
	
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofEnableSmoothing();

}

void testApp::update() {
	cam.update();
    
    ofSoundUpdate();
    
    px += vx;
	py += vy;

    // (2) check for collision, and trigger sounds:
	// horizontal collisions:
	if (px < 0){
		px = 0;
		vx *= -1;
	} else if (px > ofGetWidth()){
		px = ofGetWidth();
		vx *= -1;
	}
	// vertical collisions:
	if (py < 0 ){
		py = 0;
		vy *= -1;
	} else if (py > ofGetHeight()){
		py = ofGetHeight();
		vy *= -1;
	}
    
	// (3) slow down velocity:
	vx 	*= 0.996f;
	vy 	*= 0.996f;
    
	// (4) we use velocity for volume of the samples:    
	if(cam.isFrameNew()) {
		finder.update(cam);
	}
    
    float vel = sqrt(vx*vx + vy*vy);
    float * val = ofSoundGetSpectrum(nBandsToGet);		// request 128 values for fft
    
	for (int i = 0;i < nBandsToGet; i++){
		
		// let the smoothed calue sink to zero:
		fftSmoothed[i] *= 0.96f;
		
		// take the max, either the smoothed or the incoming:
		if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];
		
	}

}

void testApp::draw() {
    
    ofEnableAlphaBlending();
    ofSetColor(255,255,255,0);
    ofRect(0,ofGetHeight(),10*128,200);
	ofDisableAlphaBlending();
	
	// draw the fft resutls:
	ofSetColor(255,255,255,255);
	
	float width = (float)(5*128) / nBandsToGet;
	for (int i = 0;i < nBandsToGet; i++){
		// (we use negative height here, because we want to flip them
		// because the top corner is 0,0)
		ofRect(100+i*width,ofGetHeight()-100,width,-(fftSmoothed[i] * 200));
	}
	
	// finally draw the playing circle:
	ofEnableAlphaBlending();
    ofSetColor(255,255,255,20);
    ofCircle(px, py,50);
	ofDisableAlphaBlending();
	
	ofSetHexColor(0xffffff);
	ofCircle(px, py,8);
    
	cam.draw(0, 0);
	
	for(int i = 0; i < finder.size(); i++) {
		ofRectangle object = finder.getObjectSmoothed(i);
        
        pos_x = object.x;
        pos_y = object.y;
        
        volume = (canvas_width - object.x) / canvas_width;
        mapped_volume = ofMap( volume, 0.3, 1, 0, 10);
        bass = (canvas_height - object.y);
        
        rate = ((double ) bass / ofGetHeight() * 4.0) - 2.0;
        
        mapped_rate = ofMap( rate, -2, 2, 0, 2);
        
		sunglasses.setAnchorPercent(.5, .5);
		float scaleAmount = .85 * object.width / sunglasses.getWidth();
		ofPushMatrix();
		ofTranslate(object.x + object.width / 2., object.y + object.height * .42);
		ofScale(scaleAmount, scaleAmount);
		sunglasses.draw(0, 0);
		ofPopMatrix();
		ofPushMatrix();
		ofTranslate(object.getPosition());
		ofDrawBitmapStringHighlight(ofToString(finder.getLabel(i)), 0, 0);
		ofLine(ofVec2f(), toOf(finder.getVelocity(i)) * 10);
		ofPopMatrix();
	}
}


//--------------------------------------------------------------
void testApp::audioRequested 	(float * output, int bufferSize, int nChannels){
    speed = 1;
	for (int i = 0; i < bufferSize; i++){
        // wave = stretches[current]->play(speed, grainLength, 5, 0);
        // wave = stretches[current]->play(speed*2, rate, 0.1, 4, 0);
        wave = stretches[current]->play(speed, mapped_rate, 0.1, 4, 0);
        // wave = stretches[current]->play2(pos, 0.1, 4);
		if (fft.process(wave)) {
			oct.calculate(fft.magnitudes);
		}
		
		//play result
		mymix.stereo(wave, outputs, 0.5);
		lAudioOut[i] = output[i*nChannels    ] = outputs[0] * mapped_volume; /* You may end up with lots of outputs. add them here */
		rAudioOut[i] = output[i*nChannels + 1] = outputs[1] * mapped_volume;
	}
}

//--------------------------------------------------------------
void testApp::audioReceived 	(float * input, int bufferSize, int nChannels){
	/* You can just grab this input and stick it in a double, then use it above to create output*/
	for (int i = 0; i < bufferSize; i++){
		/* you can also grab the data out of the arrays*/
		lAudioIn[i] = input[i*2];
		rAudioIn[i] = input[i*2+1];
	}
}
