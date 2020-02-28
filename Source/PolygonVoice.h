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
    PolygonVoice(AudioProcessorValueTreeState&, ADSR::Parameters&, std::vector<float>&, std::vector<float>&);
    ~PolygonVoice();

    bool canPlaySound(SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void resetSmoothedValues();
    void setSmoothedValues(float order, float teeth, float fold, float rotation, float fmRatio, float fmAmt);

    void updatePitchBend(int newPitchWheelValue);
    void updatePhaseIncrement();
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    float getNextSample(bool isL);
    void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void setCurrentPlaybackSampleRate(double newRate) override;

private:
    AudioProcessorValueTreeState& parameters;
    std::vector<float> &wavetableL, &wavetableR;

    SmoothedValue<float> order, teeth, fold;
    float rotation, fmRatio, fmAmt;

    int currentNoteNumber{};
    float pitchBend{};
    float phaseIncrement{}, rotationPhaseIncrement{}, fmPhaseIncrement{}, maxPhaseIncrIncrement{};
    float t{}, t_rotation{}, t_mod{};

    float level{};

    ADSR::Parameters& envParams;
    ADSR envelope;

    float prevValue{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolygonVoice)
};

