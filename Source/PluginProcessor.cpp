/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

auto getPhaserRateName() { return juce::String("Phaser RateHz"); }
auto getPhaserCenterFreqName() { return juce::String("Phaser Center FreqHz"); }
auto getPhaserDepthName() { return juce::String("Phaser Depth %"); }
auto getPhaserFeedbackName() { return juce::String("Phaser Feedback %"); }
auto getPhaserMixName() { return juce::String("Phaser Mix %"); }

//==============================================================================
AudiopluginAudioProcessor::AudiopluginAudioProcessor()
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

}

AudiopluginAudioProcessor::~AudiopluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudiopluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudiopluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudiopluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudiopluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudiopluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudiopluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudiopluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudiopluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AudiopluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void AudiopluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AudiopluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AudiopluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudiopluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

juce::AudioProcessorValueTreeState::ParameterLayout AudiopluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const int versionHint = 1;
    /*
        Phaser:
        Rate: hz
        Depth: [0, 1]
        Center freq: hz
        Feedback: [-1, +1]
        Mix: [0, 1]
    */

    // Rate: [0.01, 2] Hz, 0.01 step, 0.2 Hz default
    auto name = getPhaserRateName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{name, versionHint},
            name,
            juce::NormalisableRange<float>(0.01f, 2.0f, 0.01f, 1.0f),
            0.2f,
            "Hz"
    ));

    // Phaser depth: [0, 1] %
    name = getPhaserDepthName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(0.01f, 1.0f, 0.01f, 1.0f),
        0.05f,
        "%"
    ));

    // Center Frequency: audio Hz [20, 20000] Hz, 1 step, 1000 Hz default
    name = getPhaserCenterFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 1.0f),
        1000.0f,
        "Hz"
    ));

    // Phaser feedback: [-1, 1] %
    name = getPhaserFeedbackName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f, 1.0f),
        0.0f,
        "%"
    ));

    // Phaser mix: [0, 1] %
    name = getPhaserMixName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(0.01f, 1.0f, 0.01f, 1.0f),
        0.05f,
        "%"
    ));

    return layout;
}

void AudiopluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
        buffer.clear (i, 0, buffer.getNumSamples());

    // [DONE]: add APVTS
    // TODO: create audio parameters for all dsp choices
    // TODO: update DSP ahere from audio parameters
    // TODO: save/load settings
    // TODO: save/load DSP order
    // TODO: Drag-To-Reorder GUI
    // TODO: GUI design for each DSP instance?
    // TODO: metering
    // TODO: prepare all DSP
    // TODO: wet/dry knob [BONUS]
    // TODO: mono & stero versions [mono is BONUS]
    // TODO: modulators [BONUS]
    // TODO: thread-safe filter updating [BONUS]
    // TODO: pre/post filtering [BONUS]
    // TODO: delay module [BONUS]

    auto newDSPOrder = DSP_Order();

    // try to pull
    while (dspOrderFifo.pull(newDSPOrder))
    {
        // continue
    }

    // if you pulled, replace dspOrder
    if (newDSPOrder != DSP_Order())
    {
        dspOrder = newDSPOrder;
    }

    // now convert dspOrder into an array of pointers
    DSP_Pointers dspPointers;
    for (size_t i = 0; i < dspPointers.size(); ++i)
    {
        switch (dspOrder[i])
        {
        case DSP_Option::Phase:
            dspPointers[i] = &phaser;
            break;
        case DSP_Option::Chorus:
            dspPointers[i] = &chorus;
            break;
        case DSP_Option::Overdrive:
            dspPointers[i] = &overdrive;
            break;
        case DSP_Option::LadderFilter:
            dspPointers[i] = &ladderFilter;
            break;
        case DSP_Option::END_OF_LIST:
            jassertfalse;
            break;
        default:
            break;
        }
    }

    // now process
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    for (size_t i = 0; i < dspPointers.size(); ++i)
    {
        if (dspPointers[i])
        {
            dspPointers[i]->process(context);
        }
    }
}

//==============================================================================
bool AudiopluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudiopluginAudioProcessor::createEditor()
{
    return new AudiopluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudiopluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AudiopluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudiopluginAudioProcessor();
}
