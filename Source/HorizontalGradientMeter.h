/*
  ==============================================================================

    VerticalGradientMeter.h
    Created: 6 Jun 2023 2:29:08pm
    Author:  Edwin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Gui
{
    class VerticalGradientMeter : public juce::Component, public juce::Timer
    {
    public:

        VerticalGradientMeter(std::function<float()>&& valueFunction) : valueSupplier(std::move(valueFunction))
        {
            startTimer(24);
        }

        void paint(juce::Graphics& g) override
        {
            const auto level = valueSupplier();

            auto bounds = getLocalBounds().toFloat();

            //background
            g.setColour(juce::Colours::black);
            g.fillRect(bounds);


            g.setGradientFill(gradient);
            const auto scaledX = juce::jmap(level, -60.f, 6.f, 0.f, static_cast<float>(getWidth()));

            g.fillRect(bounds.removeFromLeft(scaledX));

        }

        void resized() override
        {
            const auto bounds = getLocalBounds().toFloat();

            //gradient
            gradient = juce::ColourGradient{
                 juce::Colours::green,
                     bounds.getTopLeft(),
                     juce::Colours::red,
                     bounds.getTopRight(),
                     false
            };
            //to add color in almost end
            gradient.addColour(0.65, juce::Colours::yellow);
        }

        void timerCallback() override
        {
            repaint();
        }



    private:
        std::function<float()> valueSupplier;

        juce::ColourGradient gradient{};

    };
}
