#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "UI/PianoKeyPage.h"
#include "UI/SettingsPage.h"
#include "UI/NoteDetectionComponent.h"

class OfforVocalToMidiAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      private juce::Timer
{
public:
    explicit OfforVocalToMidiAudioProcessorEditor(
        OfforVocalToMidiAudioProcessor&);

    ~OfforVocalToMidiAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    void showMainPage();
    void showSettingsPage();

    // =========================================================
    // UI DRAWING
    // =========================================================

    void drawPitchDisplay(juce::Graphics& g);
    void drawConfidenceMeter(juce::Graphics& g);

    // =========================================================
    // TEMPORARY FL STUDIO KEYBOARD TEST
    // =========================================================

    void sendTestC4();

    OfforVocalToMidiAudioProcessor& audioProcessor;

    // =========================================================
    // HEADER
    // =========================================================

    juce::Image offorLogo;

    juce::ImageButton settingsButton;

    // =========================================================
    // MAIN PAGE
    // =========================================================

    PianoKeyboardComponent pianoKeyboard;
    NoteDetectionComponent noteDetection;

    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::Label noteLabel;
    juce::Label frequencyLabel;
    juce::Label confidenceLabel;
    juce::Label midiLabel;

    // =========================================================
    // LIVE PITCH DISPLAY
    // =========================================================

    int displayedMidiNote = -1;
    float displayedFrequency = 0.0f;
    float displayedConfidence = 0.0f;
    bool displayedVoiced = false;

    // =========================================================
    // SETTINGS PAGE
    // =========================================================

    SettingsPage settingsPage;

    bool showingSettings = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        OfforVocalToMidiAudioProcessorEditor)
};