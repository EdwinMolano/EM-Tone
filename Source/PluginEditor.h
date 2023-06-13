/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "HorizontalMeter.h"
#include "HorizontalGradientMeter.h"



struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider()  :juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};



//==============================================================================
/**
*/
class EMToneAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    EMToneAudioProcessorEditor (EMToneAudioProcessor&);
    ~EMToneAudioProcessorEditor() override;

    //==============================================================================

    void timerCallback() override;


    void paint (juce::Graphics&) override;
    void resized() override;



private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EMToneAudioProcessor& audioProcessor;
  
    //Visual sliders
    CustomRotarySlider lowSlider,
        midSlider,
        highSlider,

        GainSlider,
        ToneSlider,
        VolumeSlider,

        ChorusMixSlider,
        ChorusDepthSlider,
        ChorusRateSlider,

        ReverbMix,
        ReverbSize,
        ReverbWidth,
        ReverbDamp;


    //Attachment sliders
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment lowSliderAttachment,
        midSliderAttachment,
        highSliderAttachment,

        GainSliderAttachment,
        ToneSliderAttachment,
        VolumeSliderAttachment,
        
        ChorusMixAttachment,
        ChorusDepthAttachment,
        ChorusRateAtachment,

        ReverbMixAttachment,
        ReverbSizeAttachment,
        ReverbWidthAtachment,
        ReverbDampAtachment;

    //We store them in this vector
    std::vector<juce::Component*> getComps();


    //RMS readers
    Gui::HorizontalMeter InputHorizonalMeterL, InputHorizonalMeterR;
    Gui::HorizontalMeter OutputHorizonalMeterL, OutputHorizonalMeterR;

    Gui::VerticalGradientMeter InputGradientMeterL, InputGradientMeterR;
    Gui::VerticalGradientMeter OutputGradientMeterL, OutputGradientMeterR;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EMToneAudioProcessorEditor)
};
