/*
  ==============================================================================

    PolygonDisplay.cpp
    Created: 24 Feb 2020 6:56:20pm
    Author:  bowen

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "PolygonDisplay.h"

//==============================================================================
PolygonDisplay::PolygonDisplay(PolygonAudioProcessor& p) :
    processor(p), ringBuffer(p.getRingBuffer())
{
    waveX.resize(resolution, 0.0f);
    waveY.resize(resolution, 0.0f);

    oscilloscope2D = std::make_unique<Oscilloscope2D>(ringBuffer);
    addChildComponent(*oscilloscope2D);
    oscilloscope2D->setVisible(true);
    oscilloscope2D->start();

    p.addListener(this);

    startTimer(100);
}

PolygonDisplay::~PolygonDisplay()
{
    processor.removeListener(this);
    oscilloscope2D->stop();
}

void PolygonDisplay::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    g.fillRect(getLocalBounds().reduced(1));

    ScopedLock lock(waveformLock);
    g.setColour (Colours::white);
    auto width = getWidth() / 2 - 8, height = getHeight() / 2 - 8;
    auto radius = jmin(width, height) / 2;
    auto amplitudeHeight = height / 2, amplitudeWidth = width / 2;
    Point<float> center1(getWidth() / 4, getHeight() / 4);
    auto deltaWidth = width / (float)waveY.size(), deltaHeight = height / (float)waveX.size();
    Point<float> starting1(width + 8 + 4, getHeight() / 4);
    Point<float> starting2(getWidth() / 4, height + 8 + 4);

    g.fillEllipse(radius * waveX[0] + center1.getX(), radius * -waveY[0] + center1.getY(), 1, 1);
    for (int i = 1; i < waveX.size(); i++)
    {
        g.fillEllipse(radius * waveX[i] + center1.getX(), radius * -waveY[i] + center1.getY(), 1, 1);
        g.drawLine(Line<float>(starting1 + Point<float>((i - 1) * deltaWidth, amplitudeHeight * waveY[i - 1]), starting1 + Point<float>(i * deltaWidth, amplitudeHeight * waveY[i])));
        g.drawLine(Line<float>(starting2 + Point<float>(amplitudeWidth * waveX[i - 1], (i - 1) * deltaHeight), starting2 + Point<float>(amplitudeWidth * waveX[i], i * deltaHeight)));
    }
}

void PolygonDisplay::resized()
{
    auto area = getLocalBounds();
    area.removeFromLeft(area.proportionOfWidth(0.5));
    area.removeFromTop(area.proportionOfHeight(0.5));
    area.reduce(4, 4);
    oscilloscope2D->setBounds(area);
}

void PolygonDisplay::timerCallback()
{
    if (changed)
    {
        changed = false;
        auto result = processor.generateWavetable(resolution);

        {
            ScopedLock lock(waveformLock);
            waveX = result.first;
            waveY = result.second;
        }
        repaint();
    }
}

