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

PolygonVoice::PolygonVoice(AudioProcessorValueTreeState& apvts, ADSR::Parameters& envParams,
    std::vector<float>& wavetableL, std::vector<float>& wavetableR) : parameters(apvts),
    wavetableL(wavetableL),
    wavetableR(wavetableR),
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

void PolygonVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
    currentNoteNumber = midiNoteNumber;
    updatePitchBend(currentPitchWheelPosition);

    //updatePhaseIncrement();
    level = velocity;
    envelope.noteOn();
}

void PolygonVoice::resetSmoothedValues()
{
    this->order.reset(getSampleRate(), 0.05);
    this->teeth.reset(getSampleRate(), 0.05);
    this->fold.reset(getSampleRate(), 0.05);
}

void PolygonVoice::setSmoothedValues(float order, float teeth, float fold, float rotation, float fmRatio, float fmAmt)
{
    this->order.reset(getSampleRate(), 0.05);
    this->order.setCurrentAndTargetValue(order);
    this->teeth.reset(getSampleRate(), 0.05);
    this->teeth.setCurrentAndTargetValue(teeth);
    this->fold.reset(getSampleRate(), 0.05);
    this->fold.setCurrentAndTargetValue(fold);
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
        phaseIncrement = 0.0f;
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
    const auto frequencyOfA = 440.0f;
    const auto frequency = frequencyOfA * std::pow(2.0f, (currentNoteNumber + pitchBend - 69.0f) / 12.0f);
    fmPhaseIncrement = (frequency * fmRatio) / getSampleRate();
    maxPhaseIncrIncrement = frequency * fmRatio * fmAmt / getSampleRate();
    phaseIncrement = (frequency + maxPhaseIncrIncrement * std::sinf(2 * MathConstants<float>::twoPi * t_mod)) / getSampleRate();
    rotationPhaseIncrement = rotation / getSampleRate();
}

void PolygonVoice::pitchWheelMoved(int newPitchWheelValue)
{
    updatePitchBend(newPitchWheelValue);
    updatePhaseIncrement();
}

void PolygonVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

float PolygonVoice::getNextSample(bool isL)
{
    auto& wavetable = isL ? wavetableL : wavetableR;

    const auto waveTablesize = wavetable.size();
    //linear interpolation between points in the wavetable	
    auto iFloating = t * waveTablesize;
    iFloating = std::min(waveTablesize - 1.0f, iFloating);
    const int iv0 = std::floor(iFloating);
    const int iv1 = std::ceil(iFloating);
    const auto b = iFloating - iv0;

    auto v0 = wavetable[iv0];
    auto v1 = wavetable[iv1];

    auto interpolated = (1.0f - b) * v0 + b * v1;

    auto value = interpolated;

    // if the path has duplicate points it sometimes returns nans
    if (std::isnan(value))
    {
        value = prevValue;
    }
    prevValue = value;
    return value;
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
                    order.getNextValue(),
                    teeth.getNextValue(),
                    fold.getNextValue(),
                    t_rotation);

                t_mod += fmPhaseIncrement;
                phaseIncrement += maxPhaseIncrIncrement * std::sinf(MathConstants<float>::twoPi * t_mod);
                t += phaseIncrement;
                t_rotation += rotationPhaseIncrement;

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
            phaseIncrement = 0.0f;
            rotationPhaseIncrement = 0.0f;
            prevValue = 0.0f;
            t = 0.0f;
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
