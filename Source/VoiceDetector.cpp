#include "VoiceDetector.h"

#include <cmath>

VoiceDetector::VoiceDetector()
{
}

void VoiceDetector::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;

    reset();
}

void VoiceDetector::reset()
{
    smoothedLevel = 0.0f;
}

void VoiceDetector::setThreshold(float newThreshold)
{
    threshold =
        juce::jlimit(
            0.0001f,
            0.2f,
            newThreshold
        );
}

bool VoiceDetector::isVoiced(const float* samples,
                             int numSamples,
                             float& rmsLevel)
{
    rmsLevel = 0.0f;

    if (samples == nullptr || numSamples <= 0)
        return false;

    double sumSquares = 0.0;

    for (int i = 0; i < numSamples; ++i)
    {
        const float sample = samples[i];

        sumSquares +=
            static_cast<double>(sample) * sample;
    }

    const float rms =
        static_cast<float>(
            std::sqrt(
                sumSquares /
                static_cast<double>(numSamples)
            )
        );

    // Smooth the level to avoid rapid gate chatter.
    smoothedLevel =
        0.85f * smoothedLevel +
        0.15f * rms;

    rmsLevel = smoothedLevel;

    return smoothedLevel >= threshold;
}