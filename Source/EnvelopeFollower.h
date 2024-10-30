#pragma once

#define SENSITIVITY 10.0f

#include "Utils.h"
#include "DSPParameters.h"
#include "FilteredParameter.h"

class EnvelopeFollower
{
    float sampleRate{ 44100.0f };
    float nChannels{ 2.0f };
    int blockSize{ 0 };

    float attack{};
    float release{};
    float depth{};

    bool isOn{ true };

public:
    void prepare(DSPParameters<float>& params);
    void update(DSPParameters<float>& params);
    void processBlock(float* const* inputBuffer, int numChannels, int numSamples);
};