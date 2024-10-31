#include "PluginProcessor.h"
#include "PluginEditor.h"

EnvelopeMatchAudioProcessorEditor::EnvelopeMatchAudioProcessorEditor (EnvelopeMatchAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(amountSlider);
    addAndMakeVisible(signalDisplay);

    setSize (WIDTH, HEIGHT);
}

EnvelopeMatchAudioProcessorEditor::~EnvelopeMatchAudioProcessorEditor()
{
}

void EnvelopeMatchAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(Colors::bgColor);

    // Draw logo
    float logoHeight = getHeight() * 0.16f;
    float logoWidth = getWidth() * 0.8f;
    float xOffset = getWidth() * 0.02f;
    float yOffset = getHeight() * 0.02f;
    auto bounds = Rectangle<float>{ xOffset, yOffset, logoWidth, logoHeight };
    logo->drawWithin(g, bounds, RectanglePlacement::xLeft, 1.0f);


    // Draw display

    int cornerSize = getBounds().getWidth() * 0.01f;

    g.setColour(Colors::displayColor);
    Path topRect;
    topRect.addRoundedRectangle(
        displayBounds.getX(), displayBounds.getY(),
        displayBounds.getWidth(), displayBounds.getHeight(),
        cornerSize, cornerSize, true, true, false, false
    );
    g.fillPath(topRect);
    
    Path bottomRect;
    bottomRect.addRoundedRectangle(
        controlBarBounds.getX(), controlBarBounds.getY(),
        controlBarBounds.getWidth(), controlBarBounds.getHeight(),
        cornerSize, cornerSize, false, false, true, true
    );
    g.setColour(Colors::ctrlBarColor);
    g.fillPath(bottomRect);
}

void EnvelopeMatchAudioProcessorEditor::resized()
{
    displayBounds = getBounds()
        .removeFromBottom(getHeight() * 0.84f)
        .reduced(getWidth() * 0.01f);

    signalDisplay.setBounds(displayBounds.withHeight(displayBounds.getHeight() * 0.8f));

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
