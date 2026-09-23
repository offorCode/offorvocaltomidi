#pragma once

#include <JuceHeader.h>

#include "../PluginProcessor.h"

class SettingsPage
    : public juce::Component
{
public:
    explicit SettingsPage(
        OfforVocalToMidiAudioProcessor& processor);

    ~SettingsPage() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void loadCurrentValues();

    std::function<void()> onBack;

private:
    void showTab(int tabIndex);
    void updateTabButtons();

    OfforVocalToMidiAudioProcessor& audioProcessor;

    // =========================================================
    // SCROLLING CONTENT
    // =========================================================

    juce::Viewport settingsViewport;
    juce::Component settingsContent;

    // =========================================================
    // HEADER
    // =========================================================

    juce::TextButton backButton;
    juce::Label titleLabel;

    // =========================================================
    // TOP TABS
    //
    // 0 = Pitch
    // 1 = Voice
    // 2 = MIDI
    // 3 = Musical
    // 4 = Output
    // =========================================================

    juce::TextButton pitchTabButton;
    juce::TextButton voiceTabButton;
    juce::TextButton midiTabButton;
    juce::TextButton musicalTabButton;
    juce::TextButton outputTabButton;

    int currentTab = 0;

    // =========================================================
    // PITCH / DETECTION
    // =========================================================

    juce::Label pitchSectionLabel;

    juce::Label confidenceLabel;
    juce::Label stabilityLabel;
    juce::Label toleranceLabel;
    juce::Label noteOffLabel;

    juce::Slider confidenceSlider;
    juce::Slider stabilitySlider;
    juce::Slider toleranceSlider;
    juce::Slider noteOffSlider;

    // =========================================================
    // PITCH RANGE
    // =========================================================

    juce::Label pitchRangeLabel;
    juce::Label pitchRangePresetLabel;
    juce::Label minFrequencyLabel;
    juce::Label maxFrequencyLabel;

    juce::ComboBox pitchRangePresetBox;

    juce::Slider minFrequencySlider;
    juce::Slider maxFrequencySlider;

    // =========================================================
    // VOICE
    // =========================================================

    juce::Label voiceSectionLabel;

    juce::Label voiceThresholdLabel;
    juce::Label pitchThresholdLabel;

    juce::Slider voiceThresholdSlider;
    juce::Slider pitchThresholdSlider;

    // =========================================================
    // MIDI
    // =========================================================

    juce::Label midiSectionLabel;

    juce::Label midiChannelLabel;
    juce::Label velocityModeLabel;
    juce::Label fixedVelocityLabel;
    juce::Label octaveShiftLabel;
    juce::Label transposeLabel;

    juce::Slider midiChannelSlider;
    juce::ComboBox velocityModeBox;
    juce::Slider fixedVelocitySlider;
    juce::Slider octaveShiftSlider;
    juce::Slider transposeSlider;

    // =========================================================
    // MUSICAL / SCALE
    // =========================================================

    juce::Label musicalSectionLabel;

    juce::Label scaleLockLabel;
    juce::ToggleButton scaleLockButton;

    juce::Label musicalKeyLabel;
    juce::ComboBox musicalKeyBox;

    juce::Label musicalScaleLabel;
    juce::ComboBox musicalScaleBox;

    // =========================================================
    // CHORD GENERATION
    // =========================================================

    juce::Label chordSectionLabel;

    juce::Label midiModeLabel;
    juce::ComboBox midiModeBox;

    juce::Label chordGenreLabel;
    juce::ComboBox chordGenreBox;

    juce::Label chordTypeLabel;
    juce::ComboBox chordTypeBox;

    juce::Label chordVoicingLabel;
    juce::ComboBox chordVoicingBox;

    juce::Label chordOctaveLabel;
    juce::Slider chordOctaveSlider;

    // =========================================================
    // OUTPUT
    // =========================================================

    juce::Label outputSectionLabel;
    juce::Label outputInfoLabel;

    // =========================================================
    // ABOUT / SUPPORT
    // =========================================================

    juce::Label aboutSectionLabel;
    juce::Label versionLabel;

    juce::TextButton userGuideButton;
    juce::TextButton feedbackButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        SettingsPage)
};