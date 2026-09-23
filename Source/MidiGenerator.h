#pragma once

#include <JuceHeader.h>
#include <array>

class MidiGenerator
{
public:
    static constexpr int maxActiveNotes = 6;

    MidiGenerator();

    void reset();

    void processNote(
        int newNote,
        float velocity,
        juce::MidiBuffer& midiBuffer,
        int samplePosition);

    void processChord(
        const std::array<int, maxActiveNotes>& notes,
        int numNotes,
        float velocity,
        juce::MidiBuffer& midiBuffer,
        int samplePosition);

    void noteOff(
        juce::MidiBuffer& midiBuffer,
        int samplePosition);

    bool hasActiveNote() const;

    int getActiveNote() const;

    void setMidiChannel(int newChannel);

    int getMidiChannel() const;

private:
    int activeNote = -1;

    std::array<int, maxActiveNotes> activeNotes {};

    int activeNoteCount = 0;

    int midiChannel = 1;

    bool containsNote(
        const std::array<int, maxActiveNotes>& notes,
        int numNotes,
        int note) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        MidiGenerator)
};