/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class EMToneAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    EMToneAudioProcessorEditor (EMToneAudioProcessor&);
    ~EMToneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EMToneAudioProcessor& audioProcessor;

    juce::ScopedPointer<juce::Slider>  gainSlider;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;






    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EMToneAudioProcessorEditor)
};
