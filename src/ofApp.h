#pragma once

#include "ofMain.h"
class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		void exit();
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		void audioOut(float * input, int bufferSize, int nChannels);
		void steadyVertical();
		void getVibrato();
		ofSoundStream soundStream;

		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	volume;
		float targetVolume;

		vector <float> lAudio;
		vector <float> rAudio;
		
		//------------------- for the simple sine wave synthesis
		float 	targetFrequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;

		float   vibPhase;
		float 	vibPhaseAdder;
		float vibWidth;
		int pitchInput, VolumeInput,vibratoInput;
		int xOffset;
		int prMouse[2];
		float autoVibrato;
		int RANGE;
		float firstFreq;
		float xStability;
		float vibratoFrequency;
		float vibTargetWidth;
		vector<float> harmonics;
		vector<float> harmonicsVowel;
		float vowelMoves;
		float envelopCenter;
};
