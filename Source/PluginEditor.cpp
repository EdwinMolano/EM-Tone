/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EMToneAudioProcessorEditor::EMToneAudioProcessorEditor (EMToneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

    InputGradientMeterL([&]() {return audioProcessor.getInputRmsValue(0); }),
    InputGradientMeterR([&]() {return audioProcessor.getInputRmsValue(1); }),
    OutputGradientMeterL([&]() {return audioProcessor.getOutputRmsValue(0); }),
    OutputGradientMeterR([&]() {return audioProcessor.getOutputRmsValue(1); }),

    GainSliderAttachment(audioProcessor.apvts, "Gain", GainSlider),
    lowSliderAttachment(audioProcessor.apvts, "Low", lowSlider),
    midSliderAttachment(audioProcessor.apvts, "Mid", midSlider),
    highSliderAttachment(audioProcessor.apvts, "High", highSlider),
    ToneSliderAttachment(audioProcessor.apvts, "Tone", ToneSlider),
    VolumeSliderAttachment(audioProcessor.apvts, "Volume", VolumeSlider),

    ChorusMixAttachment(audioProcessor.apvts, "ChorusMix", ChorusMixSlider),
    ChorusDepthAttachment(audioProcessor.apvts, "ChorusDepth", ChorusDepthSlider),
    ChorusRateAtachment(audioProcessor.apvts, "ChorusRate", ChorusRateSlider),

    ReverbMixAttachment(audioProcessor.apvts, "ReverbMix", ReverbMix),
    ReverbSizeAttachment(audioProcessor.apvts, "ReverbSize", ReverbSize),
    ReverbWidthAtachment(audioProcessor.apvts, "ReverbWidth", ReverbWidth),
    ReverbDampAtachment(audioProcessor.apvts, "ReverbDamp", ReverbDamp)

     
{
    ///////Version 1
    /*addAndMakeVisible(InputHorizonalMeterL);
    addAndMakeVisible(InputHorizonalMeterR);

    addAndMakeVisible(OutputHorizonalMeterL);
    addAndMakeVisible(OutputHorizonalMeterR);*/

    //////Version 2
    addAndMakeVisible(InputGradientMeterL);
    addAndMakeVisible(InputGradientMeterR);

    addAndMakeVisible(OutputGradientMeterL);
    addAndMakeVisible(OutputGradientMeterR);


    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }


    setSize(700, 500);

    startTimerHz(24);
    //ffps
    //startTimer(1000.f/24);
}

EMToneAudioProcessorEditor::~EMToneAudioProcessorEditor()
{
}

//==============================================================================
void EMToneAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
   // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
   // juce::File temp("C:\\Users\\Edwin\\Desktop\\Memes_Factory\\EM_Tone.png");

    juce::File temp("C:\\Users\\Edwin\\Desktop\\HOWEST\\2º Year\\2ºSemester\\Audio Prog\\Assignement\\EM-Tone\\Builds\\VisualStudio2022\\Resources\\Em-Tone_Background.png");
    juce::Image backround = juce::ImageCache::getFromFile(temp);
    g.drawImage(backround, getLocalBounds().toFloat());

    
    juce::File temp2("C:\\Users\\Edwin\\Desktop\\HOWEST\\2º Year\\2ºSemester\\Audio Prog\\Assignement\\EM-Tone\\Builds\\VisualStudio2022\\Resources\\Em-Tone_Grid_RMSreaders.png");
    juce::Image gridRMSreaders = juce::ImageCache::getFromFile(temp2);
    g.drawImage(gridRMSreaders, getLocalBounds().toFloat());
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);

  

    
    g.drawText("Input Level", 85,76,100, juce::Justification::centred, false);
    g.drawText("Output Level", 542,76,100, juce::Justification::centred, false);

    g.drawText("Gain", ((getWidth() / 6.7) * 1) - (130 / 2), (getHeight() * 0.3) + 1, 100, 100, juce::Justification::centred, false);
    g.drawText("Low", ((getWidth() / 6.7) * 2) - (130 / 2), (getHeight() * 0.3) + 1, 100, 100, juce::Justification::centred, false);
    g.drawText("Mid", ((getWidth() / 6.7) * 3) - (130 / 2), (getHeight() * 0.3) + 1, 100, 100, juce::Justification::centred, false);
    g.drawText("High", ((getWidth() / 6.7) * 4) - (130 / 2), (getHeight() * 0.3) + 1, 100, 100, juce::Justification::centred, false);
    g.drawText("Tone", ((getWidth() / 6.7) * 5) - (130 / 2), (getHeight() * 0.3) + 1, 100, 100, juce::Justification::centred, false);
    g.drawText("Volume", ((getWidth() / 6.7) * 6) - (130 / 2), (getHeight() * 0.3) + 1, 100, 100, juce::Justification::centred, false);

    g.drawText("Chorus Mix", ((getWidth() / 6.7) * 2.2) - (130 / 2), (getHeight() * 0.55) + 1, 100, 100, juce::Justification::centred, false);
    g.drawText("Chorus Depth", ((getWidth() / 6.7) * 3.5) - (130 / 2), (getHeight() * 0.55) + 1, 100, 100, juce::Justification::centred, false);
    g.drawText("Chorus Rate", ((getWidth() / 6.7) * 4.7) - (130 / 2), (getHeight() * 0.55) + 1, 100, 100, juce::Justification::centred, false);

    g.drawText("Reverb Mix", ((getWidth() / 6.7) * 1.2) - (130 / 2), (getHeight() * 0.8) +1, 100, 100, juce::Justification::centred, false);
    g.drawText("Reverb Size", ((getWidth() / 6.7) * 2.7) - (130 / 2), (getHeight() * 0.8) + 1, 100, 100, juce::Justification::centred, false);
    g.drawText("Reverb Width", ((getWidth() / 6.7) * 4.2) - (130 / 2), (getHeight() * 0.8) + 1, 100, 100, juce::Justification::centred, false);
    g.drawText("Reverb Tone", ((getWidth() / 6.7) * 5.7) - (130 / 2), (getHeight() * 0.8) + 1, 100, 100, juce::Justification::centred, false);

   /* g.setFont(55.0f);
    g.drawText("EM-Tone", 250, 30, 200, juce::Justification::centred,false);*/

}

void EMToneAudioProcessorEditor::resized()
{

   

    ///---------------approach to eacho ther knbos--------------height staring from top------------------
    //1st row
    GainSlider.setBounds(((getWidth() / 6.7) * 1) - (130 / 2), (getHeight() * 0.3) - (100 / 2), 100, 100);
    lowSlider.setBounds(((getWidth() / 6.7) * 2) - (130 / 2), (getHeight() * 0.3) - (100 / 2), 100, 100);
    midSlider.setBounds(((getWidth() / 6.7) * 3) - (130 / 2), (getHeight() * 0.3) - (100 / 2), 100, 100);
    highSlider.setBounds(((getWidth() / 6.7) * 4) - (130 / 2), (getHeight() * 0.3) - (100 / 2), 100, 100);
    ToneSlider.setBounds(((getWidth() / 6.7) * 5) - (130 / 2), (getHeight() * 0.3) - (100 / 2), 100, 100);
    VolumeSlider.setBounds(((getWidth() / 6.7) * 6) - (130 / 2), (getHeight() * 0.3) - (100 / 2), 100, 100);

    ChorusMixSlider.setBounds(((getWidth() / 6.7) * 2.2) - (130 / 2), (getHeight() * 0.55) - (100 / 2), 100, 100);
    ChorusDepthSlider.setBounds(((getWidth() / 6.7) * 3.5) - (130 / 2), (getHeight() * 0.55) - (100 / 2), 100, 100);
    ChorusRateSlider.setBounds(((getWidth() / 6.7) * 4.7) - (130 / 2), (getHeight() * 0.55) - (100 / 2), 100, 100);

    ReverbMix.setBounds(((getWidth() / 6.7) * 1.2) - (130 / 2), (getHeight() * 0.8) - (100 / 2), 100, 100);
    ReverbSize.setBounds(((getWidth() / 6.7) * 2.7) - (130 / 2), (getHeight() * 0.8) - (100 / 2), 100, 100);
    ReverbWidth.setBounds(((getWidth() / 6.7) * 4.2) - (130 / 2), (getHeight() * 0.8) - (100 / 2), 100, 100);
    ReverbDamp.setBounds(((getWidth() / 6.7) * 5.7) - (130 / 2), (getHeight() * 0.8) - (100 / 2), 100, 100);

    ///---------------RMS readers------------------
    InputHorizonalMeterL.setBounds(30, 50, 180, 15);
    InputHorizonalMeterR.setBounds(30, 70, 180, 15);

    OutputHorizonalMeterL.setBounds(493, 50, 173, 15);
    OutputHorizonalMeterR.setBounds(493, 70, 173, 15);


    InputGradientMeterL.setBounds(30, 50, 180, 15);
    InputGradientMeterR.setBounds(30, 70, 180, 15);

    OutputGradientMeterL.setBounds(493, 50, 173, 15);
    OutputGradientMeterR.setBounds(493, 70, 173, 15);

    

}

std::vector<juce::Component*> EMToneAudioProcessorEditor::getComps()
{
    return 
    {
        &GainSlider,
        &lowSlider,
        &midSlider,
        &highSlider,
        &ToneSlider,
        &VolumeSlider,

        &ChorusMixSlider,
        &ChorusDepthSlider,
        &ChorusRateSlider,

        &ReverbMix,
        &ReverbSize,
        &ReverbWidth,
        &ReverbDamp
    };

}


void EMToneAudioProcessorEditor::timerCallback()
{

    InputHorizonalMeterL.setLevel(audioProcessor.getInputRmsValue(0));
    InputHorizonalMeterR.setLevel(audioProcessor.getInputRmsValue(1));
    OutputHorizonalMeterL.setLevel(audioProcessor.getOutputRmsValue(0));
    OutputHorizonalMeterR.setLevel(audioProcessor.getOutputRmsValue(1));

    InputHorizonalMeterL.repaint();
    InputHorizonalMeterR.repaint();
    OutputHorizonalMeterL.repaint();
    OutputHorizonalMeterR.repaint();

}
