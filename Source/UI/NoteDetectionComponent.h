#pragma once

#include <JuceHeader.h>

class NoteDetectionComponent
    : public juce::Component
{
public:
    NoteDetectionComponent();
    ~NoteDetectionComponent() override = default;

    void setMidiNote(int midiNote);
    void setVoiced(bool isVoiced);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:

    // =========================================================
    // NOTE INFORMATION
    // =========================================================

    static constexpr int numberOfNotes = 12;

    const juce::String noteNames[numberOfNotes]
    {
        "C",
        "C#",
        "D",
        "D#",
        "E",
        "F",
        "F#",
        "G",
        "G#",
        "A",
        "A#",
        "B"
    };

    int activeMidiNote = -1;
    bool voiced = false;

    // =========================================================
    // COLORS
    // =========================================================

    const juce::Colour backgroundColour = juce::Colour(0xff111216);
    const juce::Colour textColour       = juce::Colour(0xfff2f2f4);
    const juce::Colour mutedColour      = juce::Colour(0xff858791);
    const juce::Colour accentColour     = juce::Colour(0xffdf513e);

    // =========================================================
    // HELPERS
    // =========================================================

    int getPitchClass() const;
    int getOctave() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        NoteDetectionComponent)
};