/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StutterPluginAudioProcessor::StutterPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

StutterPluginAudioProcessor::~StutterPluginAudioProcessor()
{
}

//==============================================================================
const String StutterPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StutterPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StutterPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StutterPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StutterPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StutterPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int StutterPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StutterPluginAudioProcessor::setCurrentProgram (int index)
{
}

const String StutterPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void StutterPluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void StutterPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    playBuffer.setSize(2, sampleRate * 10.0);
    captureBuffer.setSize(2, sampleRate * 10.0);
    playBufferIndex = 0;
    captureBufferIndex = 0;
    curSampleRate = sampleRate;
}

void StutterPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StutterPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void StutterPluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedLock locker(cs);
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            captureBuffer.setSample(channel, captureBufferIndex, buffer.getSample(channel, sample));
        }
        ++captureBufferIndex;
        if (captureBufferIndex >= captureBuffer.getNumSamples())
            captureBufferIndex = 0;
    }
    if (stutterActive)
    {
        int fadelenSamples = 256;
        int stutterLenSamples = curSampleRate * stutterLength;
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float fadegain = 1.0f;
            if (playBufferIndex < fadelenSamples)
                fadegain = jmap<float>(playBufferIndex, 0, fadelenSamples, 0.0, 1.0f);
            if (playBufferIndex >= stutterLenSamples -fadelenSamples)
                fadegain = jmap<float>(playBufferIndex, stutterLenSamples-fadelenSamples, stutterLenSamples, 1.0, 0.0f);
            for (int channel = 0; channel < totalNumInputChannels; ++channel)
            {
                buffer.setSample(channel, sample, 0.5*fadegain*playBuffer.getSample(0, playBufferIndex));
            }
            ++playBufferIndex;
            if (playBufferIndex >= stutterLenSamples)
            {
                playBufferIndex = 0;
                //stutterLength = 0.0;
            }
                
        }
    }
}

//==============================================================================
bool StutterPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* StutterPluginAudioProcessor::createEditor()
{
    return new StutterPluginAudioProcessorEditor (*this);
}

//==============================================================================
void StutterPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    
}

void StutterPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    
}

void StutterPluginAudioProcessor::setStutter(bool stutter)
{
    ScopedLock locker(cs);
    stutterLength = 0.2; 
    playBufferIndex = 0;
    stutterActive = stutter;
    if (stutter == false)
        return;
    int playLength = curSampleRate * stutterLength;
    for (int i = 0; i < playLength; ++i)
    {
        int index = (captureBufferIndex - playLength) + i;
        // ok this isn't probably the smartest way to do this, but suffices for now
        if (index < 0)
            index = captureBuffer.getNumSamples() - (-index);
        if (index >= captureBuffer.getNumSamples())
            index = index % captureBuffer.getNumSamples();
        playBuffer.setSample(0, i, captureBuffer.getSample(0, index));
        playBuffer.setSample(1, i, captureBuffer.getSample(1, index));
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StutterPluginAudioProcessor();
}
