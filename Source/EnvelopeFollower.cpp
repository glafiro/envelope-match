#include "EnvelopeFollower.h"

void EnvelopeFollower::prepare(DSPParameters<float>& params) {
	sampleRate = params["sampleRate"];
	blockSize = params["blockSize"];
	nChannels = params["nChannels"];

	update(params);
}

void EnvelopeFollower::update(DSPParameters<float>& params) {
	attack = params["attack"];
	release = params["release"];
	depth = params["depth"] * 0.01f;
	isOn = static_cast<bool>(params["isOn"]);
}

void EnvelopeFollower::processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
	for (int ch = 0; ch < numChannels; ++ch) {
		for (int s = 0; s < numSamples; ++s) {
			auto sample = inputBuffer[ch][s];
			inputBuffer[ch][s] = sample;
		}
	}
}
