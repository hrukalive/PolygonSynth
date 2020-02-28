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
    parameters(p.getParameters()), ringBuffer(p.getRingBuffer())
{
    waveX.resize(resolution, 0.0f);
    waveY.resize(resolution, 0.0f);

    oscilloscope2D = std::make_unique<Oscilloscope2D>(ringBuffer);
    oscilloscope2D->updateBlocksize(256);// p.getBlockSize());
    addChildComponent(*oscilloscope2D);
    oscilloscope2D->setVisible(true);
    oscilloscope2D->start();

    parameters.addParameterListener("order", this);
    parameters.addParameterListener("teeth", this);
    parameters.addParameterListener("fold", this);

    startTimer(100);
}

PolygonDisplay::~PolygonDisplay()
{
    oscilloscope2D->stop();
    parameters.removeParameterListener("order", this);
    parameters.removeParameterListener("teeth", this);
    parameters.removeParameterListener("fold", this);
}

void PolygonDisplay::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    g.fillRect(getLocalBounds().reduced(1));

    ScopedLock lock(waveformLock);
    g.setColour (Colours::white);
    auto width = getWidth() / 2 - 8, height = getHeight() / 2 - 8;
    auto radius = jmin(width, height) / 2;
    Point<float> center1(getWidth() / 4, getHeight() / 4);
    auto deltaWidth = width / (float)waveY.size(), deltaHeight = height / (float)waveX.size();
    Point<float> starting1(width + 8 + 4, getHeight() / 4);
    Point<float> starting2(getWidth() / 4, height + 8 + 4);

    g.fillEllipse(radius * waveX[0] + center1.getX(), radius * -waveY[0] + center1.getY(), 1, 1);
    for (int i = 1; i < waveX.size(); i++)
    {
        g.fillEllipse(radius * waveX[i] + center1.getX(), radius * -waveY[i] + center1.getY(), 1, 1);
        g.drawLine(Line<float>(starting1 + Point<float>((i - 1) * deltaWidth, -radius * waveY[i - 1]), starting1 + Point<float>(i * deltaWidth, -radius * waveY[i])));
        g.drawLine(Line<float>(starting2 + Point<float>(radius * waveX[i - 1], (i - 1) * deltaHeight), starting2 + Point<float>(radius * waveX[i], i * deltaHeight)));
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

        const auto order = (float)parameters.getParameterAsValue("order").getValue();
        const auto teeth = (float)parameters.getParameterAsValue("teeth").getValue();
        const auto fold = (float)parameters.getParameterAsValue("fold").getValue();

        {
            ScopedLock lock(waveformLock);
            for (size_t i = 0; i < resolution; i++)
            {
                const auto value = PolygonSynthAlgorithm::getSample((float)i / (float)resolution, order, teeth, fold, 0);
                waveX[i] = value.getX();
                waveY[i] = value.getY();
            }
        }

        repaint();
    }
}

