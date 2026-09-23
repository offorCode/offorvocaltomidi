#pragma once

#include <JuceHeader.h>

class NoteTracker
{
public:
    NoteTracker();

    void reset();

    int frequencyToMidiNote(float frequency) const;

    int processFrequency(float frequency,
                         float confidence);

    void processVoiceState(bool isVoiced);

    bool hasActiveNote() const;
    int getCurrentNote() const;

    bool shouldNoteOff() const;

    void setStabilityRequired(int numberOfFrames);
    void setNoteTolerance(float semitones);
    void setNoteOffDelayMs(float milliseconds);

    void setConfidenceThreshold(float threshold);
    float getConfidenceThreshold() const;

private:
    int currentNote = -1;

    int candidateNote = -1;
    int candidateFrames = 0;

    int requiredStableFrames = 2;

    // Allows small natural pitch movement without
    // changing the MIDI note.
    float noteTolerance = 0.35f;

    // Smoothed frequency.
    float smoothedFrequency = 0.0f;

    // Frequency smoothing amount.
    float frequencySmoothing = 0.30f;

    // Maximum allowed pitch jump between valid
    // consecutive detections before stabilization.
    float maximumJumpSemitones = 7.0f;

    int silenceFrames = 0;
    int noteOffDelayFrames = 3;

    float confidenceThreshold = 0.55f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        NoteTracker)
};