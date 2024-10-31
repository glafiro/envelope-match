#include "PluginProcessor.h"
#include "PluginEditor.h"

EnvelopeMatchAudioProcessorEditor::EnvelopeMatchAudioProcessorEditor (EnvelopeMatchAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    attackSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    addAndMakeVisible(attackSlider);

    releaseSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    addAndMakeVisible(releaseSlider);

    amountSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    addAndMakeVisible(amountSlider);

    setSize (WIDTH, HEIGHT);
}

EnvelopeMatchAudioProcessorEditor::~EnvelopeMatchAudioProcessorEditor()
{
}

void EnvelopeMatchAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void EnvelopeMatchAudioProcessorEditor::resized()
{
    auto mainGroupBounds = getBounds()
        .removeFromBottom(getHeight() * 0.84f)
        .reduced(getWidth() * 0.01f);
    
    auto controlBarBounds = mainGroupBounds
        .removeFromBottom(mainGroupBounds.getHeight() * 0.2f);

    auto sliderWidth = controlBarBounds.getWidth() * 0.3f;
    auto sliderHeight = controlBarBounds.getHeight();

    attackSlider.setTextBoxStyle(Slider::TextBoxLeft, false, sliderWidth / 2, sliderHeight);
    attackSlider.setSize(sliderWidth, sliderHeight);
    
    releaseSlider.setTextBoxStyle(Slider::TextBoxLeft, false, sliderWidth / 2, sliderHeight);
    releaseSlider.setSize(sliderWidth, sliderHeight);
    
    amountSlider.setTextBoxStyle(Slider::TextBoxLeft, false, sliderWidth / 2, sliderHeight);
    amountSlider.setSize(sliderWidth, sliderHeight);

    Grid controlBarGrid;

    controlBarGrid.templateRows    = { Track(Fr(1)) };
    controlBarGrid.templateColumns = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };

    controlBarGrid.items = {
        GridItem(attackSlider),
        GridItem(releaseSlider),
        GridItem(amountSlider)
    };

    controlBarGrid.performLayout(controlBarBounds);
}
