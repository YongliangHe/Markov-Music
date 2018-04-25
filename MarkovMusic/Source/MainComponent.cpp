/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
//==============================================================================
MainComponent::MainComponent(): keyboardComponent(keyboardState,                      MidiKeyboardComponent::horizontalKeyboard),
                                markovOrder(1),
                                markovNumIter(50)
{
    setSize (600, 450);
//    audioFilePath = "/Users/yonglianghe/Desktop/mozart.duos.k487_midip_02.mid";
    audioFilePath = "../../../../Resources/mozart.duos.k487_midip_02.mid";

    addAndMakeVisible(projTitleLabel);
    projTitleLabel.setFont(Font(20.0f, Font::bold));
    projTitleLabel.setText("Markov Chain Music Generator", dontSendNotification);
    projTitleLabel.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(playOrigButton);
    playOrigButton.setButtonText("Play the original midi");
    playOrigButton.onClick = [this] {playOrigButtonClicked();};
    
    addAndMakeVisible(playButton);
    playButton.setButtonText("Play Markov music");
    playButton.onClick = [this] {playButtonClicked();};
    
    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] {stopButtonClicked();};
    
    addAndMakeVisible(orderLabel);
    orderLabel.setText("Markov order:", dontSendNotification);
    addAndMakeVisible(orderSlider);
    orderSlider.setRange(1, 5, 1);
    orderSlider.onValueChange = [this] {markovOrder = (int)orderSlider.getValue();};
    
    addAndMakeVisible(iterLabel);
    iterLabel.setText("Number of notes:", dontSendNotification);
    addAndMakeVisible(iterSlider);
    iterSlider.setRange(50, 300, 1);
    iterSlider.onValueChange = [this] {markovNumIter = (int)iterSlider.getValue();};
    
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);
    
    setAudioChannels(2, 2);
    setSynth();
    
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    midiPlayer.setSampleRate(sampleRate);
    synth.setCurrentPlaybackSampleRate (sampleRate);

}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{

    bufferToFill.clearActiveBufferRegion();
    
    MidiBuffer incomingMidi;
    if (midiPlayer.getIsPlaying()) {
        int sampleDeltaToAdd = -midiPlayer.samplesPlayed;
        incomingMidi.addEvents(midiPlayer.getMidiBuffer(), midiPlayer.samplesPlayed, bufferToFill.numSamples, sampleDeltaToAdd);
        midiPlayer.samplesPlayed += bufferToFill.numSamples;
    }
    
    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    keyboardState.processNextMidiBuffer(incomingMidi, 0, bufferToFill.numSamples, false);
    
}

void MainComponent::releaseResources()
{
}

void MainComponent::playOrigButtonClicked() {
    midiParser.readFile(audioFilePath);
    midiPlayer.getMidiBuffer().clear();
    synth.allNotesOff(0, false);
    keyboardState.allNotesOff(0);
    midiPlayer.addOrigSequenceToBuffer(midiParser);
    midiPlayer.samplesPlayed = 0;
    midiPlayer.setIsPlaying(true);
}

void MainComponent::playButtonClicked() {
    midiParser.readFile(audioFilePath);
    vector<std::pair<int, double>> noteSequence = midiParser.getNoteSequenceForTrack(3);
    
    MidiBuffer::Iterator it(midiPlayer.getMidiBuffer());
    
    //! print for debug
    /*
    MidiMessage message;
    int samplePosition;
    std::cout << "button clicked!" << std::endl;
    while (it.getNextEvent(message, samplePosition)) {
        std::cout << message.getDescription() << "samplePostion: " << samplePosition << std::endl;
    }
    std::cout << "markovOrder:             " << markovOrder << std::endl;
    std::cout << "markovNumIter:           " << markovNumIter << std::endl;
    */
    vector<float> pitchSequence;
    vector<float> durationSequence;
    for (auto& note: noteSequence) {
        pitchSequence.push_back(static_cast<float>(note.first));
        durationSequence.push_back(static_cast<double>(note.second));
    }
     
    
    markovChain.init(pitchSequence, markovOrder);
    pitchSequence = markovChain.getSequences(markovNumIter);
    //! print for debug
    /*
    std::cout << "Generated pitch sequence: " << std::endl;
    for (int i = 0; i < pitchSequence.size(); i++) {
        std::cout << pitchSequence[i] << " ";
    }
    std::cout << std::endl;
     */
    markovChain.init(durationSequence, markovOrder);
    durationSequence = markovChain.getSequences(markovNumIter);
    
    noteSequence.clear();
    int size = pitchSequence.size() > durationSequence.size()? (int)durationSequence.size(): (int)pitchSequence.size();
    for (int i = 0; i < size; i++) {
        auto pair = std::make_pair(static_cast<int>(pitchSequence[i]), static_cast<double>(durationSequence[i]));
        noteSequence.push_back(pair);
    }
    midiPlayer.getMidiBuffer().clear();
    synth.allNotesOff(0, false);
    keyboardState.allNotesOff(0);
    midiPlayer.addSequenceToBuffer(noteSequence);
    midiPlayer.samplesPlayed = 0;
    midiPlayer.setIsPlaying(true);
}

void MainComponent::stopButtonClicked() {
    midiPlayer.setIsPlaying(false);
    midiPlayer.getMidiBuffer().clear();
    synth.allNotesOff(0, false);
    keyboardState.allNotesOff(0);
}

void MainComponent::setSynth() {
    for (auto i = 0; i < 4; ++i)
    {
        synth.addVoice (new SineWaveVoice());
    }
    synth.clearSounds();
    synth.addSound (new SineWaveSound());
}

void MainComponent::handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    
}

void MainComponent::handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    
}


//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{

    auto area = getLocalBounds();
    projTitleLabel.setBounds(area.removeFromTop(60).removeFromLeft(getWidth()).reduced(8));
    keyboardComponent.setBounds(area.removeFromTop(70).removeFromLeft(getWidth()).reduced(15, 0));
    iterLabel.setBounds(28, 160, getWidth() / 3, 30);
    iterSlider.setBounds(getWidth()/4, 160, getWidth() / 3 * 2, 30);
    orderLabel.setBounds(28, 210, getWidth() / 3, 30);
    orderSlider.setBounds(getWidth()/4, 210, getWidth() / 3 * 2, 30);
    playButton.setBounds(getWidth() / 2 - 100, 260, 200, 35);
    stopButton.setBounds(getWidth() / 2 - 100, 310, 200, 35);
    playOrigButton.setBounds(getWidth() / 2 - 100, 360, 200, 35);
    
}
