#pragma once

#include <JuceHeader.h>

class VoiceDetector
{
public:
    VoiceDetector();

    void prepare(double newSampleRate);

    void reset();

    bool isVoiced(const float* samples,
                  int numSamples,
                  float& rmsLevel);

    void setThreshold(float newThreshold);

private:
    double sampleRate = 44100.0;

    float threshold = 0.008f;

    float smoothedLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoiceDetector)
};