/*
  ==============================================================================

    PolygonVoice.h
    Created: 19 Feb 2020 8:38:43am
    Author:  bowen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PolygonVoice : public SynthesiserVoice
{
public:
    PolygonVoice(AudioProcessorValueTreeState&, ADSR::Parameters&, std::vector<float>&);
    ~PolygonVoice() = default;

    bool canPlaySound(SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;

    void updatePitchBend(int newPitchWheelValue);
    void updatePhaseIncrement();
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    float getNextSample();
    void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void setCurrentPlaybackSampleRate(double newRate) override;

private:
    AudioProcessorValueTreeState& parameters;
    std::vector<float>& wavetable;

    int currentNoteNumber{};
    float pitchBend{};
    float phaseIncrement{};
    float t{};

    float level{};

    ADSR::Parameters& envParams;
    ADSR envelope;

    float prevValue{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolygonVoice)
};
