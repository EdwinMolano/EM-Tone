/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


struct ChainSettings
{
    float GainLow{ 0 }, GainMid{ 0 }, GainHigh{ 0 },
        Gain{ 0 }, Tone{ 0 }, Volume{ 0 },
        ChorusRate{ 0 }, ChorusDepth{ 0 }, ChorusMix{ 0 }, 
        ReverbSize{ 0 }, ReverbDamp{ 0 }, ReverbWidth{ 0 }, ReverbMix{ 0 }   /*, freeze{ 0 }*/;


   
};

//helper function that will give us this previous data
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);





//==============================================================================
/**
*/
class EMToneAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    EMToneAudioProcessor();
    ~EMToneAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    //----------·3 EQ Knobs--------------
    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};

    //--------------Distortion knobs------------------
    //void update3Knobs();

    //--------------Chorus Functions------------------
   // void updateChorusParams();

    //--------------Reverb Functions------------------
    //void updateReverbParams();

    //--------------RMS reader Functions------------------
    float getInputRmsValue(const int channel) const;
    float getOutputRmsValue(const int channel) const;


private:
    //prepare to use the stereo stuff
    using Filter = juce::dsp::IIR::Filter<float>;

    using MonoChain = juce::dsp::ProcessorChain<Filter, Filter, Filter>;

    MonoChain leftChain, rightChain;


    //----------------------------Pre Chain Process------------------------
    juce::dsp::ProcessorChain<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Filter<float>> pre_eq;

    juce::dsp::Compressor<float> pre_compressor;
    
    //----------------3 Knobs EQ------------------------

    /*juce::AudioParameterFloat* lowKnob  { nullptr };
    juce::AudioParameterFloat* midKnob  { nullptr };
    juce::AudioParameterFloat* highKnob  { nullptr };*/

    enum ChainPos
    {
        lowK,
        midK,
        highK
    };

    ////-----------------Distortion Parameters------------------------
   // juce::AudioParameterFloat* gainParam { nullptr };
    //juce::AudioParameterFloat* toneParam { nullptr };
    //juce::AudioParameterFloat* volumeParam { nullptr };

    juce::dsp::Convolution convolver_clean;
    juce::dsp::Convolution convolver_fuzz;

    //---------------Chorus Parameters-----------------
    juce::dsp::Chorus<float> chorus;

    //juce::AudioParameterFloat* chorusRate  { nullptr };
  //  juce::AudioParameterFloat* chorusDepth  { nullptr };
   // juce::AudioParameterFloat* chorusMix  { nullptr };
    //------------Reverb Parameters--------------------
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters params;

    //juce::AudioParameterFloat* size   { nullptr };
    //juce::AudioParameterFloat* damp   { nullptr };
    //juce::AudioParameterFloat* width  { nullptr };
    //juce::AudioParameterFloat* reverbMix    { nullptr };
    juce::AudioParameterBool* freeze  { nullptr };

    //------------RMS reader Parameters--------------------
    juce::LinearSmoothedValue<float> InputRMSLevelLeft, InputRMSLevelRight;
    juce::LinearSmoothedValue<float> OutputRMSLevelLeft, OutputRMSLevelRight;
 
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EMToneAudioProcessor)
};
