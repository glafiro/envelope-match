#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EnvelopeMatchAudioProcessor::EnvelopeMatchAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    apvts(*this, nullptr, "Parameters", createParameterLayout()),
    envFollower()
#endif
{
    apvts.state.addListener(this);

    for (auto& param : apvtsParameters) {
        param->castParameter(apvts);
    }
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

//==============================================================================
void EnvelopeMatchAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    int nChannels = getTotalNumInputChannels();

    envelopeParameters.set("sampleRate", sampleRate);
    envelopeParameters.set("blockSize", samplesPerBlock);
    envelopeParameters.set("nChannels", nChannels);

    for (auto& param : apvtsParameters) {
        envelopeParameters.set(param->id.getParamID().toStdString(), param->getDefault());
    }

    envFollower.prepare(envelopeParameters);
}

void EnvelopeMatchAudioProcessor::updateDSP()
{
    for (auto& param : apvtsParameters) {
        envelopeParameters.set(param->id.getParamID().toStdString(), param->get());
    }

    envFollower.update(envelopeParameters);
}

void EnvelopeMatchAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EnvelopeMatchAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
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

    envFollower.processBlock(
        buffer.getArrayOfWritePointers(),
        buffer.getNumChannels(),
        buffer.getNumSamples()
    );
}

//==============================================================================
bool EnvelopeMatchAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EnvelopeMatchAudioProcessor::createEditor()
{
    return new GenericAudioProcessorEditor(*this);
    //return new EnvelopeMatchAudioProcessorEditor (*this);
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

    layout.add(std::make_unique <AudioParameterFloat>(
        apvtsParameters[ParameterNames::ATTACK]->id,
        apvtsParameters[ParameterNames::ATTACK]->displayValue,
        NormalisableRange<float>{ 0.05f, 10.0f, 0.01f },
        apvtsParameters[ParameterNames::ATTACK]->getDefault()
    ));

    layout.add(std::make_unique <AudioParameterFloat>(
        apvtsParameters[ParameterNames::RELEASE]->id,
        apvtsParameters[ParameterNames::RELEASE]->displayValue,
        NormalisableRange<float>{ 0.05f, 10.0f, 0.01f },
        apvtsParameters[ParameterNames::RELEASE]->getDefault()
    ));

    layout.add(std::make_unique <AudioParameterFloat>(
        apvtsParameters[ParameterNames::DEPTH]->id,
        apvtsParameters[ParameterNames::DEPTH]->displayValue,
        NormalisableRange<float>{ 0.0f, 100.0f, 0.01f },
        apvtsParameters[ParameterNames::DEPTH]->getDefault()
    ));

    layout.add(std::make_unique <AudioParameterBool>(
        apvtsParameters[ParameterNames::IS_ON]->id,
        apvtsParameters[ParameterNames::IS_ON]->displayValue,
        apvtsParameters[ParameterNames::IS_ON]->getDefault()
    ));

    return layout;
}