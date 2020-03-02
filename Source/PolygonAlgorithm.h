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

    static inline Point<float> getSample(float t, float t_rot, float numVertices, float teeth, float fold)
    {
        if (numVertices < 2.0f)
            numVertices = 2.0f;

        t -= t_rot / MathConstants<float>::twoPi;
        while (t < 0.0f)
            t += 1.0f;

        jassert(t >= 0.0f && t <= 1.0f);

        auto n = t * numVertices;
        auto floor_n = std::floorf(n);

        Point<float> val;
        auto fullLength = std::sqrtf(2.0f - 2.0f * std::cosf(MathConstants<float>::twoPi / numVertices));
        auto deltaAngle = MathConstants<float>::twoPi / numVertices;

        Point<float> p0 = Point<float>(std::cos(deltaAngle * floor_n), std::sin(deltaAngle * floor_n));
        Point<float> p1 = Point<float>(std::cos(deltaAngle * (floor_n + 1)), std::sin(deltaAngle * (floor_n + 1)));

        if (floor_n + 1 > numVertices)
        {
            auto arg_arccos = (1.0f - std::cosf(deltaAngle)) / 2.0f;
            auto int_angle = deltaAngle * std::floorf(numVertices);
            auto proportion = std::sinf(int_angle) / (fullLength * std::sinf(int_angle - std::acosf(std::sqrtf(arg_arccos))));
            auto tmp = Line<float>(p0 * (1 - teeth), p1 * (1 - (1 - proportion) * teeth));
            val = tmp.getPointAlongLineProportionally(proportion * (n - floor_n) / (numVertices - floor_n)).rotatedAboutOrigin(t_rot);
            jassert(!isnan(val.getX()) && !isnan(val.getY()));
        }
        else
        {
            auto tmp = Line<float>(p0 * (1 - teeth), p1);
            val = tmp.getPointAlongLineProportionally(n - floor_n).rotatedAboutOrigin(t_rot);
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

        jassert(!isnan(waveX) && !isnan(waveY) && fabsf(waveX) <= 1.0f && std::fabsf(waveY) <= 1.0f);

        return Point<float>(waveX, waveY);
    }
};

