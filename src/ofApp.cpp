#include "ofApp.h"
#include <assert.h>
//--------------------------------------------------------------

void ofApp::exit() {

}

void ofApp::setup(){
	FILE* config=fopen("configuration.txt","r");
	float windowHeight = 0.5;
	RANGE = 25;
	int firstNote = 48;
	xStability = 1.f;
	vibTargetWidth = pow(2, 0.4 / 12) - 1.f;
	vibWidth = 0;
	vibratoFrequency = 6;
	if (config == NULL) {
		printf("configuration.txt not found! Setting Default parameters.\n");
	}
	else { 
		char parName[30];
		float temp;
		printf("loading Parameters from file:\n");
		while (!feof(config)) {
			fscanf(config, "%s %f", parName, &temp);
			if (strcmp(parName, "semitones") == 0) {
				RANGE = (int)temp;
				printf("Range set to %d semitones.\n", RANGE);
			}
			else if (strcmp(parName, "windowHeight") == 0) {
				windowHeight = temp;
				printf("Height set to %d.\n", (int)(windowHeight*ofGetScreenHeight()));
			}
			else if (strcmp(parName, "firstMidiNote") == 0) {
				firstNote = (int)temp;
				printf("first note set to %d.\n", firstNote);
			}
			else if (strcmp(parName, "slopeXStability") == 0) {
				xStability = temp / 180.f * PI;
				printf("xStabilitySlope set to %d.\n", (int)temp);
			}
			else if (strcmp(parName, "vibratoWidth") == 0) {
				vibTargetWidth = pow(2, temp / 12.f) - 1.f;
				printf("vibrato Width set to %f semitones.\n", temp);
			}
			else if (strcmp(parName, "vibratoFrequency") == 0) {
				vibratoFrequency = temp;
				printf("vibrato Frequency set to %fHz.\n", vibratoFrequency);
			}
			else if (strcmp(parName, "harmonics") == 0) {
				harmonics.push_back(temp/1000.f);
				printf("Harmoninc %d set to %d\n", harmonics.size(), (int)temp);
				char tempH = fgetc(config);
				while (tempH != '\n' && tempH != '\r' && !feof(config)) {
					fscanf(config, "%f", &temp);
					harmonics.push_back(temp/1000.f);
					printf("Harmoninc %d set to %d\n", harmonics.size(), (int)temp);
					tempH = fgetc(config);
				}
			}
			fscanf(config, "\n");
		}
	}
	if (harmonics.empty()) {
		harmonics = { 0.8f, 0.1f, 0.02f, 0.01f ,0.005f , 0.002f, 0.001f };
	}
	harmonicsVowel = harmonics;
	ofBackground(34, 34, 34);

	// 2 output channels,
	// 0 input channels
	// 22050 samples per second
	// 512 samples per buffer
	// 4 num buffers (latency)
	
	int bufferSize		= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	vibPhase = 0.f;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	volume				= 0.1f;
	bNoise 				= false;
	vibPhaseAdder = (vibratoFrequency / (float)sampleRate) * TWO_PI;
	pan = 0.5f;
	lAudio.assign(bufferSize, 0.0);
	rAudio.assign(bufferSize, 0.0);
	
	soundStream.printDeviceList();
	
	//if you want to set the device id to be different than the default
	//soundStream.setDeviceID(1); 	//note some devices are input only and some are output only 

	soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);

	// on OSX: if you want to use ofSoundPlayer together with ofSoundStream you need to synchronize buffersizes.
	// use ofFmodSetBuffersize(bufferSize) to set the buffersize in fmodx prior to loading a file.
	
	ofSetFrameRate(60);

	targetVolume = 0.8;
	
	xOffset = 0;
	ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight()*windowHeight);
	ofSetWindowPosition(0, (ofGetScreenHeight()-ofGetWindowHeight())/2.f);
	ofHideCursor();
	firstFreq = 440.f * pow(2, (float)(firstNote-69.5) / 12.f);
	vowelMoves = 0.f;
}


//--------------------------------------------------------------
void ofApp::update(){

	targetVolume = (float)(ofGetWindowHeight() - mouseY) / (float)ofGetHeight();

	if (targetVolume > 0.98)
		targetVolume = 0.98;
	if (targetVolume < 0.1)
		targetVolume = 0;

	float widthPct = (float)(mouseX - xOffset) / (float)ofGetWidth();
	steadyVertical(); //we have to remember each time wich is our offset
	getVibrato();
	prMouse[0] = mouseX;
	prMouse[1] = mouseY;
	targetFrequency = firstFreq*pow(2, widthPct* RANGE / 12.f);//half semitone under DO_3
	phaseAdderTarget = targetFrequency / (float)sampleRate * TWO_PI;
	vibWidth = 0.9*vibWidth + 0.1*autoVibrato*targetFrequency*vibTargetWidth / TWO_PI/2.0;
	vowelMoves += 0.02;
	if (vowelMoves >= TWO_PI)
		vowelMoves = 0;
	//envelopCenter = abs(cos(vowelMoves)) * targetFrequency*harmonics.size()/2.f+ targetFrequency;
	for (int i = 0; i < harmonics.size(); i++) {
		float dist = targetFrequency*pow(i,1.5f);
		harmonicsVowel[i] = harmonics[i] * (1.f/pow(2.7f,dist/3000.f));
	}
}
void ofApp::getVibrato() {
	if (abs(mouseX-prMouse[0])> ofGetScreenWidth()*0.003f)
		autoVibrato = 0.f;
	else if (autoVibrato < 1)
		autoVibrato += 0.01;
}
void ofApp::steadyVertical() {
	float angle = atan((float)abs(prMouse[1] - mouseY) / (float)abs(prMouse[0] - mouseX));
	if (angle > xStability && (prMouse[0]+prMouse[1]) !=0) {
		xOffset += mouseX - prMouse[0];
	}
	/*else
		xOffset = 0.f;*/
	
}
//--------------------------------------------------------------
void ofApp::draw(){
	ofSetLineWidth(1);
	for (int i = 0; i < (int)RANGE; i++) {
		int semi = i % 12;
		if (semi == 0 || semi == 2 || semi == 4 || semi == 5 || semi == 7 || semi == 9 || semi == 11)
			ofSetColor(255);
		else
			ofSetColor(0);
		//ofRectangle(i*ofGetWidth() / RANGE, 0, ofGetWidth() / RANGE, ofGetHeight());
		ofFill();
		ofDrawRectangle(i*ofGetWidth() / RANGE, 0, ofGetWidth() / RANGE, ofGetHeight());
		ofSetColor(255, 0, 0);
		ofLine(i*ofGetWidth() / RANGE, 0, i*ofGetWidth() / RANGE, ofGetHeight());
	}

	ofSetColor(0,255,0);

	ofNoFill();

	ofSetLineWidth(1);
	ofSetColor(120, 120, 120);
	ofPoint pointer;

	pointer.y = mouseY;


	pointer.x = mouseX - xOffset;
	
	//ofCircle(ofPoint(gaze[0]*ofGetScreenWidth()-ofGetWindowPositionX(), gaze[1]*ofGetScreenHeight()-ofGetWindowPositionY()), ofGetScreenHeight()*0.06);
	
	//ofLine(ofPoint(0, gaze[1] * ofGetScreenHeight() - ofGetWindowPositionY()), ofPoint(ofGetWidth(), gaze[1] * ofGetScreenHeight() - ofGetWindowPositionY()));
	//ofLine(ofPoint((0.5 - head[1]) * ofGetWidth(), 0), ofPoint((0.5 - head[1]) * ofGetWidth(), ofGetHeight()));
	//ofCircle(ofPoint((0.5-head[1]) * ofGetWidth(), (0.5-head[0]) * ofGetHeight()), head[2]*ofGetWidth());
	//ofDrawRectangle(0, myTobii.eventParams.Y*1.1, ofGetWidth(), myTobii.eventParams.Y*0.9);
	ofSetColor(120, 120, 120);
	for (int i = 0; i < (int)RANGE; i++) {
		ofFill();
		for (int y = 0; y < ofGetHeight(); y += ofGetScreenHeight()*0.05) {
			ofCircle(i * ofGetWidth() / RANGE, y, ofGetHeight()*0.002);
			ofCircle((i + 0.25)*ofGetWidth() / RANGE, y, ofGetHeight()*0.002);
			ofCircle((i + 0.5)*ofGetWidth() / RANGE, y , ofGetHeight()*0.004);
			ofCircle((i + 0.75)*ofGetWidth() / RANGE, y , ofGetHeight()*0.002);
		}
	}
	ofNoFill();
	ofCircle(pointer, ofGetScreenWidth()*0.01);
	/*ofDrawBitmapString(ofToString(targetFrequency), 10, 10);
	ofDrawBitmapString(ofToString(envelopCenter), 10, 50);
	for (int i = 0; i < harmonicsVowel.size(); i++) {
		ofDrawBitmapString(ofToString(harmonicsVowel[i]), 100, 10+i*25);
	}*/
}



//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
	if (key == '-' || key == '_' ){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} else if (key == '+' || key == '=' ){
		volume += 0.05;
		volume = MIN(volume, 1);
	}
	
	if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}
	if (key == 'f') {
		ofToggleFullscreen();
	}
}


//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels){
	float leftScale = 1 - pan;
	float rightScale = pan;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI) {
		phase -= TWO_PI;
	}
	while (vibPhase > TWO_PI) {
		vibPhase -= TWO_PI;
	}
	
	
	phaseAdder = phaseAdderTarget;
	volume = 0.8*volume + 0.2*targetVolume;
	for (int i = 0; i < bufferSize; i++) {
		vibPhase += vibPhaseAdder;
		phase += phaseAdder;
		float cosPhase = cos(vibPhase)*vibWidth;
		float sample = 0.f;
		for (int j = 0; j<harmonicsVowel.size(); j++) {
			sample += harmonicsVowel[j]*sin((j+1) *(phase + cosPhase));
		}
		lAudio[i] = output[i*nChannels] = sample * volume * leftScale;
		rAudio[i] = output[i*nChannels + 1] = sample * volume * rightScale;
	}


}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	xOffset = 0.f;
}
