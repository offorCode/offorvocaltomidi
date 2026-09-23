#include "ChordGenerator.h"

ChordGenerator::ChordGenerator()
{
}

void ChordGenerator::reset()
{
}

void ChordGenerator::setMode(Mode newMode)
{
    mode = newMode;
}

ChordGenerator::Mode
ChordGenerator::getMode() const
{
    return mode;
}

void ChordGenerator::setGenre(Genre newGenre)
{
    genre = newGenre;
}

ChordGenerator::Genre
ChordGenerator::getGenre() const
{
    return genre;
}

void ChordGenerator::setChordType(ChordType newChord)
{
    chordType = newChord;
}

ChordGenerator::ChordType
ChordGenerator::getChordType() const
{
    return chordType;
}

void ChordGenerator::setVoicing(Voicing newVoicing)
{
    voicing = newVoicing;
}

ChordGenerator::Voicing
ChordGenerator::getVoicing() const
{
    return voicing;
}

void ChordGenerator::setOctave(int newOctave)
{
    octave = juce::jlimit(-3, 3, newOctave);
}

int ChordGenerator::getOctave() const
{
    return octave;
}

int ChordGenerator::addIntervalsForChord(
    std::array<int, maxChordNotes>& intervals) const
{
    int count = 0;

    switch (chordType)
    {
        case ChordType::Major:
            intervals[count++] = 0;
            intervals[count++] = 4;
            intervals[count++] = 7;
            break;

        case ChordType::Minor:
            intervals[count++] = 0;
            intervals[count++] = 3;
            intervals[count++] = 7;
            break;

        case ChordType::Major7:
            intervals[count++] = 0;
            intervals[count++] = 4;
            intervals[count++] = 7;
            intervals[count++] = 11;
            break;

        case ChordType::Minor7:
            intervals[count++] = 0;
            intervals[count++] = 3;
            intervals[count++] = 7;
            intervals[count++] = 10;
            break;

        case ChordType::Dominant7:
            intervals[count++] = 0;
            intervals[count++] = 4;
            intervals[count++] = 7;
            intervals[count++] = 10;
            break;

        case ChordType::Diminished:
            intervals[count++] = 0;
            intervals[count++] = 3;
            intervals[count++] = 6;
            break;

        case ChordType::Augmented:
            intervals[count++] = 0;
            intervals[count++] = 4;
            intervals[count++] = 8;
            break;

        case ChordType::Sus2:
            intervals[count++] = 0;
            intervals[count++] = 2;
            intervals[count++] = 7;
            break;

        case ChordType::Sus4:
            intervals[count++] = 0;
            intervals[count++] = 5;
            intervals[count++] = 7;
            break;

        case ChordType::Major6:
            intervals[count++] = 0;
            intervals[count++] = 4;
            intervals[count++] = 7;
            intervals[count++] = 9;
            break;

        case ChordType::Minor6:
            intervals[count++] = 0;
            intervals[count++] = 3;
            intervals[count++] = 7;
            intervals[count++] = 9;
            break;

        case ChordType::Major9:
            intervals[count++] = 0;
            intervals[count++] = 4;
            intervals[count++] = 7;
            intervals[count++] = 11;
            intervals[count++] = 14;
            break;

        case ChordType::Minor9:
            intervals[count++] = 0;
            intervals[count++] = 3;
            intervals[count++] = 7;
            intervals[count++] = 10;
            intervals[count++] = 14;
            break;

        case ChordType::Dominant9:
            intervals[count++] = 0;
            intervals[count++] = 4;
            intervals[count++] = 7;
            intervals[count++] = 10;
            intervals[count++] = 14;
            break;

        case ChordType::Major11:
            intervals[count++] = 0;
            intervals[count++] = 4;
            intervals[count++] = 7;
            intervals[count++] = 11;
            intervals[count++] = 14;
            intervals[count++] = 17;
            break;

        case ChordType::Minor11:
            intervals[count++] = 0;
            intervals[count++] = 3;
            intervals[count++] = 7;
            intervals[count++] = 10;
            intervals[count++] = 14;
            intervals[count++] = 17;
            break;

        case ChordType::Dominant11:
            intervals[count++] = 0;
            intervals[count++] = 4;
            intervals[count++] = 7;
            intervals[count++] = 10;
            intervals[count++] = 14;
            intervals[count++] = 17;
            break;

        case ChordType::SixNine:
            intervals[count++] = 0;
            intervals[count++] = 4;
            intervals[count++] = 7;
            intervals[count++] = 9;
            intervals[count++] = 14;
            break;
    }

    return count;
}

void ChordGenerator::applyVoicing(
    std::array<int, maxChordNotes>& notes,
    int numNotes) const
{
    if (voicing == Voicing::Close)
        return;

    if (voicing == Voicing::Open)
    {
        if (numNotes >= 3)
            notes[1] += 12;

        return;
    }

    if (voicing == Voicing::Wide)
    {
        if (numNotes >= 2)
            notes[1] += 12;

        if (numNotes >= 3)
            notes[2] += 24;
    }
}

int ChordGenerator::generateChord(
    int rootNote,
    std::array<int, maxChordNotes>& notes) const
{
    notes.fill(-1);

    if (rootNote < 0 || rootNote > 127)
        return 0;

    std::array<int, maxChordNotes> intervals {};

    const int numNotes =
        addIntervalsForChord(intervals);

    const int octaveOffset =
        octave * 12;

    int validNotes = 0;

    for (int i = 0; i < numNotes; ++i)
    {
        const int note =
            rootNote +
            octaveOffset +
            intervals[i];

        if (note >= 0 && note <= 127)
        {
            notes[validNotes++] = note;
        }
    }

    applyVoicing(
        notes,
        validNotes);

    // Clamp after voicing changes.
    for (int i = 0; i < validNotes; ++i)
    {
        notes[i] =
            juce::jlimit(
                0,
                127,
                notes[i]);
    }

    return validNotes;
}

int ChordGenerator::getNumChordNotes() const
{
    std::array<int, maxChordNotes> intervals {};

    return addIntervalsForChord(intervals);
}

int ChordGenerator::getChordNote(
    int rootNote,
    int index) const
{
    std::array<int, maxChordNotes> notes {};

    const int count =
        generateChord(
            rootNote,
            notes);

    if (index < 0 || index >= count)
        return -1;

    return notes[index];
}