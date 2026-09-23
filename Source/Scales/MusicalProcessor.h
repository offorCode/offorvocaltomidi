#pragma once

#include <JuceHeader.h>

class MusicalProcessor
{
public:
    enum class Key
    {
        C = 0,
        CSharp,
        D,
        DSharp,
        E,
        F,
        FSharp,
        G,
        GSharp,
        A,
        ASharp,
        B
    };

    enum class Scale
    {
        Chromatic = 0,
        Major,
        Minor,
        MajorPentatonic,
        MinorPentatonic,
        Blues
    };

    MusicalProcessor();

    void reset();

    // =========================================================
    // KEY
    // =========================================================

    void setKey(Key newKey);
    Key getKey() const;

    // =========================================================
    // SCALE
    // =========================================================

    void setScale(Scale newScale);
    Scale getScale() const;

    // =========================================================
    // SCALE LOCK
    // =========================================================

    void setScaleLockEnabled(bool enabled);
    bool isScaleLockEnabled() const;

    // =========================================================
    // TRANSPOSE
    // =========================================================

    void setTranspose(int semitones);
    int getTranspose() const;

    // =========================================================
    // OCTAVE SHIFT
    // =========================================================

    void setOctaveShift(int octaves);
    int getOctaveShift() const;

    // =========================================================
    // PROCESS NOTE
    // =========================================================

    int processNote(int midiNote) const;

private:
    bool isNoteInScale(int midiNote) const;

    int quantizeToScale(int midiNote) const;

    // int getScaleDegree(int pitchClass) const;

    Key currentKey = Key::C;
    Scale currentScale = Scale::Chromatic;

    bool scaleLockEnabled = false;

    int transposeSemitones = 0;
    int octaveShift = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        MusicalProcessor)
};