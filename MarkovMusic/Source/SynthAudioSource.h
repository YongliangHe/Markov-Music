/*
 ==============================================================================
 
 synthSource.h
 Created: 15 Apr 2018 11:07:23pm
 Author:  Yongliang He
 
 ==============================================================================
 */

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


/** Our demo synth sound is just a basic sine wave.. */
struct SineWaveSound : public SynthesiserSound
{
    SineWaveSound() {}
    
    bool appliesToNote (int /*midiNoteNumber*/) override    { return true; }
    bool appliesToChannel (int /*midiChannel*/) override    { return true; }
};

//==============================================================================
/** Our demo synth voice just plays a sine wave.. */
struct SineWaveVoice  : public SynthesiserVoice
{
    SineWaveVoice() {}
    
    bool canPlaySound (SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }
    
    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;
        
        auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();
        
        angleDelta = cyclesPerSample * MathConstants<double>::twoPi;
    }
    
    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.
            
            if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
                tailOff = 1.0;  // stopNote method could be called more than once.
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }
    
    void pitchWheelMoved (int /*newValue*/) override                              {}
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override    {}
    
    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            if (tailOff > 0.0)
            {
                while (--numSamples >= 0)
                {
                    auto currentSample = (float) (std::sin (currentAngle) * level * tailOff);
                    
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);
                    
                    currentAngle += angleDelta;
                    ++startSample;
                    
                    tailOff *= 0.99;
                    
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    auto currentSample = (float) (std::sin (currentAngle) * level);
                    
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);
                    
                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }
    
private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
};

struct SynthAudioSource  : public AudioSource
{
    SynthAudioSource (MidiKeyboardState& keyState)  : keyboardState (keyState)
    {
        for (auto i = 0; i < 4; ++i)
        {
            synth.addVoice (new SineWaveVoice());
        }
        
        setUsingSineWaveSound();
    }
    
    void setUsingSineWaveSound()
    {
        synth.clearSounds();
        synth.addSound (new SineWaveSound());
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        midiCollector.reset (sampleRate);
        
        synth.setCurrentPlaybackSampleRate (sampleRate);
    }
    
    void releaseResources() override {}
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // the synth always adds its output to the audio buffer, so we have to clear it
        // first..
        bufferToFill.clearActiveBufferRegion();
        
        // fill a midi buffer with incoming messages from the midi input.
        MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);
        
        // pass these messages to the keyboard state so that it can update the component
        // to show on-screen which keys are being pressed on the physical midi keyboard.
        // This call will also add midi messages to the buffer which were generated by
        // the mouse-clicking on the on-screen keyboard.
        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);
        
        // and now get the synth to process the midi events and generate its output.
        synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    }
    
    //==============================================================================
    // this collects real-time midi messages from the midi input device, and
    // turns them into blocks that we can process in our audio callback
    MidiMessageCollector midiCollector;
    
    // this represents the state of which keys on our on-screen keyboard are held
    // down. When the mouse is clicked on the keyboard component, this object also
    // generates midi messages for this, which we can pass on to our synth.
    MidiKeyboardState& keyboardState;
    
    // the synth itself!
    Synthesiser synth;
};
