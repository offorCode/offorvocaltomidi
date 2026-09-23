#include "SettingsPage.h"
#include "../Version.h"

namespace
{
    const juce::Colour backgroundColour (0xff111216);
    const juce::Colour panelColour      (0xff1d1f25);
    const juce::Colour secondaryColour  (0xff17181d);
    const juce::Colour textColour       (0xfff2f2f4);
    const juce::Colour mutedColour      (0xff858791);
    const juce::Colour accentColour     (0xffdf513e);
    const juce::Colour accentDarkColour (0xffa93629);
}


// =============================================================
// CONSTRUCTOR
// =============================================================

SettingsPage::SettingsPage(
    OfforVocalToMidiAudioProcessor& processor)
    : audioProcessor(processor)
{

    // =========================================================
    // SETTINGS SCROLL VIEW
    // =========================================================

    addAndMakeVisible(settingsViewport);

    settingsViewport.setViewedComponent(
        &settingsContent,
        false);

    settingsViewport.setScrollBarsShown(
        true,
        false);

    settingsViewport.setScrollBarThickness(8);

    settingsContent.setSize(
        getWidth() - 32,
        700);
    // =========================================================
    // GENERAL BUTTON SETUP
    // =========================================================

    addAndMakeVisible(backButton);

    backButton.setButtonText("BACK");

    backButton.onClick =
        [this]
        {
            if (onBack)
                onBack();
        };


    // =========================================================
    // TITLE
    // =========================================================

    addAndMakeVisible(titleLabel);

    titleLabel.setText(
        "SETTINGS",
        juce::dontSendNotification);

    titleLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(20.0f)
                .withStyle("Bold")));

    titleLabel.setColour(
        juce::Label::textColourId,
        textColour);


    // =========================================================
    // TOP TABS
    // =========================================================

    addAndMakeVisible(pitchTabButton);
    addAndMakeVisible(voiceTabButton);
    addAndMakeVisible(midiTabButton);
    addAndMakeVisible(musicalTabButton);
    addAndMakeVisible(outputTabButton);

    pitchTabButton.setButtonText("Pitch");
    voiceTabButton.setButtonText("Voice");
    midiTabButton.setButtonText("MIDI");
    musicalTabButton.setButtonText("Musical");
    outputTabButton.setButtonText("Output");


    pitchTabButton.onClick =
        [this]
        {
            showTab(0);
        };

    voiceTabButton.onClick =
        [this]
        {
            showTab(1);
        };

    midiTabButton.onClick =
        [this]
        {
            showTab(2);
        };

    musicalTabButton.onClick =
        [this]
        {
            showTab(3);
        };

    outputTabButton.onClick =
        [this]
        {
            showTab(4);
        };


    // =========================================================
    // PITCH SECTION
    // =========================================================

    addAndMakeVisible(pitchSectionLabel);

    pitchSectionLabel.setText(
        "PITCH & DETECTION",
        juce::dontSendNotification);

    pitchSectionLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(18.0f)
                .withStyle("Bold")));

    pitchSectionLabel.setColour(
        juce::Label::textColourId,
        textColour);


    // Confidence
    addAndMakeVisible(confidenceLabel);

    confidenceLabel.setText(
        "Confidence Threshold",
        juce::dontSendNotification);

    addAndMakeVisible(confidenceSlider);

    confidenceSlider.setRange(
        0.30,
        0.95,
        0.01);

    confidenceSlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    confidenceSlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    confidenceSlider.setColour(
        juce::Slider::trackColourId,
        accentDarkColour);

    confidenceSlider.onValueChange =
        [this]
        {
            audioProcessor.setConfidenceThreshold(
                static_cast<float>(
                    confidenceSlider.getValue()));
        };


    // Stability
    addAndMakeVisible(stabilityLabel);

    stabilityLabel.setText(
        "Note Stability",
        juce::dontSendNotification);

    addAndMakeVisible(stabilitySlider);

    stabilitySlider.setRange(
        1,
        10,
        1);

    stabilitySlider.setNumDecimalPlacesToDisplay(0);

    stabilitySlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    stabilitySlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    stabilitySlider.onValueChange =
        [this]
        {
            audioProcessor.setNoteStability(
                static_cast<int>(
                    stabilitySlider.getValue()));
        };


    // Tolerance
    addAndMakeVisible(toleranceLabel);

    toleranceLabel.setText(
        "Note Tolerance",
        juce::dontSendNotification);

    addAndMakeVisible(toleranceSlider);

    toleranceSlider.setRange(
        0.05,
        1.0,
        0.05);

    toleranceSlider.setNumDecimalPlacesToDisplay(2);

    toleranceSlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    toleranceSlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    toleranceSlider.onValueChange =
        [this]
        {
            audioProcessor.setNoteTolerance(
                static_cast<float>(
                    toleranceSlider.getValue()));
        };


    // Note-Off Delay
    addAndMakeVisible(noteOffLabel);

    noteOffLabel.setText(
        "Note-Off Delay",
        juce::dontSendNotification);

    addAndMakeVisible(noteOffSlider);

    noteOffSlider.setRange(
        10,
        500,
        5);

    noteOffSlider.setNumDecimalPlacesToDisplay(0);

    noteOffSlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    noteOffSlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    noteOffSlider.onValueChange =
        [this]
        {
            audioProcessor.setNoteOffDelay(
                static_cast<float>(
                    noteOffSlider.getValue()));
        };


    // =========================================================
    // VOICE SECTION
    // =========================================================

    addAndMakeVisible(voiceSectionLabel);

    voiceSectionLabel.setText(
        "VOICE DETECTION",
        juce::dontSendNotification);

    voiceSectionLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(18.0f)
                .withStyle("Bold")));

    voiceSectionLabel.setColour(
        juce::Label::textColourId,
        textColour);


    // Voice Threshold
    addAndMakeVisible(voiceThresholdLabel);

    voiceThresholdLabel.setText(
        "Voice Threshold",
        juce::dontSendNotification);

    addAndMakeVisible(voiceThresholdSlider);

    voiceThresholdSlider.setRange(
        0.001,
        0.100,
        0.001);

    voiceThresholdSlider.setNumDecimalPlacesToDisplay(3);

    voiceThresholdSlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    voiceThresholdSlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    voiceThresholdSlider.onValueChange =
        [this]
        {
            audioProcessor.setVoiceThreshold(
                static_cast<float>(
                    voiceThresholdSlider.getValue()));
        };


    // Pitch Threshold
    addAndMakeVisible(pitchThresholdLabel);

    pitchThresholdLabel.setText(
        "Pitch Threshold",
        juce::dontSendNotification);

    addAndMakeVisible(pitchThresholdSlider);

    pitchThresholdSlider.setRange(
        0.01,
        1.0,
        0.01);

    pitchThresholdSlider.setNumDecimalPlacesToDisplay(2);

    pitchThresholdSlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    pitchThresholdSlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    pitchThresholdSlider.onValueChange =
        [this]
        {
            audioProcessor.setPitchThreshold(
                static_cast<float>(
                    pitchThresholdSlider.getValue()));
        };


    // =========================================================
    // MIDI SECTION
    // =========================================================

    addAndMakeVisible(midiSectionLabel);

    midiSectionLabel.setText(
        "MIDI OUTPUT",
        juce::dontSendNotification);

    midiSectionLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(18.0f)
                .withStyle("Bold")));

    midiSectionLabel.setColour(
        juce::Label::textColourId,
        textColour);


    // MIDI Channel
    addAndMakeVisible(midiChannelLabel);

    midiChannelLabel.setText(
        "MIDI Channel",
        juce::dontSendNotification);

    addAndMakeVisible(midiChannelSlider);

    midiChannelSlider.setRange(
        1,
        16,
        1);

    midiChannelSlider.setNumDecimalPlacesToDisplay(0);

    midiChannelSlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    midiChannelSlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    midiChannelSlider.onValueChange =
        [this]
        {
            audioProcessor.setMidiChannel(
                static_cast<int>(
                    midiChannelSlider.getValue()));
        };


    // Velocity Mode
    addAndMakeVisible(velocityModeLabel);

    velocityModeLabel.setText(
        "Velocity Mode",
        juce::dontSendNotification);

    addAndMakeVisible(velocityModeBox);

    velocityModeBox.addItem(
        "Dynamic",
        1);

    velocityModeBox.addItem(
        "Fixed",
        2);

    velocityModeBox.onChange =
        [this]
        {
            const int selectedId =
                velocityModeBox.getSelectedId();

            if (selectedId > 0)
            {
                audioProcessor.setVelocityMode(
                    selectedId - 1);
            }
        };


    // Fixed Velocity
    addAndMakeVisible(fixedVelocityLabel);

    fixedVelocityLabel.setText(
        "Fixed Velocity",
        juce::dontSendNotification);

    addAndMakeVisible(fixedVelocitySlider);

    fixedVelocitySlider.setRange(
        1,
        127,
        1);

    fixedVelocitySlider.setNumDecimalPlacesToDisplay(0);

    fixedVelocitySlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    fixedVelocitySlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    fixedVelocitySlider.onValueChange =
        [this]
        {
            audioProcessor.setFixedVelocity(
                static_cast<int>(
                    fixedVelocitySlider.getValue()));
        };


    // Octave Shift
    addAndMakeVisible(octaveShiftLabel);

    octaveShiftLabel.setText(
        "Octave Shift",
        juce::dontSendNotification);

    addAndMakeVisible(octaveShiftSlider);

    octaveShiftSlider.setRange(
        -3,
        3,
        1);

    octaveShiftSlider.setNumDecimalPlacesToDisplay(0);

    octaveShiftSlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    octaveShiftSlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    octaveShiftSlider.onValueChange =
        [this]
        {
            audioProcessor.setOctaveShift(
                static_cast<int>(
                    octaveShiftSlider.getValue()));
        };


    // Transpose
    addAndMakeVisible(transposeLabel);

    transposeLabel.setText(
        "Transpose",
        juce::dontSendNotification);

    addAndMakeVisible(transposeSlider);

    transposeSlider.setRange(
        -12,
        12,
        1);

    transposeSlider.setNumDecimalPlacesToDisplay(0);

    transposeSlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    transposeSlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    transposeSlider.onValueChange =
        [this]
        {
            audioProcessor.setTranspose(
                static_cast<int>(
                    transposeSlider.getValue()));
        };


    // =========================================================
    // MUSICAL SECTION
    // =========================================================

    addAndMakeVisible(musicalSectionLabel);

    musicalSectionLabel.setText(
        "MUSICAL SETTINGS",
        juce::dontSendNotification);

    musicalSectionLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(18.0f)
                .withStyle("Bold")));

    musicalSectionLabel.setColour(
        juce::Label::textColourId,
        textColour);


    // =========================================================
    // SCALE LOCK
    // =========================================================

    addAndMakeVisible(scaleLockLabel);

    scaleLockLabel.setText(
        "Scale Lock",
        juce::dontSendNotification);

    scaleLockLabel.setColour(
        juce::Label::textColourId,
        textColour);


    addAndMakeVisible(scaleLockButton);

    scaleLockButton.setButtonText(
        "Enable Scale Lock");

    scaleLockButton.setColour(
        juce::ToggleButton::textColourId,
        textColour);

    scaleLockButton.setColour(
        juce::ToggleButton::tickColourId,
        accentColour);

    scaleLockButton.onClick =
        [this]
        {
            audioProcessor.setScaleLockEnabled(
                scaleLockButton.getToggleState());
        };


    // =========================================================
    // MUSICAL KEY
    // =========================================================

    addAndMakeVisible(musicalKeyLabel);

    musicalKeyLabel.setText(
        "Key",
        juce::dontSendNotification);

    musicalKeyLabel.setColour(
        juce::Label::textColourId,
        textColour);


    addAndMakeVisible(musicalKeyBox);

    musicalKeyBox.addItem("C",  1);
    musicalKeyBox.addItem("C#", 2);
    musicalKeyBox.addItem("D",  3);
    musicalKeyBox.addItem("D#", 4);
    musicalKeyBox.addItem("E",  5);
    musicalKeyBox.addItem("F",  6);
    musicalKeyBox.addItem("F#", 7);
    musicalKeyBox.addItem("G",  8);
    musicalKeyBox.addItem("G#", 9);
    musicalKeyBox.addItem("A", 10);
    musicalKeyBox.addItem("A#", 11);
    musicalKeyBox.addItem("B", 12);

    musicalKeyBox.onChange =
        [this]
        {
            const int selectedId =
                musicalKeyBox.getSelectedId();

            if (selectedId > 0)
            {
                audioProcessor.setMusicalKey(
                    selectedId - 1);
            }
        };


    // =========================================================
    // MUSICAL SCALE
    // =========================================================

    addAndMakeVisible(musicalScaleLabel);

    musicalScaleLabel.setText(
        "Scale",
        juce::dontSendNotification);

    musicalScaleLabel.setColour(
        juce::Label::textColourId,
        textColour);


    addAndMakeVisible(musicalScaleBox);

    musicalScaleBox.addItem(
        "Chromatic",
        1);

    musicalScaleBox.addItem(
        "Major",
        2);

    musicalScaleBox.addItem(
        "Minor",
        3);

    musicalScaleBox.addItem(
        "Major Pentatonic",
        4);

    musicalScaleBox.addItem(
        "Minor Pentatonic",
        5);

    musicalScaleBox.addItem(
        "Blues",
        6);

    musicalScaleBox.onChange =
        [this]
        {
            const int selectedId =
                musicalScaleBox.getSelectedId();

            if (selectedId > 0)
            {
                audioProcessor.setMusicalScale(
                    selectedId - 1);
            }
        };


    // =========================================================
    // CHORD GENERATION
    // =========================================================

    addAndMakeVisible(chordSectionLabel);

    chordSectionLabel.setText(
        "CHORD GENERATION",
        juce::dontSendNotification);

    chordSectionLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(18.0f)
                .withStyle("Bold")));

    chordSectionLabel.setColour(
        juce::Label::textColourId,
        textColour);


    // =========================================================
    // MIDI MODE
    // =========================================================

    addAndMakeVisible(midiModeLabel);

    midiModeLabel.setText(
        "MIDI Mode",
        juce::dontSendNotification);


    addAndMakeVisible(midiModeBox);

    midiModeBox.addItem(
        "Single Note",
        1);

    midiModeBox.addItem(
        "Chord",
        2);

    midiModeBox.addItem(
        "Arpeggio",
        3);

    midiModeBox.onChange =
        [this]
        {
            const int selectedId =
                midiModeBox.getSelectedId();

            if (selectedId > 0)
            {
                audioProcessor.setMidiMode(
                    selectedId - 1);
            }
        };


    // =========================================================
    // CHORD GENRE
    // =========================================================

    addAndMakeVisible(chordGenreLabel);

    chordGenreLabel.setText(
        "Genre",
        juce::dontSendNotification);


    addAndMakeVisible(chordGenreBox);

    chordGenreBox.addItem(
        "Afrobeats",
        1);

    chordGenreBox.addItem(
        "Hip-Hop",
        2);

    chordGenreBox.addItem(
        "Drill",
        3);

    chordGenreBox.addItem(
        "R&B",
        4);

    chordGenreBox.addItem(
        "Gospel",
        5);

    chordGenreBox.addItem(
        "Trap",
        6);

    chordGenreBox.addItem(
        "Pop",
        7);

    chordGenreBox.addItem(
        "Highlife",
        8);

    chordGenreBox.addItem(
        "Amapiano",
        9);

    chordGenreBox.addItem(
        "Jazz",
        10);

    chordGenreBox.onChange =
        [this]
        {
            const int selectedId =
                chordGenreBox.getSelectedId();

            if (selectedId > 0)
            {
                audioProcessor.setChordGenre(
                    selectedId - 1);
            }
        };


    // =========================================================
    // CHORD TYPE
    // =========================================================

    addAndMakeVisible(chordTypeLabel);

    chordTypeLabel.setText(
        "Chord Type",
        juce::dontSendNotification);


    addAndMakeVisible(chordTypeBox);

    chordTypeBox.addItem("Major",         1);
    chordTypeBox.addItem("Minor",         2);
    chordTypeBox.addItem("Major 7",       3);
    chordTypeBox.addItem("Minor 7",       4);
    chordTypeBox.addItem("Dominant 7",    5);
    chordTypeBox.addItem("Diminished",    6);
    chordTypeBox.addItem("Augmented",     7);
    chordTypeBox.addItem("Sus2",          8);
    chordTypeBox.addItem("Sus4",          9);
    chordTypeBox.addItem("Major 6",       10);
    chordTypeBox.addItem("Minor 6",       11);
    chordTypeBox.addItem("Major 9",       12);
    chordTypeBox.addItem("Minor 9",       13);
    chordTypeBox.addItem("Dominant 9",    14);
    chordTypeBox.addItem("Major 11",      15);
    chordTypeBox.addItem("Minor 11",      16);
    chordTypeBox.addItem("Dominant 11",   17);
    chordTypeBox.addItem("6/9",           18);

    chordTypeBox.onChange =
        [this]
        {
            const int selectedId =
                chordTypeBox.getSelectedId();

            if (selectedId > 0)
            {
                audioProcessor.setChordType(
                    selectedId - 1);
            }
        };


    // =========================================================
    // CHORD VOICING
    // =========================================================

    addAndMakeVisible(chordVoicingLabel);

    chordVoicingLabel.setText(
        "Voicing",
        juce::dontSendNotification);


    addAndMakeVisible(chordVoicingBox);

    chordVoicingBox.addItem(
        "Close",
        1);

    chordVoicingBox.addItem(
        "Open",
        2);

    chordVoicingBox.addItem(
        "Wide",
        3);

    chordVoicingBox.onChange =
        [this]
        {
            const int selectedId =
                chordVoicingBox.getSelectedId();

            if (selectedId > 0)
            {
                audioProcessor.setChordVoicing(
                    selectedId - 1);
            }
        };


    // =========================================================
    // CHORD OCTAVE
    // =========================================================

    addAndMakeVisible(chordOctaveLabel);

    chordOctaveLabel.setText(
        "Chord Octave",
        juce::dontSendNotification);


    addAndMakeVisible(chordOctaveSlider);

    chordOctaveSlider.setRange(
        -3,
        3,
        1);

    chordOctaveSlider.setNumDecimalPlacesToDisplay(0);

    chordOctaveSlider.setTextValueSuffix(
        " oct");

    chordOctaveSlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    chordOctaveSlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    chordOctaveSlider.onValueChange =
        [this]
        {
            audioProcessor.setChordOctave(
                static_cast<int>(
                    chordOctaveSlider.getValue()));
        };


    // =========================================================
    // OUTPUT
    // =========================================================

    addAndMakeVisible(outputSectionLabel);

    outputSectionLabel.setText(
        "OUTPUT",
        juce::dontSendNotification);

    outputSectionLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(18.0f)
                .withStyle("Bold")));

    outputSectionLabel.setColour(
        juce::Label::textColourId,
        textColour);


    addAndMakeVisible(outputInfoLabel);

    outputInfoLabel.setText(
        "OFFOR Vocal to MIDI sends detected vocal notes "
        "directly to the host MIDI output.",
        juce::dontSendNotification);

    outputInfoLabel.setColour(
        juce::Label::textColourId,
        mutedColour);

    outputInfoLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(14.0f)));

    outputInfoLabel.setJustificationType(
        juce::Justification::topLeft);


        // =========================================================
    // ABOUT / SUPPORT
    // =========================================================

    addAndMakeVisible(aboutSectionLabel);

    aboutSectionLabel.setText(
        "ABOUT OFFOR VOCAL TO MIDI",
        juce::dontSendNotification);

    aboutSectionLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(18.0f)
                .withStyle("Bold")));

    aboutSectionLabel.setColour(
        juce::Label::textColourId,
        textColour);


    // =========================================================
    // VERSION
    // =========================================================

    addAndMakeVisible(versionLabel);

    versionLabel.setText(
        "Version " + juce::String(OFFOR_VPRO_VERSION_STRING),
        juce::dontSendNotification);

    versionLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(12.0f)));

    versionLabel.setColour(
        juce::Label::textColourId,
        mutedColour);


    // =========================================================
    // USER GUIDE
    // =========================================================

    addAndMakeVisible(userGuideButton);

    userGuideButton.setButtonText(
        "USER GUIDE");

    userGuideButton.setColour(
        juce::TextButton::buttonColourId,
        panelColour);

    userGuideButton.setColour(
        juce::TextButton::buttonOnColourId,
        accentColour);

    userGuideButton.setColour(
        juce::TextButton::textColourOffId,
        textColour);

    userGuideButton.setColour(
        juce::TextButton::textColourOnId,
        textColour);

    userGuideButton.onClick =
        []
        {
            juce::URL (
                "https://chechris.com/software"
            ).launchInDefaultBrowser();
        };


    // =========================================================
    // FEEDBACK
    // =========================================================

    addAndMakeVisible(feedbackButton);

    feedbackButton.setButtonText(
        "SEND FEEDBACK");

    feedbackButton.setColour(
        juce::TextButton::buttonColourId,
        panelColour);

    feedbackButton.setColour(
        juce::TextButton::buttonOnColourId,
        accentColour);

    feedbackButton.setColour(
        juce::TextButton::textColourOffId,
        textColour);

    feedbackButton.setColour(
        juce::TextButton::textColourOnId,
        textColour);

    feedbackButton.onClick =
        []
        {
            juce::URL (
                "https://chechris.com/software"
            ).launchInDefaultBrowser();
        };


    // =========================================================
    // PITCH RANGE
    // =========================================================

    addAndMakeVisible(pitchRangeLabel);

    pitchRangeLabel.setText(
        "PITCH RANGE",
        juce::dontSendNotification);

    pitchRangeLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(18.0f)
                .withStyle("Bold")));

    pitchRangeLabel.setColour(
        juce::Label::textColourId,
        textColour);


    // Preset
    addAndMakeVisible(pitchRangePresetLabel);

    pitchRangePresetLabel.setText(
        "Vocal Range",
        juce::dontSendNotification);

    pitchRangePresetLabel.setColour(
        juce::Label::textColourId,
        textColour);


    addAndMakeVisible(pitchRangePresetBox);

    pitchRangePresetBox.addItem(
        "Bass",
        1);

    pitchRangePresetBox.addItem(
        "Low Male",
        2);

    pitchRangePresetBox.addItem(
        "Male",
        3);

    pitchRangePresetBox.addItem(
        "Female",
        4);

    pitchRangePresetBox.addItem(
        "High Female",
        5);

    pitchRangePresetBox.addItem(
        "Custom",
        6);

    pitchRangePresetBox.onChange =
        [this]
        {
            const int selectedPreset =
                pitchRangePresetBox.getSelectedId();

            if (selectedPreset <= 0)
                return;

            const int preset =
                selectedPreset - 1;

            audioProcessor.setPitchRangePreset(
                preset);

            minFrequencySlider.setValue(
                audioProcessor.getMinPitchFrequency(),
                juce::dontSendNotification);

            maxFrequencySlider.setValue(
                audioProcessor.getMaxPitchFrequency(),
                juce::dontSendNotification);

            const bool custom =
                preset == 5;

            minFrequencySlider.setEnabled(custom);
            maxFrequencySlider.setEnabled(custom);
        };


    // Minimum Frequency
    addAndMakeVisible(minFrequencyLabel);

    minFrequencyLabel.setText(
        "Minimum Frequency",
        juce::dontSendNotification);

    minFrequencyLabel.setColour(
        juce::Label::textColourId,
        textColour);


    addAndMakeVisible(minFrequencySlider);

    minFrequencySlider.setRange(
        40.0,
        1000.0,
        1.0);

    minFrequencySlider.setNumDecimalPlacesToDisplay(0);

    minFrequencySlider.setTextValueSuffix(
        " Hz");

    minFrequencySlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    minFrequencySlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    minFrequencySlider.onValueChange =
        [this]
        {
            if (pitchRangePresetBox.getSelectedId() != 6)
                return;

            float minimum =
                static_cast<float>(
                    minFrequencySlider.getValue());

            float maximum =
                static_cast<float>(
                    maxFrequencySlider.getValue());

            if (minimum >= maximum - 20.0f)
            {
                maximum = minimum + 20.0f;

                maxFrequencySlider.setValue(
                    maximum,
                    juce::dontSendNotification);
            }

            audioProcessor.setCustomPitchRange(
                minimum,
                maximum);
        };


    // Maximum Frequency
    addAndMakeVisible(maxFrequencyLabel);

    maxFrequencyLabel.setText(
        "Maximum Frequency",
        juce::dontSendNotification);

    maxFrequencyLabel.setColour(
        juce::Label::textColourId,
        textColour);


    addAndMakeVisible(maxFrequencySlider);

    maxFrequencySlider.setRange(
        60.0,
        2000.0,
        1.0);

    maxFrequencySlider.setNumDecimalPlacesToDisplay(0);

    maxFrequencySlider.setTextValueSuffix(
        " Hz");

    maxFrequencySlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        80,
        28);

    maxFrequencySlider.setColour(
        juce::Slider::thumbColourId,
        accentColour);

    maxFrequencySlider.onValueChange =
        [this]
        {
            if (pitchRangePresetBox.getSelectedId() != 6)
                return;

            float minimum =
                static_cast<float>(
                    minFrequencySlider.getValue());

            float maximum =
                static_cast<float>(
                    maxFrequencySlider.getValue());

            if (maximum <= minimum + 20.0f)
            {
                minimum = maximum - 20.0f;

                minFrequencySlider.setValue(
                    minimum,
                    juce::dontSendNotification);
            }

            audioProcessor.setCustomPitchRange(
                minimum,
                maximum);
        };


    // =========================================================
    // MOVE SETTINGS CONTROLS INTO SCROLL CONTENT
    // =========================================================

    auto moveToContent =
        [this](juce::Component& component)
        {
            component.setVisible(false);

            settingsContent.addAndMakeVisible(
                component);
        };

    moveToContent(pitchSectionLabel);

    moveToContent(confidenceLabel);
    moveToContent(confidenceSlider);

    moveToContent(stabilityLabel);
    moveToContent(stabilitySlider);

    moveToContent(toleranceLabel);
    moveToContent(toleranceSlider);

    moveToContent(noteOffLabel);
    moveToContent(noteOffSlider);

    moveToContent(pitchRangeLabel);
    moveToContent(pitchRangePresetLabel);
    moveToContent(pitchRangePresetBox);
    moveToContent(minFrequencyLabel);
    moveToContent(minFrequencySlider);
    moveToContent(maxFrequencyLabel);
    moveToContent(maxFrequencySlider);

    moveToContent(voiceSectionLabel);

    moveToContent(voiceThresholdLabel);
    moveToContent(voiceThresholdSlider);

    moveToContent(pitchThresholdLabel);
    moveToContent(pitchThresholdSlider);

    moveToContent(midiSectionLabel);

    moveToContent(midiChannelLabel);
    moveToContent(midiChannelSlider);

    moveToContent(velocityModeLabel);
    moveToContent(velocityModeBox);

    moveToContent(fixedVelocityLabel);
    moveToContent(fixedVelocitySlider);

    moveToContent(octaveShiftLabel);
    moveToContent(octaveShiftSlider);

    moveToContent(transposeLabel);
    moveToContent(transposeSlider);

    moveToContent(musicalSectionLabel);

    moveToContent(scaleLockLabel);
    moveToContent(scaleLockButton);

    moveToContent(musicalKeyLabel);
    moveToContent(musicalKeyBox);

    moveToContent(musicalScaleLabel);
    moveToContent(musicalScaleBox);

    moveToContent(chordSectionLabel);

    moveToContent(midiModeLabel);
    moveToContent(midiModeBox);

    moveToContent(chordGenreLabel);
    moveToContent(chordGenreBox);

    moveToContent(chordTypeLabel);
    moveToContent(chordTypeBox);

    moveToContent(chordVoicingLabel);
    moveToContent(chordVoicingBox);

    moveToContent(chordOctaveLabel);
    moveToContent(chordOctaveSlider);

    moveToContent(outputSectionLabel);
    moveToContent(outputInfoLabel);

    moveToContent(aboutSectionLabel);
    moveToContent(versionLabel);
    moveToContent(userGuideButton);
    moveToContent(feedbackButton);


    // =========================================================
    // INITIAL VALUES
    // =========================================================

    loadCurrentValues();

    showTab(0);
}


// =============================================================
// LOAD CURRENT VALUES
// =============================================================

void SettingsPage::loadCurrentValues()
{
    confidenceSlider.setValue(
        audioProcessor.getConfidenceThreshold(),
        juce::dontSendNotification);

    stabilitySlider.setValue(
        audioProcessor.getNoteStability(),
        juce::dontSendNotification);

    toleranceSlider.setValue(
        audioProcessor.getNoteTolerance(),
        juce::dontSendNotification);

    noteOffSlider.setValue(
        audioProcessor.getNoteOffDelay(),
        juce::dontSendNotification);


    voiceThresholdSlider.setValue(
        audioProcessor.getVoiceThreshold(),
        juce::dontSendNotification);

    pitchThresholdSlider.setValue(
        audioProcessor.getPitchThreshold(),
        juce::dontSendNotification);


    // MIDI
    midiChannelSlider.setValue(
        audioProcessor.getMidiChannel(),
        juce::dontSendNotification);

    velocityModeBox.setSelectedId(
        audioProcessor.getVelocityMode() + 1,
        juce::dontSendNotification);

    fixedVelocitySlider.setValue(
        audioProcessor.getFixedVelocity(),
        juce::dontSendNotification);

    octaveShiftSlider.setValue(
        audioProcessor.getOctaveShift(),
        juce::dontSendNotification);

    transposeSlider.setValue(
        audioProcessor.getTranspose(),
        juce::dontSendNotification);


    // Musical
    scaleLockButton.setToggleState(
        audioProcessor.isScaleLockEnabled(),
        juce::dontSendNotification);

    musicalKeyBox.setSelectedId(
        audioProcessor.getMusicalKey() + 1,
        juce::dontSendNotification);

    musicalScaleBox.setSelectedId(
        audioProcessor.getMusicalScale() + 1,
        juce::dontSendNotification);


    // Chord
    midiModeBox.setSelectedId(
        audioProcessor.getMidiMode() + 1,
        juce::dontSendNotification);

    chordGenreBox.setSelectedId(
        audioProcessor.getChordGenre() + 1,
        juce::dontSendNotification);

    chordTypeBox.setSelectedId(
        audioProcessor.getChordType() + 1,
        juce::dontSendNotification);

    chordVoicingBox.setSelectedId(
        audioProcessor.getChordVoicing() + 1,
        juce::dontSendNotification);

    chordOctaveSlider.setValue(
        audioProcessor.getChordOctave(),
        juce::dontSendNotification);


    // Pitch range
    pitchRangePresetBox.setSelectedId(
        audioProcessor.getPitchRangePreset() + 1,
        juce::dontSendNotification);

    minFrequencySlider.setValue(
        audioProcessor.getMinPitchFrequency(),
        juce::dontSendNotification);

    maxFrequencySlider.setValue(
        audioProcessor.getMaxPitchFrequency(),
        juce::dontSendNotification);

    const bool custom =
        audioProcessor.getPitchRangePreset() == 5;

    minFrequencySlider.setEnabled(custom);
    maxFrequencySlider.setEnabled(custom);
}


// =============================================================
// SHOW TAB
// =============================================================

void SettingsPage::showTab(int tabIndex)
{
    currentTab =
        juce::jlimit(
            0,
            4,
            tabIndex);


    // =========================================================
    // PITCH
    // =========================================================

    const bool showPitch =
        currentTab == 0;

    pitchSectionLabel.setVisible(showPitch);

    confidenceLabel.setVisible(showPitch);
    confidenceSlider.setVisible(showPitch);

    stabilityLabel.setVisible(showPitch);
    stabilitySlider.setVisible(showPitch);

    toleranceLabel.setVisible(showPitch);
    toleranceSlider.setVisible(showPitch);

    noteOffLabel.setVisible(showPitch);
    noteOffSlider.setVisible(showPitch);

    pitchRangeLabel.setVisible(showPitch);
    pitchRangePresetLabel.setVisible(showPitch);
    pitchRangePresetBox.setVisible(showPitch);
    minFrequencyLabel.setVisible(showPitch);
    minFrequencySlider.setVisible(showPitch);
    maxFrequencyLabel.setVisible(showPitch);
    maxFrequencySlider.setVisible(showPitch);


    // =========================================================
    // VOICE
    // =========================================================

    const bool showVoice =
        currentTab == 1;

    voiceSectionLabel.setVisible(showVoice);

    voiceThresholdLabel.setVisible(showVoice);
    voiceThresholdSlider.setVisible(showVoice);

    pitchThresholdLabel.setVisible(showVoice);
    pitchThresholdSlider.setVisible(showVoice);


    // =========================================================
    // MIDI
    // =========================================================

    const bool showMidi =
        currentTab == 2;

    midiSectionLabel.setVisible(showMidi);

    midiChannelLabel.setVisible(showMidi);
    midiChannelSlider.setVisible(showMidi);

    velocityModeLabel.setVisible(showMidi);
    velocityModeBox.setVisible(showMidi);

    fixedVelocityLabel.setVisible(showMidi);
    fixedVelocitySlider.setVisible(showMidi);

    octaveShiftLabel.setVisible(showMidi);
    octaveShiftSlider.setVisible(showMidi);

    transposeLabel.setVisible(showMidi);
    transposeSlider.setVisible(showMidi);


    // =========================================================
    // MUSICAL
    // =========================================================

    const bool showMusical =
        currentTab == 3;

    musicalSectionLabel.setVisible(showMusical);

    scaleLockLabel.setVisible(showMusical);
    scaleLockButton.setVisible(showMusical);

    musicalKeyLabel.setVisible(showMusical);
    musicalKeyBox.setVisible(showMusical);

    musicalScaleLabel.setVisible(showMusical);
    musicalScaleBox.setVisible(showMusical);

    chordSectionLabel.setVisible(showMusical);

    midiModeLabel.setVisible(showMusical);
    midiModeBox.setVisible(showMusical);

    chordGenreLabel.setVisible(showMusical);
    chordGenreBox.setVisible(showMusical);

    chordTypeLabel.setVisible(showMusical);
    chordTypeBox.setVisible(showMusical);

    chordVoicingLabel.setVisible(showMusical);
    chordVoicingBox.setVisible(showMusical);

    chordOctaveLabel.setVisible(showMusical);
    chordOctaveSlider.setVisible(showMusical);


    // =========================================================
    // OUTPUT
    // =========================================================

    const bool showOutput =
        currentTab == 4;

    outputSectionLabel.setVisible(showOutput);
    outputInfoLabel.setVisible(showOutput);

    aboutSectionLabel.setVisible(showOutput);
    versionLabel.setVisible(showOutput);
    userGuideButton.setVisible(showOutput);
    feedbackButton.setVisible(showOutput);


    updateTabButtons();

    resized();

    settingsViewport.setViewPosition(
        0,
        0);
}


// =============================================================
// UPDATE TAB BUTTONS
// =============================================================

void SettingsPage::updateTabButtons()
{
    const juce::Colour active =
        accentColour;

    const juce::Colour inactive =
        panelColour;


    pitchTabButton.setColour(
        juce::TextButton::buttonColourId,
        currentTab == 0 ? active : inactive);

    voiceTabButton.setColour(
        juce::TextButton::buttonColourId,
        currentTab == 1 ? active : inactive);

    midiTabButton.setColour(
        juce::TextButton::buttonColourId,
        currentTab == 2 ? active : inactive);

    musicalTabButton.setColour(
        juce::TextButton::buttonColourId,
        currentTab == 3 ? active : inactive);

    outputTabButton.setColour(
        juce::TextButton::buttonColourId,
        currentTab == 4 ? active : inactive);


    pitchTabButton.setColour(
        juce::TextButton::textColourOffId,
        textColour);

    voiceTabButton.setColour(
        juce::TextButton::textColourOffId,
        textColour);

    midiTabButton.setColour(
        juce::TextButton::textColourOffId,
        textColour);

    musicalTabButton.setColour(
        juce::TextButton::textColourOffId,
        textColour);

    outputTabButton.setColour(
        juce::TextButton::textColourOffId,
        textColour);
}


// =============================================================
// PAINT
// =============================================================

void SettingsPage::paint(
    juce::Graphics& g)
{
    g.fillAll(backgroundColour);


    // Main content panel
    g.setColour(panelColour);

    g.fillRoundedRectangle(
        16.0f,
        94.0f,
        static_cast<float>(getWidth() - 32),
        static_cast<float>(getHeight() - 110),
        8.0f);


    // Tab separator
    g.setColour(
        juce::Colour(0xff30323a));

    g.fillRect(
        16,
        88,
        getWidth() - 32,
        1);


    // Active tab underline
    juce::Rectangle<int> activeBounds;

    switch (currentTab)
    {
        case 0:
            activeBounds =
                pitchTabButton.getBounds();
            break;

        case 1:
            activeBounds =
                voiceTabButton.getBounds();
            break;

        case 2:
            activeBounds =
                midiTabButton.getBounds();
            break;

        case 3:
            activeBounds =
                musicalTabButton.getBounds();
            break;

        case 4:
            activeBounds =
                outputTabButton.getBounds();
            break;

        default:
            break;
    }


    if (!activeBounds.isEmpty())
    {
        g.setColour(accentColour);

        g.fillRect(
            activeBounds.getX(),
            activeBounds.getBottom() - 2,
            activeBounds.getWidth(),
            2);
    }
}


// =============================================================
// RESIZED
// =============================================================

void SettingsPage::resized()
{
    const int width =
        getWidth();

    const int height =
        getHeight();

    // =========================================================
    // SETTINGS VIEWPORT
    // =========================================================

    settingsViewport.setBounds(
        16,
        94,
        juce::jmax(1, width - 32),
        juce::jmax(1, height - 110));

    const int contentWidth =
        juce::jmax(
            1,
            settingsViewport.getWidth());

    settingsContent.setSize(
        contentWidth,
        700);


    // =========================================================
    // HEADER
    // =========================================================

    titleLabel.setBounds(
        24,
        14,
        180,
        30);

    backButton.setBounds(
        width - 90,
        14,
        66,
        30);


    // =========================================================
    // TOP TABS
    // =========================================================

    const int tabY = 52;
    const int tabHeight = 36;

    const int leftMargin = 20;
    const int tabGap = 4;

    const int availableWidth =
        width - (leftMargin * 2);

    const int tabWidth =
        juce::jmax(
            70,
            (availableWidth - (tabGap * 4)) / 5);


    pitchTabButton.setBounds(
        leftMargin,
        tabY,
        tabWidth,
        tabHeight);

    voiceTabButton.setBounds(
        leftMargin + (tabWidth + tabGap),
        tabY,
        tabWidth,
        tabHeight);

    midiTabButton.setBounds(
        leftMargin + 2 * (tabWidth + tabGap),
        tabY,
        tabWidth,
        tabHeight);

    musicalTabButton.setBounds(
        leftMargin + 3 * (tabWidth + tabGap),
        tabY,
        tabWidth,
        tabHeight);

    outputTabButton.setBounds(
        leftMargin + 4 * (tabWidth + tabGap),
        tabY,
        tabWidth,
        tabHeight);


    // =========================================================
    // COMMON CONTENT
    // =========================================================

    const int labelX = 42;
    const int controlX = 300;

    const int controlWidth =
        juce::jmax(
            150,
            contentWidth - controlX - 42);

    const int rowHeight = 42;


    // =========================================================
    // PITCH
    // =========================================================

    int pitchY = 112;

    pitchSectionLabel.setBounds(
        labelX,
        pitchY,
        300,
        28);

    pitchY += 38;


    confidenceLabel.setBounds(
        labelX,
        pitchY,
        230,
        28);

    confidenceSlider.setBounds(
        controlX,
        pitchY,
        controlWidth,
        28);

    pitchY += 34;


    stabilityLabel.setBounds(
        labelX,
        pitchY,
        230,
        28);

    stabilitySlider.setBounds(
        controlX,
        pitchY,
        controlWidth,
        28);

    pitchY += 34;


    toleranceLabel.setBounds(
        labelX,
        pitchY,
        230,
        28);

    toleranceSlider.setBounds(
        controlX,
        pitchY,
        controlWidth,
        28);

    pitchY += 34;


    noteOffLabel.setBounds(
        labelX,
        pitchY,
        230,
        28);

    noteOffSlider.setBounds(
        controlX,
        pitchY,
        controlWidth,
        28);

    pitchY += 42;


    pitchRangeLabel.setBounds(
        labelX,
        pitchY,
        300,
        28);

    pitchY += 34;


    pitchRangePresetLabel.setBounds(
        labelX,
        pitchY,
        230,
        28);

    pitchRangePresetBox.setBounds(
        controlX,
        pitchY,
        controlWidth,
        28);

    pitchY += 34;


    minFrequencyLabel.setBounds(
        labelX,
        pitchY,
        230,
        28);

    minFrequencySlider.setBounds(
        controlX,
        pitchY,
        controlWidth,
        28);

    pitchY += 34;


    maxFrequencyLabel.setBounds(
        labelX,
        pitchY,
        230,
        28);

    maxFrequencySlider.setBounds(
        controlX,
        pitchY,
        controlWidth,
        28);


    // =========================================================
    // VOICE
    // =========================================================

    int voiceY = 120;

    voiceSectionLabel.setBounds(
        labelX,
        voiceY,
        300,
        28);

    voiceY += 50;


    voiceThresholdLabel.setBounds(
        labelX,
        voiceY,
        230,
        28);

    voiceThresholdSlider.setBounds(
        controlX,
        voiceY,
        controlWidth,
        28);

    voiceY += rowHeight;


    pitchThresholdLabel.setBounds(
        labelX,
        voiceY,
        230,
        28);

    pitchThresholdSlider.setBounds(
        controlX,
        voiceY,
        controlWidth,
        28);


    // =========================================================
    // MIDI
    // =========================================================

    int midiY = 120;

    midiSectionLabel.setBounds(
        labelX,
        midiY,
        300,
        28);

    midiY += 50;


    midiChannelLabel.setBounds(
        labelX,
        midiY,
        230,
        28);

    midiChannelSlider.setBounds(
        controlX,
        midiY,
        controlWidth,
        28);

    midiY += rowHeight;


    velocityModeLabel.setBounds(
        labelX,
        midiY,
        230,
        28);

    velocityModeBox.setBounds(
        controlX,
        midiY,
        controlWidth,
        28);

    midiY += rowHeight;


    fixedVelocityLabel.setBounds(
        labelX,
        midiY,
        230,
        28);

    fixedVelocitySlider.setBounds(
        controlX,
        midiY,
        controlWidth,
        28);

    midiY += rowHeight;


    octaveShiftLabel.setBounds(
        labelX,
        midiY,
        230,
        28);

    octaveShiftSlider.setBounds(
        controlX,
        midiY,
        controlWidth,
        28);

    midiY += rowHeight;


    transposeLabel.setBounds(
        labelX,
        midiY,
        230,
        28);

    transposeSlider.setBounds(
        controlX,
        midiY,
        controlWidth,
        28);


    // =========================================================
    // MUSICAL
    // =========================================================

    int musicalY = 112;

    musicalSectionLabel.setBounds(
        labelX,
        musicalY,
        300,
        28);

    musicalY += 38;


    // Scale Lock
    scaleLockLabel.setBounds(
        labelX,
        musicalY,
        230,
        28);

    scaleLockButton.setBounds(
        controlX,
        musicalY,
        controlWidth,
        28);

    musicalY += rowHeight;


    // Key
    musicalKeyLabel.setBounds(
        labelX,
        musicalY,
        230,
        28);

    musicalKeyBox.setBounds(
        controlX,
        musicalY,
        controlWidth,
        28);

    musicalY += rowHeight;


    // Scale
    musicalScaleLabel.setBounds(
        labelX,
        musicalY,
        230,
        28);

    musicalScaleBox.setBounds(
        controlX,
        musicalY,
        controlWidth,
        28);

    musicalY += 50;


    // Chord section
    chordSectionLabel.setBounds(
        labelX,
        musicalY,
        300,
        28);

    musicalY += 38;


    // MIDI Mode
    midiModeLabel.setBounds(
        labelX,
        musicalY,
        230,
        28);

    midiModeBox.setBounds(
        controlX,
        musicalY,
        controlWidth,
        28);

    musicalY += rowHeight;


    // Genre
    chordGenreLabel.setBounds(
        labelX,
        musicalY,
        230,
        28);

    chordGenreBox.setBounds(
        controlX,
        musicalY,
        controlWidth,
        28);

    musicalY += rowHeight;


    // Chord Type
    chordTypeLabel.setBounds(
        labelX,
        musicalY,
        230,
        28);

    chordTypeBox.setBounds(
        controlX,
        musicalY,
        controlWidth,
        28);

    musicalY += rowHeight;


    // Voicing
    chordVoicingLabel.setBounds(
        labelX,
        musicalY,
        230,
        28);

    chordVoicingBox.setBounds(
        controlX,
        musicalY,
        controlWidth,
        28);

    musicalY += rowHeight;


    // Chord Octave
    chordOctaveLabel.setBounds(
        labelX,
        musicalY,
        230,
        28);

    chordOctaveSlider.setBounds(
        controlX,
        musicalY,
        controlWidth,
        28);


        // =========================================================
    // OUTPUT
    // =========================================================

    outputSectionLabel.setBounds(
        labelX,
        120,
        300,
        28);

    outputInfoLabel.setBounds(
        labelX,
        170,
        contentWidth - 84,
        80);


    // =========================================================
    // ABOUT / SUPPORT
    // =========================================================

    aboutSectionLabel.setBounds(
        labelX,
        275,
        320,
        28);

    versionLabel.setBounds(
        labelX,
        310,
        200,
        24);

    userGuideButton.setBounds(
        labelX,
        350,
        150,
        34);

    feedbackButton.setBounds(
        labelX + 165,
        350,
        170,
        34);
}