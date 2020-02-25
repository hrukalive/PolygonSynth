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

class PolygonAudioProcessorEditor;
class PolygonAudioProcessor;

//==============================================================================
/*
*/
class PolygonDisplay    : public Component, public Timer, public AudioProcessorListener
{
public:
    PolygonDisplay(PolygonAudioProcessor& processor);
    ~PolygonDisplay();

    void paint(Graphics&) override;
    void resized() override;

    void timerCallback() override;
    void audioProcessorParameterChanged(AudioProcessor* /*processor*/, int /*parameterIndex*/, float /*newValue*/) override
    {
        changed = true;
    }
    void audioProcessorChanged(AudioProcessor* processor) override {}
    void audioProcessorParameterChangeGestureBegin(AudioProcessor* processor, int parameterIndex) override {}
    void audioProcessorParameterChangeGestureEnd(AudioProcessor* processor, int parameterIndex) override {}

private:
    PolygonAudioProcessor& processor;
    std::vector<float> waveX, waveY;

    std::shared_ptr<RingBuffer<float>>& ringBuffer;
    std::unique_ptr<Oscilloscope2D> oscilloscope2D;

    size_t resolution{ 512 };
    bool changed{ true };

    CriticalSection waveformLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolygonDisplay)
};
