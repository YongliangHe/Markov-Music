/*
  ==============================================================================

    MidiPlayer.h
    Created: 15 Apr 2018 9:47:32pm
    Author:  Yongliang He

  ==============================================================================
*/

#pragma once
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiParser.h"

using std::vector;

class MidiPlayer {
private:
    MidiBuffer midiBuffer;
    double sampleRate;
    bool isPlaying;
public:
    MidiPlayer();
    virtual ~MidiPlayer();
    void setSampleRate(double fs);
    MidiBuffer& getMidiBuffer();
    void setIsPlaying(bool state);
    bool getIsPlaying();
    void addMessageToBuffer(const MidiMessage& message);
    void addSequenceToBuffer(vector<std::pair<int, double>> noteSequence);
    void addOrigSequenceToBuffer(MidiParser parser);
    
    int samplesPlayed;
    
    
};
