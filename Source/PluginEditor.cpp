/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StutterPluginAudioProcessorEditor::StutterPluginAudioProcessorEditor (StutterPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    addAndMakeVisible(butStutterActive);
    butStutterActive.setButtonText("Stutter");
    butStutterActive.onClick = [this]() { processor.setStutter(butStutterActive.getToggleState()); };
    setSize (400, 110);
    startTimer(50);
}

StutterPluginAudioProcessorEditor::~StutterPluginAudioProcessorEditor()
{
}

//==============================================================================
void StutterPluginAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::grey);
    float captureXcor = jmap<double>(processor.getCaptureBufferPosition(), 0.0, 1.0, 5.0, getWidth() - 10.0);
    g.setColour(Colours::white);
    g.drawLine(captureXcor, 35.0, captureXcor, 55.0);
    float playXcor = jmap<double>(processor.getStutterBufferPlayPos(), 0.0, 1.0, 5.0, getWidth() - 10.0);
    g.drawLine(playXcor, 60.0, playXcor, 80.0);
    
}

void StutterPluginAudioProcessorEditor::resized()
{
    butStutterActive.setBounds(5, 5, 100, 25);
}
