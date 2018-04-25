/*
  ==============================================================================

    MidiParser.h
    Created: 15 Apr 2018 2:51:09pm
    Author:  Yongliang He

  ==============================================================================
*/

#pragma once
#include <vector>
#include <map>
#include "../JuceLibraryCode/JuceHeader.h"
using std::vector;
using std::pair;
using std::map;

class MidiParser {
private:
    MidiFile midiFile;
    int numTrack;
//    const MidiMessageSequence **midiMessageSequence;
    Array<const MidiMessageSequence*> messageSequenceArray;
public:
    MidiParser();
    virtual ~MidiParser();
    void readFile(String path);
    void reset();
    void printEventsForTrack(int index);
    void printEventsForAll();
    int getNumTrack();
    Array< const MidiMessageSequence*> getMessageSequence();
    vector<std::pair<int, double>> getNoteSequenceForTrack(int index);
};
