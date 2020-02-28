/*
  ==============================================================================

    PolygonDisplay.h
    Created: 24 Feb 2020 6:56:20pm
    Author:  bowen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Oscilloscope2D.h"
#include "RingBuffer.h"
#include "PolygonAlgorithm.h"

class PolygonAudioProcessorEditor;
class PolygonAudioProcessor;

//==============================================================================
/*
*/
class PolygonDisplay : public Component, public Timer, public AudioProcessorValueTreeState::Listener
{
public:
    PolygonDisplay(PolygonAudioProcessor& processor);
    ~PolygonDisplay();

    void paint(Graphics&) override;
    void resized() override;
    void parameterChanged(const String& /*parameterID*/, float /*newValue*/) override
    {
        changed = true;
    }

    void timerCallback() override;

private:
    AudioProcessorValueTreeState& parameters;
    std::vector<float> waveX, waveY;

    std::shared_ptr<RingBuffer<float>>& ringBuffer;
    std::unique_ptr<Oscilloscope2D> oscilloscope2D;

    size_t resolution{ 1024 };
    bool changed{ true };

    CriticalSection waveformLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolygonDisplay)
};
