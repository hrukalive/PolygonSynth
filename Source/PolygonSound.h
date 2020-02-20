/*
  ==============================================================================

    PolygonSound.h
    Created: 19 Feb 2020 8:38:50am
    Author:  bowen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PolygonSound : public SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
