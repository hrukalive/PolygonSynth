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

PolygonVoice::PolygonVoice(AudioProcessorValueTreeState& apvts, ADSR::Parameters& envParams) :
    parameters(apvts),
    envParams(envParams)
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

void PolygonVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* /*sound*/, int currentPitchWheelPosition)
{
    currentNoteNumber = midiNoteNumber;
    updatePitchBend(currentPitchWheelPosition);

    updatePhaseIncrement();
    level = std::powf(velocity, _velGamma.load());
    envelope.noteOn();
}

void PolygonVoice::resetSmoothedValues(float order, float teeth, float fold)
{
    _order.reset(getSampleRate(), 0.1);
    _teeth.reset(getSampleRate(), 0.1);
    _fold.reset(getSampleRate(), 0.1);

    _order.setCurrentAndTargetValue(order);
    _teeth.setCurrentAndTargetValue(teeth);
    _fold.setCurrentAndTargetValue(fold);
}

void PolygonVoice::setTargetValues(float order, float teeth, float fold, float rotation, float fmRatio, float fmAmt)
{
    _order.setTargetValue(order);
    _teeth.setTargetValue(teeth);
    _fold.setTargetValue(fold);
    _rotation = rotation;
    _fmRatio = fmRatio;
    _fmAmt = fmAmt;
}

void PolygonVoice::stopNote(float /*velocity*/, bool allowTailOff)
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
    const auto frequency = 440.0 * std::pow(2.0f, (currentNoteNumber + pitchBend - 69.0f) / 12.0f);
    fmPhaseIncrement = frequency * _fmRatio / getSampleRate();
    maxPhaseIncrIncrement = frequency * _fmRatio * _fmAmt / getSampleRate();
    rotationPhaseIncrement = _rotation * MathConstants<float>::twoPi / getSampleRate();
}

void PolygonVoice::updateVelGamma(float velGamma)
{
    _velGamma = velGamma;
}

void PolygonVoice::pitchWheelMoved(int newPitchWheelValue)
{
    updatePitchBend(newPitchWheelValue);
    updatePhaseIncrement();
}

void PolygonVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
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
                    _order.getNextValue(),
                    _teeth.getNextValue(),
                    _fold.getNextValue());

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
