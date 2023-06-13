/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout EMToneAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout; //created layout variable

    /*layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Freq",
        "Peak Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
        2000.f));*/
    layout.add(std::make_unique<juce::AudioParameterFloat>("Low",
        "Low",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
        0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Mid",
        "Mid",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
        0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("High",
        "High",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
        0.f));

    /*layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Quality",
        "Peak Quality",
        juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
        1.f));*/

    return layout;

}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;

    settings.GainLow = apvts.getRawParameterValue("Low")->load();
    settings.GainMid = apvts.getRawParameterValue("Mid")->load();
    settings.GainHigh = apvts.getRawParameterValue("High")->load();

    return settings;
}

//==============================================================================
EMToneAudioProcessor::EMToneAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    //3 EQ Knobs Parameters

    /*lowKnob = new juce::AudioParameterFloat("Low", "Low",-24.f, 24.f, 0.f);
    midKnob = new juce::AudioParameterFloat("Mid", "Mid", -24.f, 24.f, 0.f);
    highKnob = new juce::AudioParameterFloat("High", "High", -24.f, 24.f, 0.f);

    addParameter(lowKnob);
    addParameter(midKnob);
    addParameter(highKnob);*/
    
    //Distortion Parameters

    addParameter(gainParam = new juce::AudioParameterFloat("gain", "Gain", juce::NormalisableRange<float>(0.01f, 30.f, 0.001), 5.0));
    addParameter(toneParam = new juce::AudioParameterFloat("tone", "Tone", juce::NormalisableRange<float>(10.f, 20.f, 0.001), 15.0));
    addParameter(volumeParam = new juce::AudioParameterFloat("volume", "Volume", juce::NormalisableRange<float>(0.f, 1.7f, 0.001), 1.0));

    convolver_clean.reset();

    convolver_fuzz.reset();

    //Chorus Parameters
    chorusRate = new juce::AudioParameterFloat("ChorusRate", "Chorus Rate", 0.1f, 1.0f, 0.5f);
    chorusDepth = new juce::AudioParameterFloat("ChorusDepth", "Chorus Depth", 0.0f, 1.0f, 0.5f);
    chorusMix = new juce::AudioParameterFloat("ChorusMix", "Chorus Mix", 0.0f, 1.0f, 0.0f);

    addParameter(chorusRate);
    addParameter(chorusDepth);
    addParameter(chorusMix);

    updateChorusParams();

    //Reverb Parameters
    size = new juce::AudioParameterFloat("Size", "Reverb Size", 0.0f, 1.0f, 0.5f);
    damp = new juce::AudioParameterFloat("Damp", "Reverb Damp", 0.0f, 1.0f, 0.5f);//Bright reverb or darker
    width = new juce::AudioParameterFloat("Width", "Reverb Width", 0.0f, 1.0f, 0.5f);
    reverbMix = new juce::AudioParameterFloat("ReverbMix", "Reverb Mix", 0.0f, 1.0f, 0.0f);
    freeze = new juce::AudioParameterBool("Freeze", "Reverb Freeze", false);

    addParameter(size);
    addParameter(damp);
    addParameter(width);
    addParameter(reverbMix);
    addParameter(freeze);

    updateReverbParams();
}

EMToneAudioProcessor::~EMToneAudioProcessor()
{
}

//==============================================================================
const juce::String EMToneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EMToneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EMToneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EMToneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EMToneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EMToneAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EMToneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EMToneAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EMToneAudioProcessor::getProgramName (int index)
{
    return {};
}

void EMToneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EMToneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //--------------Preparing filters before using them
    juce::dsp::ProcessSpec spec;

    //needs to know

    spec.maximumBlockSize = samplesPerBlock;

    spec.numChannels = 1;//mono chains can only handle 1 chanel

    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);


   //----------------------------Pre Chain Process---------------------------------------------
    // EQ AND COMPRESSOR

    // Initialize compressor
    juce::dsp::ProcessSpec Spec_Comp;
    Spec_Comp.sampleRate = sampleRate;
    Spec_Comp.maximumBlockSize = static_cast<size_t>(samplesPerBlock);
    Spec_Comp.numChannels = getTotalNumInputChannels();

    // Initialize compressor parameters
    const float threshold = -20.0f; // Threshold (dB)
    const float ratio = 4.0f; // Ratio
    const float attack = 2.5f; // Attack time (ms)
    const float release = 25.0f; // Release time (ms)

    // Set compressor parameters
    pre_compressor.setThreshold(juce::Decibels::decibelsToGain(threshold));
    pre_compressor.setRatio(ratio);
    pre_compressor.setAttack(attack / 1000.0f);
    pre_compressor.setRelease(release / 1000.0f);


    // Initialize EQ bands
    //const float sampleRate = getSampleRate();
    const float frequencyLow = 100.0f; // Low shelf frequency (Hz)
    const float frequencyMid = 2000.0f; // Mid frequency (Hz)
    const float frequencyHigh = 5000.0f; // High shelf frequency (Hz)
    const float gainLow = -35.0f; // Low shelf gain (dB)
    const float gainMid = 1.0f; // Mid gain (dB)
    const float gainHigh = -35.0f; // High shelf gain (dB)
    const float Q = 2.0f; // Filter Q value

    // Initialize EQ bands
    pre_eq.get<0>() = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, frequencyLow, Q, juce::Decibels::decibelsToGain(gainLow));
    pre_eq.get<1>() = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, frequencyMid, Q, juce::Decibels::decibelsToGain(gainMid));
    pre_eq.get<2>() = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, frequencyHigh, Q, juce::Decibels::decibelsToGain(gainHigh));

    
    //-----------------------------3 EQ Knobs Parameters----------------------------------------------
    
    auto chainSettings = getChainSettings(apvts);

    auto lowKnob = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        800.f,
        Q,
        juce::Decibels::decibelsToGain(chainSettings.GainLow));
    
    auto midKnob = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        2000.f,
        Q,
        juce::Decibels::decibelsToGain(chainSettings.GainMid));
    auto highKnob = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        5500.f,
        Q,
        juce::Decibels::decibelsToGain(chainSettings.GainHigh));
   
    *leftChain.get<ChainPos::lowK>().coefficients = *lowKnob;
    *rightChain.get<ChainPos::lowK>().coefficients = *lowKnob;

    *leftChain.get<ChainPos::midK>().coefficients = *midKnob;
    *rightChain.get<ChainPos::midK>().coefficients = *midKnob;

    *leftChain.get<ChainPos::highK>().coefficients = *highKnob;
    *rightChain.get<ChainPos::highK>().coefficients = *highKnob;
    

    //------------------------------Distortion Parameters----------------------------------------------

    // Load the first impulse response file
    juce::File impulseResponseFile1 = juce::File::getCurrentWorkingDirectory()
        .getChildFile("Resources")
        .getChildFile("TM_CL_01_Catflower.wav");

    // Check if the file exists
    if (impulseResponseFile1.exists())
    {
        // Initialize the convolver with the impulse response
        convolver_clean.loadImpulseResponse(impulseResponseFile1,
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::no,
            0, // Set the size to 0 to use the original impulse response size
            juce::dsp::Convolution::Normalise::yes);
    }
    else
    {
        //it does not work
    }


    //second file
    juce::File impulseResponseFile2 = juce::File::getCurrentWorkingDirectory()
        .getChildFile("Resources")
        .getChildFile("TM_LD_01_BiteMe.wav");

    // Check if the file exists
    if (impulseResponseFile2.exists())
    {
        // Initialize the convolver with the impulse response
        convolver_fuzz.loadImpulseResponse(impulseResponseFile2,
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::no,
            0, // Set the size to 0 to use the original impulse response size
            juce::dsp::Convolution::Normalise::yes);
    }
    else
    {
        //it does not work
    }

    // Prepare the convolvers
         //convolve1
    juce::dsp::ProcessSpec spec1;
    spec1.sampleRate = sampleRate;
    spec1.maximumBlockSize = static_cast<size_t>(samplesPerBlock);
    spec1.numChannels = getTotalNumInputChannels();

    convolver_clean.prepare(spec1);

    //convolve2
    juce::dsp::ProcessSpec spec2;
    spec2.sampleRate = sampleRate;
    spec2.maximumBlockSize = static_cast<size_t>(samplesPerBlock);
    spec2.numChannels = getTotalNumInputChannels();

    convolver_fuzz.prepare(spec2);

    //------------------------------Chorus and Reverb  Parameters----------------------------------------------
    juce::dsp::ProcessSpec specChorusRev;

    specChorusRev.sampleRate = sampleRate;
    specChorusRev.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    specChorusRev.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());

    chorus.prepare(specChorusRev);

    reverb.prepare(specChorusRev);


}

void EMToneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EMToneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void EMToneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
   ///////////////////////////////////////////////////////////////////////////////////////////
    //we update our coeficients before we run audio through it

    ////// Apply pre-compression
    /*juce::dsp::AudioBlock<float> block_Comp(buffer);
    pre_compressor.process(juce::dsp::ProcessContextReplacing<float>(block_Comp));*/

    ////// Apply pre-EQ
    juce::dsp::AudioBlock<float> block_EQ(buffer);
    pre_eq.process(juce::dsp::ProcessContextReplacing<float>(block_EQ));


    //-----------------------------3 EQ Knobs----------------------------------------------
    auto chainSettings = getChainSettings(apvts);

    auto lowKnob = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        800.f,
        2.0f,
        juce::Decibels::decibelsToGain(chainSettings.GainLow));

    auto midKnob = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        2000.f,
        2.0f,
        juce::Decibels::decibelsToGain(chainSettings.GainMid));
    auto highKnob = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
        5500.f,
        2.0f,
        juce::Decibels::decibelsToGain(chainSettings.GainHigh));

    *leftChain.get<ChainPos::lowK>().coefficients = *lowKnob;
    *rightChain.get<ChainPos::lowK>().coefficients = *lowKnob;

    *leftChain.get<ChainPos::midK>().coefficients = *midKnob;
    *rightChain.get<ChainPos::midK>().coefficients = *midKnob;

    *leftChain.get<ChainPos::highK>().coefficients = *highKnob;
    *rightChain.get<ChainPos::highK>().coefficients = *highKnob;


    ////// Process the audio buffer with the convolution effect
    juce::dsp::AudioBlock<float> block_clean(buffer);
    convolver_clean.process(juce::dsp::ProcessContextReplacing<float>(block_clean));

    ////// Process the audio buffer with the 2nd convolution effect
    juce::dsp::AudioBlock<float> block_fuzz(buffer);
    convolver_fuzz.process(juce::dsp::ProcessContextReplacing<float>(block_fuzz));

    ////// Apply the Distortion effect to each sample in the buffer
    const int numSamples = buffer.getNumSamples();
    const float gain = gainParam->get();
    const float tone = toneParam->get();
    const float volume = volumeParam->get();

    for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float inputValue = channelData[sample];

            // Apply the overdrive effect to the current sample
            float overdriveValue = inputValue * gain;
            overdriveValue = std::tanh(overdriveValue * tone);
            overdriveValue *= volume;

            channelData[sample] = overdriveValue;
        }
    }

    
    // Apply chorus effect
    updateChorusParams();
    juce::dsp::AudioBlock<float> block_ch(buffer);
    juce::dsp::ProcessContextReplacing<float> ctx_chorus(block_ch);
    chorus.process(ctx_chorus);


   ////// Apply Reverb
    updateReverbParams();
    juce::dsp::AudioBlock<float> block_rv(buffer);
    juce::dsp::ProcessContextReplacing<float> ctx(block_rv);
    reverb.process(ctx);


   ///extract the left and right chanel using from buffer 
    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    //now we create proccesing context 
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    //now we pass this context to our mono filters
    leftChain.process(leftContext);
    rightChain.process(rightContext);
    ///////////////////////////////////////////////////////////////////////////////////////////     
        
}

//==============================================================================
bool EMToneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EMToneAudioProcessor::createEditor()
{

    //return new EMToneAudioProcessorEditor (*this);
  
    return new juce::GenericAudioProcessorEditor(*this);

}

//==============================================================================
void EMToneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    /*juce::MemoryOutputStream stream(destData, false);
    state_Dist->state.writeToStream(stream);*/

}

void EMToneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

   /* juce::ValueTree tree = juce::ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid())
    {
        state_Dist->state = tree;
    }*/


}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EMToneAudioProcessor();
}



//=========================== My Functions ===================================



void EMToneAudioProcessor::updateChorusParams()
{

    chorus.setRate(chorusRate->get());
    chorus.setDepth(chorusDepth->get());
    chorus.setMix(chorusMix->get());
    
}

void EMToneAudioProcessor::updateReverbParams()
{

    params.roomSize = size->get();
    params.damping = damp->get();
    params.width = width->get();
    params.wetLevel = reverbMix->get();
    params.dryLevel = 1.0f - reverbMix->get();
    params.freezeMode = freeze->get();

    reverb.setParameters(params);

}








