#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "GuiComponents.h"

using Track = Grid::TrackInfo;
using Fr = Grid::Fr;

#define WIDTH 600
#define HEIGHT 337

class EnvelopeMatchAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    EnvelopeMatchAudioProcessorEditor (EnvelopeMatchAudioProcessor&);
    ~EnvelopeMatchAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    EnvelopeMatchAudioProcessor& audioProcessor;

    TextSlider attackSlider{"atk"};
    AudioProcessorValueTreeState::SliderAttachment attackAttachment{
        audioProcessor.apvts, apvtsParameters[ParameterNames::ATTACK]->id.getParamID(), attackSlider.slider
    };
    
    TextSlider releaseSlider{"rel"};
    AudioProcessorValueTreeState::SliderAttachment releaseAttachment{
        audioProcessor.apvts, apvtsParameters[ParameterNames::RELEASE]->id.getParamID(), releaseSlider.slider
    };
    
    TextSlider amountSlider{"amt"};
    AudioProcessorValueTreeState::SliderAttachment amountAttachment{
        audioProcessor.apvts, apvtsParameters[ParameterNames::AMOUNT]->id.getParamID(), amountSlider.slider
    };


    Rectangle<int> displayBounds;
    Rectangle<int> controlBarBounds;
   

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeMatchAudioProcessorEditor)
};
