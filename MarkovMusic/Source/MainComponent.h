/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MarkovChain.h"
#include "MidiParser.h"
#include "MidiPlayer.h"
#include "SynthAudioSource.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent,
                        public MidiKeyboardStateListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void openButtonClicked();
    void playOrigButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void setSynth();
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    //! audio related
    MidiPlayer midiPlayer;
    MidiParser midiParser;
    MarkovChain markovChain;
    Synthesiser synth;
    MidiBuffer incomingMidi;

    //! GUI related
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent keyboardComponent;
    TextButton playOrigButton;
    TextButton playButton;
    TextButton stopButton;
    TextButton openButton;
    Label projTitleLabel;
    Slider orderSlider;
    Label orderLabel;
    Slider iterSlider;
    Label iterLabel;
    int markovOrder;
    int markovNumIter;
    String audioFilePath;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
