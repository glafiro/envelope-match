#pragma once

#include <JuceHeader.h>

namespace Colors {
	const Colour bgColor{ 249, 215, 185 };
	const Colour displayColor{ 0, 114, 182 };
	const Colour ctrlBarColor{ 2, 98, 156 };
}

class TextSliderLookAndFeel : public LookAndFeel_V4
{
    Font font;

public:

    TextSliderLookAndFeel() {
        font = Font(Typeface::createSystemTypefaceFor(BinaryData::hack_ttf, BinaryData::hack_ttfSize));

    }

    static TextSliderLookAndFeel* get() {
        static TextSliderLookAndFeel inst;
        return &inst;
    }


    void drawLinearSlider(Graphics& g,
        int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        Slider::SliderStyle style, Slider& slider) override {

        auto bounds = Rectangle<int>{ x, y, width, height };
        auto textBounds = bounds.withX(x + width * 0.1f);

        g.setColour(Colors::bgColor);
        g.drawRect(bounds, 2.0f);

        g.setFont(font.withHeight(bounds.getHeight() * 0.7f));
        g.drawText(slider.getTextFromValue(slider.getValue()), textBounds, Justification::centredLeft, false);

    }

    void drawLabel(Graphics& g, Label& l) override {

        auto bounds = l.getLocalBounds().reduced(1.0f);
        
        g.setColour(Colors::bgColor);
        g.fillRect(bounds);
        g.setColour(Colors::ctrlBarColor);
        g.setFont(font.withHeight(bounds.getHeight() * 0.7f));
        g.drawText(l.getText(), bounds, Justification::centred, false);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextSliderLookAndFeel)
};
