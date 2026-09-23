#include "MusicalProcessor.h"

#include <cmath>

MusicalProcessor::MusicalProcessor()
{
}

void MusicalProcessor::reset()
{
    currentKey = Key::C;
    currentScale = Scale::Chromatic;

    scaleLockEnabled = false;

    transposeSemitones = 0;
    octaveShift = 0;
}

// =========================================================
// KEY
// =========================================================

void MusicalProcessor::setKey(Key newKey)
{
    currentKey = newKey;
}

MusicalProcessor::Key MusicalProcessor::getKey() const
{
    return currentKey;
}

// =========================================================
// SCALE
// =========================================================

void MusicalProcessor::setScale(Scale newScale)
{
    currentScale = newScale;
}

MusicalProcessor::Scale MusicalProcessor::getScale() const
{
    return currentScale;
}

// =========================================================
// SCALE LOCK
// =========================================================

void MusicalProcessor::setScaleLockEnabled(bool enabled)
{
    scaleLockEnabled = enabled;
}

bool MusicalProcessor::isScaleLockEnabled() const
{
    return scaleLockEnabled;
}

// =========================================================
// TRANSPOSE
// =========================================================

void MusicalProcessor::setTranspose(int semitones)
{
    transposeSemitones =
        juce::jlimit(-24, 24, semitones);
}

int MusicalProcessor::getTranspose() const
{
    return transposeSemitones;
}

// =========================================================
// OCTAVE SHIFT
// =========================================================

void MusicalProcessor::setOctaveShift(int octaves)
{
    octaveShift =
        juce::jlimit(-3, 3, octaves);
}

int MusicalProcessor::getOctaveShift() const
{
    return octaveShift;
}

// =========================================================
// SCALE MEMBERSHIP
// =========================================================

bool MusicalProcessor::isNoteInScale(int midiNote) const
{
    if (midiNote < 0 || midiNote > 127)
        return false;

    if (currentScale == Scale::Chromatic)
        return true;

    static constexpr bool majorScale[12] =
    {
        true,  false, true,  false,
        true,  true,  false, true,
        false, true,  false, true
    };

    static constexpr bool minorScale[12] =
    {
        true,  false, true,  true,
        false, true,  false, true,
        true,  false, true,  false
    };

    static constexpr bool majorPentatonic[12] =
    {
        true,  false, true,  false,
        true,  false, false, true,
        false, false, true, false
    };

    static constexpr bool minorPentatonic[12] =
    {
        true,  false, false, true,
        false, true,  false, true,
        false, false, true, false
    };

    static constexpr bool bluesScale[12] =
    {
        true,  false, false, true,
        false, true,  true,  true,
        false, false, true, false
    };

    const int keyPitchClass =
        static_cast<int>(currentKey);

    const int relativePitchClass =
        (midiNote - keyPitchClass + 12) % 12;

    switch (currentScale)
    {
        case Scale::Major:
            return majorScale[relativePitchClass];

        case Scale::Minor:
            return minorScale[relativePitchClass];

        case Scale::MajorPentatonic:
            return majorPentatonic[relativePitchClass];

        case Scale::MinorPentatonic:
            return minorPentatonic[relativePitchClass];

        case Scale::Blues:
            return bluesScale[relativePitchClass];

        case Scale::Chromatic:
        default:
            return true;
    }
}

// =========================================================
// SCALE DEGREE
// =========================================================

// int MusicalProcessor::getScaleDegree(int pitchClass) const
// {
//     if (currentScale == Scale::Chromatic)
//         return 0;

//     const int keyPitchClass =
//         static_cast<int>(currentKey);

//     const int relativePitchClass =
//         (pitchClass - keyPitchClass + 12) % 12;

//     static constexpr int majorScale[] =
//     {
//         0, 2, 4, 5, 7, 9, 11
//     };

//     static constexpr int minorScale[] =
//     {
//         0, 2, 3, 5, 7, 8, 10
//     };

//     static constexpr int majorPentatonic[] =
//     {
//         0, 2, 4, 7, 9
//     };

//     static constexpr int minorPentatonic[] =
//     {
//         0, 3, 5, 7, 10
//     };

//     static constexpr int bluesScale[] =
//     {
//         0, 3, 5, 6, 7, 10
//     };

//     const int* scale = nullptr;
//     int scaleSize = 0;

//     switch (currentScale)
//     {
//         case Scale::Major:
//             scale = majorScale;
//             scaleSize = 7;
//             break;

//         case Scale::Minor:
//             scale = minorScale;
//             scaleSize = 7;
//             break;

//         case Scale::MajorPentatonic:
//             scale = majorPentatonic;
//             scaleSize = 5;
//             break;

//         case Scale::MinorPentatonic:
//             scale = minorPentatonic;
//             scaleSize = 5;
//             break;

//         case Scale::Blues:
//             scale = bluesScale;
//             scaleSize = 6;
//             break;

//         case Scale::Chromatic:
//         default:
//             return 0;
//     }

//     for (int i = 0; i < scaleSize; ++i)
//     {
//         if (scale[i] == relativePitchClass)
//             return 0;
//     }

//     return 0;
// }

// =========================================================
// QUANTIZATION
// =========================================================

int MusicalProcessor::quantizeToScale(int midiNote) const
{
    if (midiNote < 0 || midiNote > 127)
        return midiNote;

    if (currentScale == Scale::Chromatic)
        return midiNote;

    if (isNoteInScale(midiNote))
        return midiNote;

    // Search outward from the detected note.
    // Prefer the closest scale note.
    for (int distance = 1; distance <= 6; ++distance)
    {
        const int lower = midiNote - distance;
        const int upper = midiNote + distance;

        if (lower >= 0 &&
            isNoteInScale(lower))
        {
            return lower;
        }

        if (upper <= 127 &&
            isNoteInScale(upper))
        {
            return upper;
        }
    }

    return midiNote;
}

// =========================================================
// PROCESS NOTE
// =========================================================

int MusicalProcessor::processNote(int midiNote) const
{
    if (midiNote < 0 || midiNote > 127)
        return -1;

    int processedNote = midiNote;

    // ---------------------------------------------------------
    // Scale quantization
    // ---------------------------------------------------------

    if (scaleLockEnabled)
    {
        processedNote =
            quantizeToScale(processedNote);
    }

    // ---------------------------------------------------------
    // Transpose
    // ---------------------------------------------------------

    processedNote += transposeSemitones;

    // ---------------------------------------------------------
    // Octave shift
    // ---------------------------------------------------------

    processedNote +=
        octaveShift * 12;

    // ---------------------------------------------------------
    // Final MIDI range
    // ---------------------------------------------------------

    processedNote =
        juce::jlimit(
            0,
            127,
            processedNote);

    return processedNote;
}