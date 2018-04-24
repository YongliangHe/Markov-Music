/*
  ==============================================================================

    MidiParser.cpp
    Created: 15 Apr 2018 2:51:09pm
    Author:  Yongliang He

  ==============================================================================
*/

#include "MidiParser.h"
#include <iostream>
MidiParser::MidiParser(): numTrack(0),
                          midiMessageSequence(nullptr)
{
}

MidiParser::~MidiParser() {
    reset();
}

void MidiParser::readFile(String path) {
    File targetFile(path);
    FileInputStream stream(targetFile);
    midiFile.readFrom(stream);
    numTrack = midiFile.getNumTracks();
    midiFile.convertTimestampTicksToSeconds();
    midiMessageSequence = new const MidiMessageSequence*[numTrack];
    for (int i = 0; i < numTrack; i++) {
        midiMessageSequence[i] = midiFile.getTrack(i);
    }
}

void MidiParser::reset() {
//    delete[] midiMessageSequence;
    midiMessageSequence = nullptr;
}

void MidiParser::printEventsForTrack(int index) {
    int numEvents = midiMessageSequence[index]->getNumEvents();
    for (int i = 0; i < numEvents; i++) {
        std::cout << midiMessageSequence[index]->getEventPointer(i)->message.getDescription();
        std::cout << "time stamp: " << midiMessageSequence[index]->getEventPointer(i)->message.getTimeStamp() << " s" << std::endl;
    }
}

void MidiParser::printEventsForAll() {
    for (int i = 0; i < numTrack; i++) {
        printEventsForTrack(i);
    }
}

vector<pair<int, double>> MidiParser::getNoteSequenceForTrack(int index) {
    vector<pair<int, double>> resSequence; //!< midi pitch and note duration pair
    vector<pair<MidiMessage, double>> noteOnSequence;  //!< midi message and note duration pair
    map<int, double> notesOfChord; //!< midi pitch and note duration pair, used to get the highist note in a chord
    const MidiMessageSequence* sequence = midiMessageSequence[index];
    
    double duration = 0;
    for (int i = 0; i < sequence->getNumEvents(); i++) {
        if (sequence->getEventPointer(i)->message.isNoteOn()) {
            duration = sequence->getTimeOfMatchingKeyUp(i) - sequence->getEventPointer(i)->message.getTimeStamp();
            noteOnSequence.push_back(std::make_pair(sequence->getEventPointer(i)->message, duration));
        }
        
    }
    double currentTime = 0;
    double nextTime = 0;
    int noteNum = 0;
    for (int i = 0; i < noteOnSequence.size() - 1; i++) {
        currentTime = noteOnSequence[i].first.getTimeStamp();
        nextTime = noteOnSequence[i + 1].first.getTimeStamp();
        noteNum = noteOnSequence[i].first.getNoteNumber();
        if (currentTime == nextTime) {
            notesOfChord.insert(std::make_pair(noteNum, noteOnSequence[i].second));
            continue;
        }
        if (!notesOfChord.empty()) {
            for (auto it = notesOfChord.begin(); it != notesOfChord.end(); it++) {
                std::cout << "midi pitch: " << it->first << std::endl;
            }
            if (notesOfChord.rbegin()->first > noteNum) {
                resSequence.push_back(std::make_pair(notesOfChord.rbegin()->first, notesOfChord.rbegin()->second));
            } else {
                resSequence.push_back(std::make_pair(noteNum, noteOnSequence[i].second));
            }
            notesOfChord.clear();
            
        } else {
            resSequence.push_back(std::make_pair(noteNum, noteOnSequence[i].second));
        }
    }
    //! print for debug
    /*
    for (auto it = resSequence.begin(); it != resSequence.end(); it++) {
        std::cout << "pitch: " << it->first << "   duration: " << it->second << std::endl;
    }
     */
    return resSequence;
}

int MidiParser::getNumTrack() {
    return numTrack;
}

const MidiMessageSequence** MidiParser::getMessageSequence() {
    return midiMessageSequence;
}






