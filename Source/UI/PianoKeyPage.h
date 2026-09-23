#pragma once

#include <JuceHeader.h>

class PianoKeyboardComponent
    : public juce::Component
{
public:
    PianoKeyboardComponent();

    void setMidiNote(int midiNote);

    void paint(juce::Graphics& g) override;

private:
    int activeMidiNote = -1;

    static constexpr int firstMidiNote = 36; // C2
    static constexpr int lastMidiNote  = 96; // C7

    bool isBlackKey(int midiNote) const;

    juce::String getNoteName(int midiNote) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        PianoKeyboardComponent)
};


class PianoKeyPage
    : public juce::Component,
      private juce::Timer
{
public:
    PianoKeyPage();

    void setMidiNote(int midiNote);

    std::function<void()> onBack;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;

    PianoKeyboardComponent keyboard;

    juce::TextButton backButton;
    juce::Label titleLabel;
    juce::Label noteLabel;

    int currentMidiNote = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        PianoKeyPage)
};