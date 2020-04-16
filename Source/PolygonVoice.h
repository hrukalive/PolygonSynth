/*
  ==============================================================================

    PolygonVoice.h
    Created: 19 Feb 2020 8:38:43am
    Author:  bowen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PolygonAlgorithm.h"

class PolygonVoice : public SynthesiserVoice
{
public:
    PolygonVoice(AudioProcessorValueTreeState&, ADSR::Parameters&);
    ~PolygonVoice();

    bool canPlaySound(SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound* /*sound*/, int currentPitchWheelPosition) override;
    void stopNote(float /*velocity*/, bool allowTailOff) override;
    void resetSmoothedValues(float order, float teeth, float fold);
    void setTargetValues(float order, float teeth, float fold, float rotation, float fmRatio, float fmAmt);

    void updatePitchBend(int newPitchWheelValue);
    void updatePhaseIncrement();
    void updateModulationPhaseIncrement();
    void updateVelGamma(float velGamma);
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/) override;

    void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void setCurrentPlaybackSampleRate(double newRate) override;

private:
    AudioProcessorValueTreeState& parameters;

    String centShiftParamNames[12]{ "centShiftOfC", "centShiftOfC#",
        "centShiftOfD", "centShiftOfD#", "centShiftOfE", "centShiftOfF", "centShiftOfF#",
        "centShiftOfG", "centShiftOfG#", "centShiftOfA", "centShiftOfA#", "centShiftOfB", };

    SmoothedValue<float> _order{ 2.0f }, _teeth{ 0.0f }, _fold{ 1.0f };
    float _rotation{ 0.0f }, _fmRatio{ 1.0f }, _fmAmt{ 0.0f };

    int currentNoteNumber{};
    float pitchBend{};
    std::atomic<float> basePhaseIncrement{}, rotationPhaseIncrement{}, fmPhaseIncrement{}, maxPhaseIncrIncrement{};
    std::atomic<float> _velGamma{ 1.0f };
    float t{}, t_rotation{}, t_mod{};

    float level{};

    ADSR::Parameters& envParams;
    ADSR envelope;

    float prevValue{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolygonVoice)
};

