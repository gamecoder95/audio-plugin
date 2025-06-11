/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

// Phaser parameter name functions
auto getPhaserRateName() { return juce::String("Phaser RateHz"); }
auto getPhaserCenterFreqName() { return juce::String("Phaser Center FreqHz"); }
auto getPhaserDepthName() { return juce::String("Phaser Depth %"); }
auto getPhaserFeedbackName() { return juce::String("Phaser Feedback %"); }
auto getPhaserMixName() { return juce::String("Phaser Mix %"); }

// Chorus parameter name functions
auto getChorusRateName() { return juce::String("Chorus RateHz"); }
auto getChorusDepthName() { return juce::String("Chorus Depth %"); }
auto getChorusCenterDelayName() { return juce::String("Chorus Center DelayMs"); }
auto getChorusFeedbackName() { return juce::String("Chorus Feedback %"); }
auto getChorusMixName() { return juce::String("Chorus Mix %"); }

// Overdrive parameter name functions
auto getOverdriveSaturationName() { return juce::String("Overdrive Saturation"); }

// Ladder filter parameter name functions
auto getLadderFilterModeName() { return juce::String("Ladder Filer Mode"); }
auto getLadderFilterCutoffName() { return juce::String("Ladder Filer Cutoff Hz"); }
auto getLadderFilterResonanceName() { return juce::String("Ladder Filer Resonance"); }
auto getLadderFilterDriveName() { return juce::String("Ladder Filer Drive"); }

auto getLadderFilterChoices()
{
    return juce::StringArray {
        "LPF12",  // low-pass  12 dB/octave
        "HPF12",  // high-pass 12 dB/octave
        "BPF12",  // band-pass 12 dB/octave
        "LPF24",  // low-pass  24 dB/octave
        "HPF24",  // high-pass 24 dB/octave
        "BPF24"   // band-pass 24 dB/octave
    };
}

// General filter parameter name functions

auto getGeneralFilterChoices()
{
    return juce::StringArray{
        "Peak",
        "Bandpass",
        "Notch",
        "Allpass"
    };
}

auto getGeneralFilterModeName() { return juce::String("General Filter Mode"); }
auto getGeneralFilterFreqName() { return juce::String("General Filter Freq Hz"); }
auto getGeneralFilterQualityName() { return juce::String("General Filter Quality"); }
auto getGeneralFilterGainName() { return juce::String("General Filter Gain"); }

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
    // Initialize Float parameters

    auto floatParams = std::array {

        // Phaser params
        &phaserRateHz,
        &phaserCenterFreqHz,
        &phaserDepthPercent,
        &phaserFeedbackPercent,
        &phaserMixPercent,

        // Chorus params
        &chorusRateHz,
        &chorusDepthPercent,
        &chorusCenterDelayMs,
        &chorusFeedbackPercent,
        &chorusMixPercent,

        // Overdrive params
        &overdriveSaturation,

        // Ladder Filter params
        &ladderFilterCutoffHz,
        &ladderFilterResonance,
        &ladderFilterDrive,

        // General Filter params
        &generalFilterFreqHz,
        &generalFilterQuality,
        &generalFilterGain,
    };

    auto floatNameFuncs = std::array{

        // Phaser param name functions
        &getPhaserRateName,
        &getPhaserCenterFreqName,
        &getPhaserDepthName,
        &getPhaserFeedbackName,
        &getPhaserMixName,

        // Chorus param name functions
        &getChorusRateName,
        &getChorusDepthName,
        &getChorusCenterDelayName,
        &getChorusFeedbackName,
        &getChorusMixName,

        // Overdrive param name functions
        &getOverdriveSaturationName,

        // Ladder Filter param name functions
        &getLadderFilterCutoffName,
        &getLadderFilterResonanceName,
        &getLadderFilterDriveName,

        // General Filter param name functions
        &getGeneralFilterFreqName,
        &getGeneralFilterQualityName,
        &getGeneralFilterGainName,
    };

    for (size_t i = 0; i < floatParams.size(); ++i)
    {
        auto ptrToParamPtr = floatParams[i];
        *ptrToParamPtr = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(floatNameFuncs[i]()));
        jassert(*ptrToParamPtr != nullptr);
    }

    // Initialize Choice parameters

    auto choiceParams = std::array{

        // Ladder Filter params
        &ladderFilterMode,

        // General Filter params
        &generalFilterMode,
    };

    auto choiceNameFuncs = std::array{

        // Ladder Filter param name functions
        &getLadderFilterModeName, 
        
        // General Filter param name functions
        &getGeneralFilterModeName,
    };

    for (size_t i = 0; i < choiceParams.size(); ++i)
    {
        auto ptrToParamPtr = choiceParams[i];
        *ptrToParamPtr = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(choiceNameFuncs[i]()));
        jassert(*ptrToParamPtr != nullptr);
    }
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

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();

    std::vector<juce::dsp::ProcessorBase*> dsp{
        &phaser,
        &chorus,
        &overdrive,
        &ladderFilter,
        &generalFilter
    };

    for (auto p : dsp)
    {
        p->prepare(spec);
        p->reset();
    }
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

    // ===========PHASER===========================================

    /*
        Phaser:
        Rate: hz
        Center freq: hz
        Depth: [0, 1]
        Feedback: [-1, +1]
        Mix: [0, 1]
    */

    // Phaser Rate: [0.01, 2] Hz, 0.01 step, 0.2 Hz default
    auto name = getPhaserRateName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{name, versionHint},
            name,
            juce::NormalisableRange<float>(0.01f, 2.0f, 0.01f, 1.0f),
            0.2f,
            "Hz"
    ));

    // Phaser Center Frequency: audio Hz [20, 20000] Hz, 1 step, 1000 Hz default
    name = getPhaserCenterFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 1.0f),
        1000.0f,
        "Hz"
    ));

    // Phaser Depth: [0, 1] %
    name = getPhaserDepthName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(0.01f, 1.0f, 0.01f, 1.0f),
        0.05f,
        "%"
    ));

    // Phaser Feedback: [-1, 1] %
    name = getPhaserFeedbackName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f, 1.0f),
        0.0f,
        "%"
    ));

    // Phaser Mix: [0, 1] %
    name = getPhaserMixName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(0.01f, 1.0f, 0.01f, 1.0f),
        0.05f,
        "%"
    ));

    // ===========CHORUS===========================================

    /*
        Chorus:
        Rate: hz
        Depth: [0, 1]
        Center delay: [1 - 100] ms 
        Feedback: [-1, +1]
        Mix: [0, 1]
    */

    // Chorus Rate: [0.01, 100] Hz, 0.01 step, 0.2 Hz default
    name = getChorusRateName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(0.01f, 100.0f, 0.01f, 1.0f),
        0.2f,
        "Hz"
    ));

    // Chorus Depth: [0, 1] %
    name = getChorusDepthName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(0.01f, 1.0f, 0.01f, 1.0f),
        0.05f,
        "%"
    ));

    // Chorus Center Delay: [1, 100] ms, default 7 ms
    name = getChorusCenterDelayName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f, 1.0f),
        7.0f,
        "ms"
    ));

    // Chorus Feedback: [-1, 1] %
    name = getChorusFeedbackName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f, 1.0f),
        0.0f,
        "%"
    ));

    // Chorus Mix: [0, 1] %
    name = getChorusMixName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(0.01f, 1.0f, 0.01f, 1.0f),
        0.05f,
        "%"
    ));

    //===========OVERDRIVE===========================================

    /*
        (Uses the drive portion of the ladder filter class for now)
        Overdrive:
        Drive: [1, 100]
    */

    // Overdrive saturation: [1, 100]
    name = getOverdriveSaturationName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f, 1.0f),
        1.0f,
        ""
    ));

    //===========LADDER FILTER===========================================

    /*
        Ladder Filter:
        Mode: LadderFilterMode enum (int)
        Cutoff: hz
        Resonance: [0, 1]
        Drive: [1, 100]
    */

    // Ladder Filter Mode: LadderFilterMode enum choices as string array
    name = getLadderFilterModeName();
    auto choices = getLadderFilterChoices();
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ name, versionHint },
        name,
        choices,
        0
    ));

    // Ladder Filter cutoff: [20, 20000] Hz
    name = getLadderFilterCutoffName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 1.0f),
        20000.0f,
        "Hz"
    ));

    // Ladder Filter Resonance: [0, 1]
    name = getLadderFilterResonanceName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f),
        0.0f,
        ""
    ));

    // Ladder Filter Drive: [0, 100]
    name = getLadderFilterDriveName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f, 1.0f),
        1.0f,
        ""
    ));

    //===========GENERAL FILTER===========================================
    
    /*
        General Filter:
        Mode: Peak, Bandpass, Notch, Allpass
        Freq: [20, 20000] hz in 1hz steps
        Q(uality): [0.1, 10] in 0.05 steps
        Gain: [-24, 24] db in 0.5 db increments
    */

    // General Filter Mode: { Peak, Bandpass, Notch, Allpass } as string array
    name = getGeneralFilterModeName();
    choices = getGeneralFilterChoices();
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ name, versionHint },
        name,
        choices,
        0
    ));

    // General Filter cutoff: [20, 20000] Hz in 1 Hz steps
    name = getGeneralFilterFreqName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 1.0f),
        750.0f,
        "Hz"
    ));

    // General Filter quality: [0.1, 10] in 0.05 steps
    name = getGeneralFilterQualityName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.05f, 1.0f),
        1.0f,
        ""
    ));

    // General Filter gain: [-24, 24] db in 0.5 db steps
    name = getGeneralFilterGainName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ name, versionHint },
        name,
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.5f, 1.0f),
        0.0f,
        "dB"
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
    // [DONE]: create audio parameters for all dsp choices
    // TODO: update DSP ahere from audio parameters
    // TODO: save/load settings
    // TODO: save/load DSP order
    // TODO: Drag-To-Reorder GUI
    // TODO: GUI design for each DSP instance?
    // TODO: metering
    // [DONE]: prepare all DSP
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
        case DSP_Option::GeneralFilter:
            dspPointers[i] = &generalFilter;
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
