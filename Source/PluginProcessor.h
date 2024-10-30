#pragma once

#include <JuceHeader.h>

#include "DSPParameters.h"
#include "APVTSParameter.h"
#include "EnvelopeFollower.h"

enum ParameterNames {
    ATTACK, RELEASE,
    DEPTH, IS_ON,
    PARAM_COUNT
};

static std::array<std::unique_ptr<IAPVTSParameter>, ParameterNames::PARAM_COUNT> apvtsParameters{
    std::make_unique<APVTSParameterFloat>("attack",  "Attack",  0.05f),
    std::make_unique<APVTSParameterFloat>("release", "Release", 0.05f),
    std::make_unique<APVTSParameterFloat>("depth",   "Depth",   1.0f),
    std::make_unique<APVTSParameterBool> ("isOn",    "On",      true)
};

class EnvelopeMatchAudioProcessor  : 
    public AudioProcessor,
    public ValueTree::Listener
{
public:
    //==============================================================================
    EnvelopeMatchAudioProcessor();
    ~EnvelopeMatchAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AudioProcessorValueTreeState     apvts;


private:
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Manage parameter changes in a thread-safe way
    std::atomic<bool> parametersChanged{ false };
    void valueTreePropertyChanged(ValueTree&, const Identifier&) override {
        parametersChanged.store(true);
    }

    DSPParameters<float> envelopeParameters;
    void updateDSP();

    EnvelopeFollower envFollower;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeMatchAudioProcessor)
};
