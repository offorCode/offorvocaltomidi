#pragma once

#include <JuceHeader.h>
#include <array>

class ChordGenerator
{
public:
    static constexpr int maxChordNotes = 6;

    enum class Mode
    {
        SingleNote = 0,
        Chord,
        Arpeggio
    };

    enum class Genre
    {
        Afrobeats = 0,
        HipHop,
        Drill,
        RnB,
        Gospel,
        Trap,
        Pop,
        Highlife,
        Amapiano,
        Jazz
    };

    enum class ChordType
    {
        Major = 0,
        Minor,
        Major7,
        Minor7,
        Dominant7,
        Diminished,
        Augmented,
        Sus2,
        Sus4,
        Major6,
        Minor6,
        Major9,
        Minor9,
        Dominant9,
        Major11,
        Minor11,
        Dominant11,
        SixNine
    };

    enum class Voicing
    {
        Close = 0,
        Open,
        Wide
    };

    ChordGenerator();

    void reset();

    void setMode(Mode newMode);
    Mode getMode() const;

    void setGenre(Genre newGenre);
    Genre getGenre() const;

    void setChordType(ChordType newChord);
    ChordType getChordType() const;

    void setVoicing(Voicing newVoicing);
    Voicing getVoicing() const;

    void setOctave(int newOctave);
    int getOctave() const;

    int getNumChordNotes() const;

    int getChordNote(int rootNote,
                     int index) const;

    int generateChord(
        int rootNote,
        std::array<int, maxChordNotes>& notes) const;

private:
    Mode mode = Mode::SingleNote;

    Genre genre = Genre::Afrobeats;

    ChordType chordType = ChordType::Major;

    Voicing voicing = Voicing::Close;

    int octave = 0;

    int addIntervalsForChord(
        std::array<int, maxChordNotes>& intervals) const;

    void applyVoicing(
        std::array<int, maxChordNotes>& notes,
        int numNotes) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ChordGenerator)
};