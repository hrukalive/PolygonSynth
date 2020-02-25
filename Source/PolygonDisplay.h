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
class PolygonDisplay    : public Component, public Timer
{
public:
    PolygonDisplay(PolygonAudioProcessorEditor& parent);
    ~PolygonDisplay();

    void paint(Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    PolygonAudioProcessorEditor& parent;
    std::vector<float> waveX, waveY;

    std::shared_ptr<RingBuffer<float>>& ringBuffer;
    std::unique_ptr<Oscilloscope2D> oscilloscope2D;

    size_t resolution{ 512 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolygonDisplay)
};
