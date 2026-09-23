#include "MidiGenerator.h"

MidiGenerator::MidiGenerator()
{
    reset();
}

void MidiGenerator::reset()
{
    activeNote = -1;

    activeNotes.fill(-1);

    activeNoteCount = 0;
}

bool MidiGenerator::containsNote(
    const std::array<int, maxActiveNotes>& notes,
    int numNotes,
    int note) const
{
    for (int i = 0; i < numNotes; ++i)
    {
        if (notes[i] == note)
            return true;
    }

    return false;
}

void MidiGenerator::processNote(
    int newNote,
    float velocity,
    juce::MidiBuffer& midiBuffer,
    int samplePosition)
{
    if (newNote < 0 || newNote > 127)
        return;

    // Same note is already active.
    // Do not retrigger it.
    if (activeNoteCount == 1 &&
        activeNotes[0] == newNote)
    {
        return;
    }

    // If another note/chord is active, turn it off first.
    if (activeNoteCount > 0)
    {
        noteOff(midiBuffer, samplePosition);
    }

    const auto midiVelocity =
        static_cast<juce::uint8>(
            juce::jlimit(1.0f, 127.0f, velocity));

    midiBuffer.addEvent(
        juce::MidiMessage::noteOn(
            midiChannel,
            newNote,
            midiVelocity),
        samplePosition);

    activeNote = newNote;

    activeNotes.fill(-1);
    activeNotes[0] = newNote;
    activeNoteCount = 1;
}

void MidiGenerator::processChord(
    const std::array<int, maxActiveNotes>& notes,
    int numNotes,
    float velocity,
    juce::MidiBuffer& midiBuffer,
    int samplePosition)
{
    numNotes =
        juce::jlimit(
            0,
            maxActiveNotes,
            numNotes);

    if (numNotes <= 0)
        return;

    // Check whether the chord is exactly the same
    // as the currently active chord.
    bool sameChord =
        activeNoteCount == numNotes;

    if (sameChord)
    {
        for (int i = 0; i < numNotes; ++i)
        {
            if (activeNotes[i] != notes[i])
            {
                sameChord = false;
                break;
            }
        }
    }

    if (sameChord)
        return;

    // Turn off previous notes.
    if (activeNoteCount > 0)
    {
        noteOff(
            midiBuffer,
            samplePosition);
    }

    const auto midiVelocity =
        static_cast<juce::uint8>(
            juce::jlimit(
                1.0f,
                127.0f,
                velocity));

    activeNotes.fill(-1);

    activeNoteCount = 0;

    for (int i = 0; i < numNotes; ++i)
    {
        const int note = notes[i];

        if (note < 0 || note > 127)
            continue;

        midiBuffer.addEvent(
            juce::MidiMessage::noteOn(
                midiChannel,
                note,
                midiVelocity),
            samplePosition);

        activeNotes[activeNoteCount++] = note;
    }

    activeNote =
        activeNoteCount > 0
            ? activeNotes[0]
            : -1;
}

void MidiGenerator::noteOff(
    juce::MidiBuffer& midiBuffer,
    int samplePosition)
{
    for (int i = 0;
         i < activeNoteCount;
         ++i)
    {
        const int note =
            activeNotes[i];

        if (note < 0)
            continue;

        midiBuffer.addEvent(
            juce::MidiMessage::noteOff(
                midiChannel,
                note),
            samplePosition);
    }

    activeNotes.fill(-1);

    activeNoteCount = 0;

    activeNote = -1;
}

bool MidiGenerator::hasActiveNote() const
{
    return activeNoteCount > 0;
}

int MidiGenerator::getActiveNote() const
{
    return activeNote;
}

void MidiGenerator::setMidiChannel(
    int newChannel)
{
    midiChannel =
        juce::jlimit(
            1,
            16,
            newChannel);
}
    
int MidiGenerator::getMidiChannel() const
{
    return midiChannel;
}