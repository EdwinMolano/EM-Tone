/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EMToneAudioProcessorEditor::EMToneAudioProcessorEditor (EMToneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(gainSlider = new juce::Slider("gain"));
    gainSlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
   
    


    setSize(600, 400);
}

EMToneAudioProcessorEditor::~EMToneAudioProcessorEditor()
{
}

//==============================================================================
void EMToneAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawText("Gain", ((getWidth() / 5) * 1) - (100 / 2), (getHeight() / 2) + 5, 100, 100, juce::Justification::centred, false);
   // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void EMToneAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    gainSlider->setBounds(((getWidth() / 5) * 1) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);


}
