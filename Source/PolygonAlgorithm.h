/*
  ==============================================================================

    PolygonAlgorithm.h
    Created: 23 Feb 2020 9:04:36pm
    Author:  bowen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct PolygonSynthAlgorithm
{
public:
    static constexpr int maxVoices{ 20 };

    static inline Point<float> getSample(float t, float numVertices, float teeth, float fold, float rotation)
    {
        jassert(numVertices >= 2.0f);

        t -= rotation / MathConstants<float>::twoPi;
        if (t < 0.0f)
            t += 1.0f;

        jassert(t >= 0.0f && t <= 1.0f);

        auto n = t * numVertices;
        auto floor_n = std::floorf(n);
        auto fullLength = std::sqrtf(2.0f - 2.0f * std::cosf(MathConstants<float>::twoPi / numVertices));
        auto deltaAngle = MathConstants<float>::twoPi / numVertices;

        Point<float> p0 = Point<float>(std::cos(deltaAngle * floor_n), std::sin(deltaAngle * floor_n)).rotatedAboutOrigin(rotation);
        Point<float> p1 = Point<float>(std::cos(deltaAngle * (floor_n + 1)), std::sin(deltaAngle * (floor_n + 1))).rotatedAboutOrigin(rotation);

        Point<float> val;
        if (floor_n + 1 > numVertices)
        {
            auto arg_arccos = (1.0f - std::cosf(deltaAngle)) / 2.0f;
            auto int_angle = deltaAngle * std::floorf(numVertices);
            auto proportion = std::sinf(int_angle) / (fullLength * std::sinf(int_angle - std::acosf(std::sqrtf(arg_arccos))));
            val = Line<float>(p0 * (1 - teeth), p1 * (1 - (1 - proportion) * teeth)).getPointAlongLineProportionally(proportion * (n - floor_n) / (numVertices - floor_n));
        }
        else
        {
            val = Line<float>(p0 * (1 - teeth), p1).getPointAlongLineProportionally(n - floor_n);
        }

        auto waveX = val.getX() * fold;
        while (waveX > 1)
            waveX -= 2.0f;
        while (waveX < -1)
            waveX += 2.0f;
        auto waveY = val.getY() * fold;
        while (waveY > 1)
            waveY -= 2.0f;
        while (waveY < -1)
            waveY += 2.0f;

        jassert(!isnan(waveX) && !isnan(waveY));

        return Point<float>(waveX, waveY);
    }
};

