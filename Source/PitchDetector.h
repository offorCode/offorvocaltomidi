#pragma once

#include <JuceHeader.h>

class PitchDetector
{
public:
    PitchDetector();

    void prepare(double newSampleRate);
    void reset();

    bool processBlock(const float* samples,
                      int numSamples,
                      float& detectedFrequency,
                      float& confidence);

    void setMinFrequency(float frequencyHz);
    void setMaxFrequency(float frequencyHz);
    void setThreshold(float newThreshold);

private:
    static constexpr int analysisSize = 1024;
    static constexpr int maxDifferenceSize = analysisSize / 2 + 1;

    double sampleRate = 44100.0;

    float minFrequency = 80.0f;
    float maxFrequency = 1000.0f;
    float threshold = 0.15f;

    std::array<float, analysisSize> analysisBuffer {};

    std::array<float, maxDifferenceSize> difference {};
    std::array<float, maxDifferenceSize> cumulativeDifference {};

    int bufferPosition = 0;

    bool findPitch(float& frequency,
                   float& confidence);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchDetector)
};