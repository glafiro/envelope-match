#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using Track = Grid::TrackInfo;
using Fr = Grid::Fr;

#define WIDTH 400
#define HEIGHT 225

class EnvelopeMatchAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    EnvelopeMatchAudioProcessorEditor (EnvelopeMatchAudioProcessor&);
    ~EnvelopeMatchAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    EnvelopeMatchAudioProcessor& audioProcessor;

    Slider attackSlider;
    AudioProcessorValueTreeState::SliderAttachment attackAttachment{
        audioProcessor.apvts, apvtsParameters[ParameterNames::ATTACK]->id.getParamID(), attackSlider
    };
    
    Slider releaseSlider;
    AudioProcessorValueTreeState::SliderAttachment releaseAttachment{
        audioProcessor.apvts, apvtsParameters[ParameterNames::RELEASE]->id.getParamID(), releaseSlider
    };
    
    Slider amountSlider;
    AudioProcessorValueTreeState::SliderAttachment amountAttachment{
        audioProcessor.apvts, apvtsParameters[ParameterNames::AMOUNT]->id.getParamID(), amountSlider
    };
   

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeMatchAudioProcessorEditor)
};
