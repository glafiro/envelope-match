#include "EnvelopeFollower.h"


void EnvelopeFollower::prepare(DSPParameters<float>& params) {
	sampleRate = params["sampleRate"];
	blockSize = params["blockSize"];
	nChannels = params["nChannels"];

	update(params);
}

void EnvelopeFollower::update(DSPParameters<float>& params) {
	attack = computeCoefficient(params["attack"]);
	release = computeCoefficient(params["release"]);
	amount = params["amount"] * 0.01f;
}

float EnvelopeFollower::computeCoefficient(float t) {
	t *= 0.001f;
	return std::exp(-1.0f / (t * sampleRate));
}

float EnvelopeFollower::process(float in) {
	float rectified = std::fabs(in) * sensitivity;
	if (rectified > env) {
		env = attack * (env - rectified) + rectified;
	}
	else if (rectified < env) {
		env = release * (env - rectified) + rectified;
	}
	return env;
}