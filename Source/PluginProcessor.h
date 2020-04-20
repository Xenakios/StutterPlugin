/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class StutterPluginAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    StutterPluginAudioProcessor();
    ~StutterPluginAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void setStutter(bool b);
    double getStutterBufferPlayPos()
    {
        return 1.0 / playBuffer.getNumSamples() * playBufferIndex;
    }
    double getCaptureBufferPosition()
    {
        return 1.0 / captureBuffer.getNumSamples() * captureBufferIndex;
    }
    
private:
    AudioBuffer<float> captureBuffer;
    AudioBuffer<float> playBuffer;
    double stutterLength = 0.0;
    bool stutterActive = false;
    int captureBufferIndex = 0;
    int playBufferIndex = 0;
    CriticalSection cs;
    double curSampleRate = 0.0;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StutterPluginAudioProcessor)
};
