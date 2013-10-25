#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxAudioUnit.h"

#include "ofxMaxim.h"
#include "maxiGrains.h"
#include <sys/time.h>

typedef hannWinFunctor grainPlayerWin;

class testApp : public ofBaseApp {
public:
    ~testApp();/* deconsructor is very useful */
	void setup();
	void update();
	void draw();
	
	ofVideoGrabber cam;
    ofSoundPlayer panama;
	ofxCv::ObjectFinder finder;
	ofImage sunglasses;
    
    float pos_x;
    float pos_y;
    
    float volume;
    float bass;
    float canvas_width;
    float canvas_height;
    
    void mouseMoved(int x, int y );
    
    void audioRequested 	(float * input, int bufferSize, int nChannels); /* output method */
	void audioReceived 	(float * input, int bufferSize, int nChannels); /* input method */
	
	float 	* lAudioOut; /* outputs */
	float   * rAudioOut;
    
	float * lAudioIn; /* inputs */
	float * rAudioIn;
	
	int		initialBufferSize; /* buffer size */
	int		sampleRate;
    
    /* stick your maximilian stuff below */
	
	double wave,sample,outputs[2];
	maxiSample samp;
	vector<maxiPitchStretch<grainPlayerWin>*> stretches;
	maxiMix mymix;
	maxiPitchStretch<grainPlayerWin> *ts;
	double speed, grainLength, rate;
	
	ofxMaxiFFT fft;
	ofxMaxiFFTOctaveAnalyzer oct;
	int current;
	double pos;
    
    float 				* fftSmoothed;
    // we will draw a rectangle, bouncing off the wall:
    float 				px, py, vx, vy;
    
    int nBandsToGet;
    float prevx, prevy;
//    		void mouseDragged(int x, int y, int button);
};
