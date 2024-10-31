#include "PluginProcessor.h"
#include "PluginEditor.h"

EnvelopeMatchAudioProcessorEditor::EnvelopeMatchAudioProcessorEditor (EnvelopeMatchAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(amountSlider);

    setSize (WIDTH, HEIGHT);
}

EnvelopeMatchAudioProcessorEditor::~EnvelopeMatchAudioProcessorEditor()
{
}

void EnvelopeMatchAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(Colors::bgColor);

    int cornerSize = getBounds().getWidth() * 0.01f;

    g.setColour(Colors::displayColor);
    g.fillRoundedRectangle(displayBounds.toFloat(), cornerSize);
    
    g.setColour(Colors::ctrlBarColor);
    g.fillRoundedRectangle(controlBarBounds.toFloat(), cornerSize);
}

void EnvelopeMatchAudioProcessorEditor::resized()
{
    displayBounds = getBounds()
        .removeFromBottom(getHeight() * 0.84f)
        .reduced(getWidth() * 0.01f);
    
    controlBarBounds = displayBounds
        .removeFromBottom(displayBounds.getHeight() * 0.2f);


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
