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
