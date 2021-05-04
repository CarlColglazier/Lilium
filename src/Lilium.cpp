#include "daisy_patch.h"
#include <math.h>
#include <string>

#define BUFF_SIZE 400000

using namespace daisy;

DaisyPatch patch;
float sample_rate;
int seek;
bool recording;

// main buffer
static float DSY_SDRAM_BSS sdbuff[BUFF_SIZE];

void iterSeek(int &s) {
	s++;
	if (s > BUFF_SIZE) {
		s = 0;
	}
}

void UpdateControls() {
	// just pass along the values
	//patch.UpdateAnalogControls();
	//patch.DebounceControls();
	patch.ProcessDigitalControls();

	recording = patch.gate_input[patch.GATE_IN_1].State();
}

void UpdateOled() {
	patch.display.Fill(false);
	patch.display.SetCursor(0, 20);
	std::string str = "recording";
	char* s = &str[0];
	if (recording) {
		patch.display.WriteString(s, Font_11x18, true);
	}
	patch.display.Update();
}


static void AudioThrough(float **in, float **out, size_t size) {
	// for each audio thing
	for (int a = 0; a < 1; a++) {
		for (size_t i = 0; i < size; i++) {
			float ii = in[a][i];
			if (recording) {
				sdbuff[seek] = ii;
			}
			iterSeek(seek);
			out[a][i] = sdbuff[seek];
		}
	}
}

int main(void) {
	patch.Init();

	patch.SetAudioBlockSize(256);
	patch.StartAdc();
	patch.StartAudio(AudioThrough);

	sample_rate = patch.seed.AudioSampleRate();

	seek = 0;
	for (int i = 0; i < BUFF_SIZE; i++) {
		sdbuff[i] = 0.0;
	}
	recording = false;

	while(1) {
		// do nothing
		UpdateControls();
		UpdateOled();
	}
}
