#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EnvelopeMatchAudioProcessor::EnvelopeMatchAudioProcessor() : 
    AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
        .withInput("Sidechain", juce::AudioChannelSet::stereo(), true)
    ),
    apvts(*this, nullptr, "Parameters", createParameterLayout()),
    envFollowerL(),
    envFollowerR()
{
    apvts.state.addListener(this);

    for (auto& param : apvtsParameters) {
        param->castParameter(apvts);
    }

    envValues.reserve(MAX_ENV_VALUES);
    waveformValues.reserve(MAX_ENV_VALUES);
}

EnvelopeMatchAudioProcessor::~EnvelopeMatchAudioProcessor()
{
    apvts.state.removeListener(this);

}

//==============================================================================
const juce::String EnvelopeMatchAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EnvelopeMatchAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EnvelopeMatchAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EnvelopeMatchAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EnvelopeMatchAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EnvelopeMatchAudioProcessor::getNumPrograms()
{
    return 1;   
}

int EnvelopeMatchAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EnvelopeMatchAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EnvelopeMatchAudioProcessor::getProgramName (int index)
{
    return {};
}

void EnvelopeMatchAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void EnvelopeMatchAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    int nChannels = getTotalNumInputChannels();

    envelopeParameters.set("sampleRate", sampleRate);
    envelopeParameters.set("blockSize", samplesPerBlock);
    envelopeParameters.set("nChannels", nChannels);

    for (auto& param : apvtsParameters) {
        envelopeParameters.set(param->id.getParamID().toStdString(), param->getDefault());
    }

    envFollowerL.prepare(envelopeParameters);
    envFollowerR.prepare(envelopeParameters);
    amount.reset(sampleRate, 0.01f);
    envSignal.prepare(static_cast<int>(lengthToSamples(sampleRate, DISPLAY_LEN_IN_MS)));
    mainSignal.prepare(static_cast<int>(lengthToSamples(sampleRate, DISPLAY_LEN_IN_MS)));

}

void EnvelopeMatchAudioProcessor::updateDSP()
{
    for (auto& param : apvtsParameters) {
        envelopeParameters.set(param->id.getParamID().toStdString(), param->get());
    }

    envFollowerL.update(envelopeParameters);
    envFollowerR.update(envelopeParameters);
    amount.setTargetValue(apvtsParameters[ParameterNames::AMOUNT]->get());
}

void EnvelopeMatchAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EnvelopeMatchAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}
#endif

void EnvelopeMatchAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    bool expected = true;

    if (isNonRealtime() || parametersChanged.compare_exchange_strong(expected, false)) {
        updateDSP();
    }

    auto* bus = getBus(true, 1);
    if (bus != nullptr && bus->isEnabled()) {     

        auto mainBuffer = getBusBuffer(buffer, false, 0);
        auto sidechainBuffer = getBusBuffer(buffer, true, 1);

        auto sideChannels = sidechainBuffer.getNumChannels();
        auto nSamples = mainBuffer.getNumSamples();

        auto inputSamples = mainBuffer.getArrayOfWritePointers();
        auto sidechainSamples = sidechainBuffer.getArrayOfReadPointers();

        for (int s = 0; s < nSamples; ++s) {
            envValues.clear();
            waveformValues.clear();

            auto sampleL = inputSamples[0][s];
            auto sampleR = inputSamples[1][s];

            auto envL = envFollowerL.process(sidechainSamples[0][s]);
            auto envR = sideChannels > 1 ? envFollowerR.process(sidechainSamples[1][s]) : envL;

            envValues.push_back((envL + envR) * 0.5f);

            float t = amount.getNextValue() * 0.01f;

            auto outL = sampleL * (1.0 - t) + (sampleL * envL) * t;
            auto outR = sampleR * (1.0 - t) + (sampleR * envR) * t;

            inputSamples[0][s] = outL;
            inputSamples[1][s] = outR;
            waveformValues.push_back(fabs((outL + outR) * 0.5f));
        }
        envSignal.insert(envValues.data(), envValues.size());
        mainSignal.insert(waveformValues.data(), waveformValues.size());
    }
}

//==============================================================================
bool EnvelopeMatchAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EnvelopeMatchAudioProcessor::createEditor()
{
    //return new GenericAudioProcessorEditor(*this);
    return new EnvelopeMatchAudioProcessorEditor (*this);
}

//==============================================================================
void EnvelopeMatchAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void EnvelopeMatchAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EnvelopeMatchAudioProcessor();
}


AudioProcessorValueTreeState::ParameterLayout EnvelopeMatchAudioProcessor::createParameterLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;

    auto getMsString = [](float value, int) { return String(value) + " ms"; };
    auto getPctString = [](float value, int) { return String(value) + "%"; };

    layout.add(std::make_unique <AudioParameterFloat>(
        apvtsParameters[ParameterNames::ATTACK]->id,
        apvtsParameters[ParameterNames::ATTACK]->displayValue,
        NormalisableRange<float>{ 0.05f, 10.0f, 0.001f },
        apvtsParameters[ParameterNames::ATTACK]->getDefault(),
        AudioParameterFloatAttributes().withStringFromValueFunction(getMsString)
    ));

    layout.add(std::make_unique <AudioParameterFloat>(
        apvtsParameters[ParameterNames::RELEASE]->id,
        apvtsParameters[ParameterNames::RELEASE]->displayValue,
        NormalisableRange<float>{ 0.05f, 10.0f, 0.001f },
        apvtsParameters[ParameterNames::RELEASE]->getDefault(),
        AudioParameterFloatAttributes().withStringFromValueFunction(getMsString)
    ));

    layout.add(std::make_unique <AudioParameterFloat>(
        apvtsParameters[ParameterNames::AMOUNT]->id,
        apvtsParameters[ParameterNames::AMOUNT]->displayValue,
        NormalisableRange<float>{ 0.0f, 100.0f, 0.001f },
        apvtsParameters[ParameterNames::AMOUNT]->getDefault(),
        AudioParameterFloatAttributes().withStringFromValueFunction(getPctString)
    ));

    return layout;
}