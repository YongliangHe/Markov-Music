/*
  ==============================================================================

    MidiPlayer.cpp
    Created: 15 Apr 2018 9:47:32pm
    Author:  Yongliang He

  ==============================================================================
*/

#include "MidiPlayer.h"

MidiPlayer::MidiPlayer(): sampleRate(0), isPlaying(false), samplesPlayed(0)
{}

MidiPlayer::~MidiPlayer() {}

void MidiPlayer::setSampleRate(double fs) {
    sampleRate = fs;
}

MidiBuffer& MidiPlayer::getMidiBuffer() {
    return this->midiBuffer;
}

void MidiPlayer::setIsPlaying(bool state) {
    isPlaying = state;
}

bool MidiPlayer::getIsPlaying() {
    return isPlaying;
}

void MidiPlayer::addMessageToBuffer(const MidiMessage& message) {
    auto timestamp = message.getTimeStamp();
    auto sampleNumber = (int)(timestamp * sampleRate);
    midiBuffer.addEvent(message, sampleNumber);
}

void MidiPlayer::addSequenceToBuffer(vector<std::pair<int, double>> noteSequence) {
    double timestamp = 0;
    for (auto& note: noteSequence) {
        auto onMessage = MidiMessage::noteOn(1, note.first, 1.0f);
        onMessage.setTimeStamp(timestamp);
        addMessageToBuffer(onMessage);
        auto offMessage = MidiMessage::noteOff(onMessage.getChannel(), onMessage.getNoteNumber());
        offMessage.setTimeStamp(timestamp + note.second * 0.8);
        addMessageToBuffer(offMessage);
        timestamp += note.second;
    }
}

void MidiPlayer::addOrigSequenceToBuffer(MidiParser parser) {
    int numTrack = parser.getNumTrack();
    int numEvents = 0;
    const MidiMessageSequence* sequenceOfATrack = 0;
    for (int t = 0; t < numTrack; t++) {
        sequenceOfATrack = parser.getMessageSequence()[t];
        numEvents = sequenceOfATrack->getNumEvents();
        for (int i = 0; i < sequenceOfATrack->getNumEvents(); i++) {
            MidiMessage m = parser.getMessageSequence()[t]->getEventPointer(i)->message;
            addMessageToBuffer(m);
        }
    }
}

