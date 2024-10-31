#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"

class TextSlider : public Component
{
public:
    Slider slider;

    TextSlider(const String& parameterName)
        : slider(Slider::SliderStyle::LinearBarVertical, Slider::TextEntryBoxPosition::NoTextBox)
    {
        nameLabel.setText(parameterName, NotificationType::dontSendNotification);
        nameLabel.attachToComponent(&slider, true);
        addAndMakeVisible(nameLabel);

        addAndMakeVisible(slider); 

        setLookAndFeel(TextSliderLookAndFeel::get());

    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(getLocalBounds().getHeight() * 0.2f);

        auto sliderBounds = bounds
            .withX(bounds.getX() + bounds.getWidth() * 0.3f)
            .withWidth(bounds.getWidth() * 0.7f);

        auto labelBounds = bounds
            .withWidth(bounds.getWidth() * 0.3f);

        slider.setBounds(sliderBounds);
        nameLabel.setBounds(labelBounds);
    }

    void paint(Graphics& g) override {
        auto bounds = getLocalBounds().reduced(getLocalBounds().getHeight() * 0.2f).reduced(1);
        g.setColour(Colors::bgColor);
        g.fillRect(bounds.withWidth(bounds.getWidth() * 0.31f));
    }

private:
    Label valueLabel;
    Label nameLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextSlider)
};

class SignalDisplay : public Component, private Timer
{
    int refreshRate{ 60 };
    size_t envLength;
    float* envData;
    size_t mainLength;
    float* mainData;

public:

    SignalDisplay(size_t n, float* env, size_t nMain,float* main) : envLength(n), envData(env), mainLength(nMain), mainData(main) {
        startTimerHz(refreshRate);
    }
    
    void timerCallback() override {
        repaint();
    }

    void paint(Graphics& g) override {
        auto bounds = getLocalBounds();
        int width = bounds.getWidth();
        int height = bounds.getHeight();
        float scale = static_cast<float>(envLength) / width;

        Path mainPath;
        mainPath.startNewSubPath(0, (1.0f - mainData[0]) * height); 

        for (int x = 1; x < width; ++x) { 
            int ix = static_cast<int>(x * scale); 
            if (ix < mainLength) {
                float y = (1.0f - mainData[ix] * 5.0f) * height;
                mainPath.lineTo(static_cast<float>(x), y);
            }
        }

        mainPath.lineTo(width, height); 
        mainPath.lineTo(0, height);     
        mainPath.closeSubPath();        
        scale = static_cast<float>(mainLength) / width;

        g.setColour(Colors::ctrlBarColor);
        g.fillPath(mainPath);

        Path signalPath;
        signalPath.startNewSubPath(0, (1.0f - envData[0]) * height); 

        for (int x = 1; x < width; ++x) {
            int idx = static_cast<int>(x * scale);
            if (idx < envLength) {
                float y = (1.0f - envData[idx]) * height;
                signalPath.lineTo(static_cast<float>(x), y);
            }
        }

        g.setColour(Colors::bgColor);
        g.strokePath(signalPath, PathStrokeType(2.0f));
    }

};