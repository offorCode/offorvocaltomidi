#include "NoteTracker.h"

#include <cmath>

NoteTracker::NoteTracker()
{
}

void NoteTracker::reset()
{
    currentNote = -1;

    candidateNote = -1;
    candidateFrames = 0;

    smoothedFrequency = 0.0f;

    silenceFrames = 0;
}


// ==========================================================
// FREQUENCY -> MIDI
// ==========================================================

int NoteTracker::frequencyToMidiNote(
    float frequency) const
{
    if (frequency <= 0.0f)
        return -1;

    const float midi =
        69.0f +
        12.0f *
        std::log2(frequency / 440.0f);

    return juce::jlimit(
        0,
        127,
        static_cast<int>(
            std::round(midi)));
}


// ==========================================================
// PROCESS FREQUENCY
// ==========================================================

int NoteTracker::processFrequency(
    float frequency,
    float confidence)
{
    // ------------------------------------------------------
    // Reject invalid / low-confidence detection
    // ------------------------------------------------------

    if (frequency <= 0.0f ||
        confidence < confidenceThreshold)
    {
        candidateNote = -1;
        candidateFrames = 0;

        return currentNote;
    }

    // ------------------------------------------------------
    // First valid frequency
    // ------------------------------------------------------

    if (smoothedFrequency <= 0.0f)
    {
        smoothedFrequency = frequency;
    }
    else
    {
        const float previousMidi =
            69.0f +
            12.0f *
            std::log2(
                smoothedFrequency / 440.0f);

        const float newMidi =
            69.0f +
            12.0f *
            std::log2(
                frequency / 440.0f);

        const float jump =
            std::abs(
                newMidi - previousMidi);

        // --------------------------------------------------
        // Reject extremely large jumps.
        //
        // This prevents occasional YIN octave errors or
        // noisy detections from immediately moving the note.
        // --------------------------------------------------

        if (jump > maximumJumpSemitones)
        {
            candidateNote = -1;
            candidateFrames = 0;

            return currentNote;
        }

        smoothedFrequency =
            smoothedFrequency +
            (frequency - smoothedFrequency) *
            frequencySmoothing;
    }

    silenceFrames = 0;

    // ------------------------------------------------------
    // Convert smoothed frequency to MIDI
    // ------------------------------------------------------

    const int detectedNote =
        frequencyToMidiNote(
            smoothedFrequency);

    if (detectedNote < 0)
        return currentNote;

    // ------------------------------------------------------
    // No current note
    // ------------------------------------------------------

    if (currentNote < 0)
    {
        if (candidateNote == detectedNote)
        {
            ++candidateFrames;
        }
        else
        {
            candidateNote = detectedNote;
            candidateFrames = 1;
        }

        if (candidateFrames >= requiredStableFrames)
        {
            currentNote = detectedNote;

            candidateNote = -1;
            candidateFrames = 0;
        }

        return currentNote;
    }

    // ------------------------------------------------------
    // Exact pitch position
    // ------------------------------------------------------

    const float exactMidi =
        69.0f +
        12.0f *
        std::log2(
            smoothedFrequency / 440.0f);

    const float distanceFromCurrent =
        std::abs(
            exactMidi -
            static_cast<float>(
                currentNote));

    // ------------------------------------------------------
    // Still inside the current note
    // ------------------------------------------------------

    if (distanceFromCurrent < noteTolerance)
    {
        candidateNote = -1;
        candidateFrames = 0;

        return currentNote;
    }

    // ------------------------------------------------------
    // New note candidate
    // ------------------------------------------------------

    if (candidateNote == detectedNote)
    {
        ++candidateFrames;
    }
    else
    {
        candidateNote = detectedNote;
        candidateFrames = 1;
    }

    // ------------------------------------------------------
    // New note confirmed
    // ------------------------------------------------------

    if (candidateFrames >= requiredStableFrames)
    {
        currentNote = detectedNote;

        candidateNote = -1;
        candidateFrames = 0;
    }

    return currentNote;
}


// ==========================================================
// VOICE STATE
// ==========================================================

void NoteTracker::processVoiceState(
    bool isVoiced)
{
    if (isVoiced)
    {
        silenceFrames = 0;
        return;
    }

    ++silenceFrames;

    candidateNote = -1;
    candidateFrames = 0;
}


// ==========================================================
// NOTE STATE
// ==========================================================

bool NoteTracker::hasActiveNote() const
{
    return currentNote >= 0;
}

int NoteTracker::getCurrentNote() const
{
    return currentNote;
}

bool NoteTracker::shouldNoteOff() const
{
    return currentNote >= 0 &&
           silenceFrames >= noteOffDelayFrames;
}


// ==========================================================
// SETTINGS
// ==========================================================

void NoteTracker::setStabilityRequired(
    int numberOfFrames)
{
    requiredStableFrames =
        juce::jlimit(
            1,
            10,
            numberOfFrames);
}

void NoteTracker::setNoteTolerance(
    float semitones)
{
    noteTolerance =
        juce::jlimit(
            0.05f,
            1.0f,
            semitones);
}

void NoteTracker::setNoteOffDelayMs(
    float milliseconds)
{
    milliseconds =
        juce::jlimit(
            10.0f,
            500.0f,
            milliseconds);

    const double analysisIntervalSeconds =
        (1024.0 / 2.0) /
        44100.0;

    noteOffDelayFrames =
        juce::jmax(
            1,
            static_cast<int>(
                std::ceil(
                    (milliseconds / 1000.0) /
                    analysisIntervalSeconds)));
}

void NoteTracker::setConfidenceThreshold(
    float threshold)
{
    confidenceThreshold =
        juce::jlimit(
            0.30f,
            0.95f,
            threshold);
}

float NoteTracker::getConfidenceThreshold() const
{
    return confidenceThreshold;
}