/*
  ==============================================================================

    PolygonVoice.cpp
    Created: 19 Feb 2020 8:38:43am
    Author:  bowen

  ==============================================================================
*/

#include "PolygonVoice.h"
#include "PolygonSound.h"
#include "PolygonAlgorithm.h"

PolygonVoice::PolygonVoice(AudioProcessorValueTreeState& apvts, ADSR::Parameters& envParams, PolygonSynthAlgorithm::PolygonCache& c) :
    parameters(apvts),
    envParams(envParams),
    cache(c)
{
    envelope.setParameters(envParams);
}

PolygonVoice::~PolygonVoice()
{
}

bool PolygonVoice::canPlaySound(SynthesiserSound* sound)
{
    return dynamic_cast<PolygonSound*>(sound) != nullptr;
}

void PolygonVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
    currentNoteNumber = midiNoteNumber;
    updatePitchBend(currentPitchWheelPosition);

    updatePhaseIncrement();
    level = velocity;
    envelope.noteOn();
}

void PolygonVoice::resetSmoothedValues(float order, float teeth, float fold)
{
    this->order.reset(getSampleRate(), 0.1);
    this->teeth.reset(getSampleRate(), 0.1);
    this->fold.reset(getSampleRate(), 0.1);

    this->order.setCurrentAndTargetValue(order);
    this->teeth.setCurrentAndTargetValue(teeth);
    this->fold.setCurrentAndTargetValue(fold);
}

void PolygonVoice::setTargetValues(float order, float teeth, float fold, float rotation, float fmRatio, float fmAmt)
{
    this->order.setTargetValue(order);
    this->teeth.setTargetValue(teeth);
    this->fold.setTargetValue(fold);
    this->rotation = rotation;
    this->fmRatio = fmRatio;
    this->fmAmt = fmAmt;
}

void PolygonVoice::stopNote(float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        envelope.noteOff();
    }
    else
    {
        clearCurrentNote();
        basePhaseIncrement = 0.0f;
        fmPhaseIncrement = 0.0f;
        maxPhaseIncrIncrement = 0.0f;
        rotationPhaseIncrement = 0.0f;
        prevValue = 0.0f;
        t = 0.0f;
        t_mod = 0.0f;
        t_rotation = 0.0f;
        envelope.reset();
    }
}

void PolygonVoice::updatePitchBend(int newPitchWheelValue)
{
    if (newPitchWheelValue == 16383)
        pitchBend = 1.0f;
    else if (newPitchWheelValue == 8192)
        pitchBend = 0.0f;
    else if (newPitchWheelValue == 0)
        pitchBend = -1.0f;
    else
        pitchBend = ((newPitchWheelValue / 16383.0f) * 2.0f) - 1.0f;
}

void PolygonVoice::updatePhaseIncrement()
{
    const auto frequency = 440.0f * std::pow(2.0f, (currentNoteNumber + pitchBend - 69.0f) / 12.0f);
    basePhaseIncrement = frequency / getSampleRate();
}

void PolygonVoice::updateModulationPhaseIncrement()
{
    const auto frequency = 440.0f * std::pow(2.0f, (currentNoteNumber + pitchBend - 69.0f) / 12.0f);
    fmPhaseIncrement = frequency * fmRatio / getSampleRate();
    maxPhaseIncrIncrement = frequency * fmRatio * fmAmt / getSampleRate();
    rotationPhaseIncrement = rotation * MathConstants<float>::twoPi / getSampleRate();
}

void PolygonVoice::pitchWheelMoved(int newPitchWheelValue)
{
    updatePitchBend(newPitchWheelValue);
    updatePhaseIncrement();
}

void PolygonVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

void PolygonVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    envelope.setParameters(envParams);

    auto* channelDataL = outputBuffer.getWritePointer(0);
    auto* channelDataR = outputBuffer.getWritePointer(1);

    for (auto sample = startSample; sample < startSample + numSamples; ++sample)
    {
        if (envelope.isActive())
        {
            const auto envValue = envelope.getNextSample();
            if (envValue != 0.0f)
            {
                const auto value = PolygonSynthAlgorithm::getSample(
                    t,
                    t_rotation,
                    order.getNextValue(),
                    teeth.getNextValue(),
                    fold.getNextValue(),
                    cache);

                t_mod += fmPhaseIncrement;
                t_rotation += rotationPhaseIncrement;
                t += basePhaseIncrement + maxPhaseIncrIncrement * std::sinf(MathConstants<float>::twoPi * t_mod);

                while (t < 0.0f)
                    t += 1.0f;
                while (t >= 1.0f)
                    t -= 1.0f;
                while (t_mod < 0.0f)
                    t_mod += 1.0f;
                while (t_mod >= 1.0f)
                    t_mod -= 1.0f;
                while (t_rotation < MathConstants<float>::twoPi)
                    t_rotation += MathConstants<float>::twoPi;
                while (t_rotation >= MathConstants<float>::twoPi)
                    t_rotation -= MathConstants<float>::twoPi;
                channelDataL[sample] += value.x * level * envValue;
                channelDataR[sample] += value.y * level * envValue;
            }
        }
        else
        {
            clearCurrentNote();
            basePhaseIncrement = 0.0f;
            fmPhaseIncrement = 0.0f;
            maxPhaseIncrIncrement = 0.0f;
            rotationPhaseIncrement = 0.0f;
            prevValue = 0.0f;
            t = 0.0f;
            t_mod = 0.0f;
            t_rotation = 0.0f;
            envelope.reset();
            break;
        }
    }
}

void PolygonVoice::setCurrentPlaybackSampleRate(double newRate)
{
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
    if (newRate != 0)
        envelope.setSampleRate(newRate);
}
