#include "PitchDetector.h"

#include <cmath>
#include <algorithm>

PitchDetector::PitchDetector()
{
}

void PitchDetector::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;

    reset();
}

void PitchDetector::reset()
{
    analysisBuffer.fill(0.0f);
    difference.fill(0.0f);
    cumulativeDifference.fill(0.0f);

    bufferPosition = 0;
}

void PitchDetector::setMinFrequency(float frequencyHz)
{
    minFrequency = juce::jmax(20.0f, frequencyHz);

    if (maxFrequency <= minFrequency)
        maxFrequency = minFrequency + 1.0f;
}

void PitchDetector::setMaxFrequency(float frequencyHz)
{
    maxFrequency =
        juce::jmax(minFrequency + 1.0f, frequencyHz);
}

void PitchDetector::setThreshold(float newThreshold)
{
    threshold =
        juce::jlimit(0.05f, 0.5f, newThreshold);
}

bool PitchDetector::processBlock(const float* samples,
                                 int numSamples,
                                 float& detectedFrequency,
                                 float& confidence)
{
    detectedFrequency = 0.0f;
    confidence = 0.0f;

    if (samples == nullptr || numSamples <= 0)
        return false;

    bool foundPitch = false;

    int inputPosition = 0;

    while (inputPosition < numSamples)
    {
        const int samplesToCopy =
            juce::jmin(
                analysisSize - bufferPosition,
                numSamples - inputPosition);

        std::copy(
            samples + inputPosition,
            samples + inputPosition + samplesToCopy,
            analysisBuffer.begin() + bufferPosition);

        bufferPosition += samplesToCopy;
        inputPosition += samplesToCopy;

        if (bufferPosition >= analysisSize)
        {
            float frequency = 0.0f;
            float currentConfidence = 0.0f;

            if (findPitch(frequency, currentConfidence))
            {
                detectedFrequency = frequency;
                confidence = currentConfidence;

                foundPitch = true;
            }

            // 50% overlap.
            constexpr int overlapSize =
                analysisSize / 2;

            std::move(
                analysisBuffer.begin() + overlapSize,
                analysisBuffer.end(),
                analysisBuffer.begin());

            std::fill(
                analysisBuffer.begin() + overlapSize,
                analysisBuffer.end(),
                0.0f);

            bufferPosition = overlapSize;
        }
    }

    return foundPitch;
}

bool PitchDetector::findPitch(float& frequency,
                              float& confidence)
{
    frequency = 0.0f;
    confidence = 0.0f;

    // ------------------------------------------------------------
    // RMS / signal check
    // ------------------------------------------------------------

    double sumSquares = 0.0;

    for (float sample : analysisBuffer)
    {
        sumSquares +=
            static_cast<double>(sample) *
            static_cast<double>(sample);
    }

    const float rms =
        static_cast<float>(
            std::sqrt(
                sumSquares /
                static_cast<double>(analysisSize)));

    if (rms < 0.003f)
        return false;

    // ------------------------------------------------------------
    // Determine YIN search range
    // ------------------------------------------------------------

    int minTau =
        static_cast<int>(
            std::floor(
                sampleRate /
                static_cast<double>(maxFrequency)));

    int maxTau =
        static_cast<int>(
            std::ceil(
                sampleRate /
                static_cast<double>(minFrequency)));

    minTau =
        juce::jlimit(
            2,
            analysisSize / 4,
            minTau);

    maxTau =
        juce::jlimit(
            minTau + 1,
            analysisSize / 2,
            maxTau);

    // ------------------------------------------------------------
    // Difference function
    // ------------------------------------------------------------

    difference.fill(0.0f);
    cumulativeDifference.fill(0.0f);

    for (int tau = 1; tau <= maxTau; ++tau)
    {
        double sum = 0.0;

        const int limit =
            analysisSize - tau;

        for (int i = 0; i < limit; ++i)
        {
            const float delta =
                analysisBuffer[i] -
                analysisBuffer[i + tau];

            sum +=
                static_cast<double>(delta) *
                static_cast<double>(delta);
        }

        difference[tau] =
            static_cast<float>(sum);
    }

    // ------------------------------------------------------------
    // Cumulative mean normalized difference
    // ------------------------------------------------------------

    cumulativeDifference[0] = 1.0f;

    float runningSum = 0.0f;

    for (int tau = 1; tau <= maxTau; ++tau)
    {
        runningSum += difference[tau];

        if (runningSum > 0.0f)
        {
            cumulativeDifference[tau] =
                difference[tau] *
                static_cast<float>(tau) /
                runningSum;
        }
        else
        {
            cumulativeDifference[tau] = 1.0f;
        }
    }

    // ------------------------------------------------------------
    // Find first reliable YIN minimum
    // ------------------------------------------------------------

    int bestTau = -1;

    for (int tau = minTau;
         tau <= maxTau;
         ++tau)
    {
        if (cumulativeDifference[tau] < threshold)
        {
            while (
                tau + 1 <= maxTau &&
                cumulativeDifference[tau + 1] <
                    cumulativeDifference[tau])
            {
                ++tau;
            }

            bestTau = tau;
            break;
        }
    }

    // ------------------------------------------------------------
    // If threshold wasn't reached, find best candidate
    // ------------------------------------------------------------

    if (bestTau < 0)
    {
        float bestValue = 1.0f;

        for (int tau = minTau;
             tau <= maxTau;
             ++tau)
        {
            if (cumulativeDifference[tau] < bestValue)
            {
                bestValue =
                    cumulativeDifference[tau];

                bestTau = tau;
            }
        }

        if (bestTau < 0 ||
            bestValue > 0.45f)
        {
            return false;
        }
    }

    // ------------------------------------------------------------
    // Parabolic interpolation
    // ------------------------------------------------------------

    float refinedTau =
        static_cast<float>(bestTau);

    if (bestTau > minTau &&
        bestTau < maxTau)
    {
        const float left =
            cumulativeDifference[bestTau - 1];

        const float center =
            cumulativeDifference[bestTau];

        const float right =
            cumulativeDifference[bestTau + 1];

        const float denominator =
            left -
            2.0f * center +
            right;

        if (std::abs(denominator) > 0.000001f)
        {
            const float adjustment =
                0.5f *
                (left - right) /
                denominator;

            refinedTau += adjustment;
        }
    }

    if (refinedTau <= 0.0f)
        return false;

    // ------------------------------------------------------------
    // Convert period to frequency
    // ------------------------------------------------------------

    const float estimatedFrequency =
        static_cast<float>(
            sampleRate /
            static_cast<double>(refinedTau));

    if (!std::isfinite(estimatedFrequency))
        return false;

    if (estimatedFrequency < minFrequency ||
        estimatedFrequency > maxFrequency)
    {
        return false;
    }

    // ------------------------------------------------------------
    // Confidence
    // ------------------------------------------------------------

    const float yinValue =
        cumulativeDifference[bestTau];

    const float estimatedConfidence =
        juce::jlimit(
            0.0f,
            1.0f,
            1.0f - yinValue);

    if (estimatedConfidence < 0.50f)
        return false;

    frequency = estimatedFrequency;
    confidence = estimatedConfidence;

    return true;
}
