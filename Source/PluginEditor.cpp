#include "PluginEditor.h"
#include "BinaryData.h"
#include "Version.h"
#include <iostream>

#if JUCE_WINDOWS
#include <windows.h>
#endif

namespace
{
    const juce::Colour backgroundColour (0xff111216);
    const juce::Colour panelColour      (0xff1d1f25);
    const juce::Colour panelDarkColour  (0xff17181d);
    const juce::Colour textColour       (0xfff2f2f4);
    const juce::Colour mutedColour      (0xff858791);
    const juce::Colour accentColour     (0xffdf513e);
    const juce::Colour accentDarkColour (0xffa93629);
}

// ============================================================
// CONSTRUCTOR
// ============================================================

OfforVocalToMidiAudioProcessorEditor::
OfforVocalToMidiAudioProcessorEditor(
    OfforVocalToMidiAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      settingsPage (p)
{

    
    setSize (700, 500);

    // ========================================================
    // LOGO
    // ========================================================

    {
        offorLogo = juce::ImageFileFormat::loadFrom (
            BinaryData::OfforVocalToMidi_png,
            BinaryData::OfforVocalToMidi_pngSize);
    }

    // ========================================================
    // SETTINGS BUTTON
    // ========================================================

    {
        const auto settingsImage =
            juce::ImageFileFormat::loadFrom (
                BinaryData::setting_png,
                BinaryData::setting_pngSize);

        settingsButton.setImages (
            false,
            true,
            true,
            settingsImage,
            1.0f,
            juce::Colours::transparentBlack,
            settingsImage,
            0.82f,
            juce::Colours::transparentBlack,
            settingsImage,
            0.65f,
            juce::Colours::transparentBlack);

        settingsButton.setTooltip ("Settings");

        settingsButton.onClick =
            [this]
            {
                showSettingsPage();
            };

        addAndMakeVisible (settingsButton);


        // ========================================================
        // UPDATE BUTTON
        // ========================================================

        updateButton.setButtonText("UPDATE");

        updateButton.setColour(
            juce::TextButton::buttonColourId,
            accentColour);

        updateButton.setColour(
            juce::TextButton::textColourOffId,
            juce::Colours::white);

        updateButton.setColour(
            juce::TextButton::buttonOnColourId,
            juce::Colour(0xffe89e81));

        updateButton.setTooltip(
            "Check for a newer version of OFFOR Vocal To MIDI");

        updateButton.onClick =
        [this]                          
        {
            
            showUpdateDialog();
        };  

        updateButton.setVisible(false);

        addAndMakeVisible(updateButton);
    }

    
    // ========================================================
    // TITLE
    // ========================================================

    titleLabel.setText (
        "OFFOR VOCAL TO MIDI",
        juce::dontSendNotification);

    titleLabel.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (15.0f)
                .withStyle ("Bold")));

    titleLabel.setColour (
        juce::Label::textColourId,
        textColour);

    addAndMakeVisible (titleLabel);

    // ========================================================
    // STATUS
    // ========================================================

    statusLabel.setText (
        "WAITING FOR VOICE",
        juce::dontSendNotification);

    statusLabel.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (11.0f)
                .withStyle ("Bold")));

    statusLabel.setColour (
        juce::Label::textColourId,
        mutedColour);

    addAndMakeVisible (statusLabel);

    addAndMakeVisible (licenseOverlay);

    licenseOverlay.onActivate =
        [this](const juce::String& licenseKey)
    {
        licenseOverlay.setStatusMessage(
            "Activating license...");

        const bool activated =
            audioProcessor.activateLicense(licenseKey);

        if (activated)
        {
            licenseOverlay.setStatusMessage(
                "License activated.");

            licenseOverlay.setVisible(false);
        }
        else
        {
            licenseOverlay.setStatusMessage(
                "Invalid or rejected license key.");
        }
    };

    licenseOverlay.onGetLicense =
        [this]
    {
        juce::URL(
            "https://ko-fi.com/s/9228915b7e")
            .launchInDefaultBrowser();
    };

    
    // ========================================================
    // NOTE
    // ========================================================

    noteLabel.setText (
        "C4",
        juce::dontSendNotification);

    noteLabel.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (58.0f)
                .withStyle ("Bold")));

    noteLabel.setColour (
        juce::Label::textColourId,
        textColour);

    noteLabel.setJustificationType (
        juce::Justification::centred);

    addAndMakeVisible (noteLabel);

    // ========================================================
    // FREQUENCY
    // ========================================================

    frequencyLabel.setText (
        "FREQUENCY  00 Hz",
        juce::dontSendNotification);

    frequencyLabel.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (11.0f)));

    frequencyLabel.setColour (
        juce::Label::textColourId,
        mutedColour);

    frequencyLabel.setJustificationType (
        juce::Justification::centred);

    addAndMakeVisible (frequencyLabel);

    // ========================================================
    // CONFIDENCE
    // ========================================================

    confidenceLabel.setText (
        "CONFIDENCE",
        juce::dontSendNotification);

    confidenceLabel.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (10.0f)
                .withStyle ("Bold")));

    confidenceLabel.setColour (
        juce::Label::textColourId,
        mutedColour);

    confidenceLabel.setJustificationType (
        juce::Justification::centred);

    addAndMakeVisible (confidenceLabel);

    // ========================================================
    // MIDI
    // ========================================================

    midiLabel.setText (
        "MIDI  00",
        juce::dontSendNotification);

    midiLabel.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (11.0f)
                .withStyle ("Bold")));

    midiLabel.setColour (
        juce::Label::textColourId,
        mutedColour);

    midiLabel.setJustificationType (
        juce::Justification::centred);

    addAndMakeVisible (midiLabel);

    // ========================================================
    // PIANO
    // ========================================================

    addAndMakeVisible (pianoKeyboard);
    addAndMakeVisible (noteDetection);

    // ========================================================
    // SETTINGS PAGE
    // ========================================================

    settingsPage.setVisible (false);

    settingsPage.onBack =
        [this]
        {
            showMainPage();
        };

    addAndMakeVisible (settingsPage);

    // ========================================================
    // INITIAL STATE
    // ========================================================

    showMainPage();

    startTimerHz (30);

    if (audioProcessor.isLicenseActivated())
    {
        licenseOverlay.setVisible(false);
    }
    else if (audioProcessor.getLicenseFreeUsesRemaining() > 0)
    {
        // User still has free trial uses.
        licenseOverlay.setVisible(false);
    }
    else
    {
        // Trial exhausted.
        licenseOverlay.setTrialRemaining(0);
        licenseOverlay.setTrialExpired(true);

        licenseOverlay.setVisible(true);
        licenseOverlay.toFront(false);
    }

    checkForUpdates();

}


void OfforVocalToMidiAudioProcessorEditor::checkForUpdates()
{
    
    updateChecker.checkForUpdate(
        OFFOR_VOCAL_TO_MIDI_VERSION_STRING,

        [this](const UpdateChecker::UpdateInfo& info)
        {
            
            if (!info.success)
            {
                
                return;
            }

            // ==================================================
            // STORE UPDATE INFORMATION
            // ==================================================

            latestVersion =
                info.latestVersion;

            updateDownloadUrl =
                info.downloadUrl;

            updateAvailable =
                info.updateAvailable;

            updateRequired =
                info.minimumVersionRequired;

            
            // ==================================================
            // SHOW UPDATE BUTTON
            // ==================================================

            if (updateAvailable || updateRequired)
            {
                updateButton.setButtonText(
                    updateRequired
                        ? "UPDATE REQUIRED"
                        : "UPDATE AVAILABLE");

                updateButton.setVisible(true);

                updateButton.setTooltip(
                    updateRequired
                        ? "A required OFFOR Vocal To MIDI update is available"
                        : "A new OFFOR Vocal To MIDI update is available");

                resized();
                repaint();

                
            }
            else
            {
                updateButton.setVisible(false);

                resized();
                repaint();

                
            }
        });
}


void OfforVocalToMidiAudioProcessorEditor::showUpdateDialog()
{
    
    if (updateDownloadUrl.isEmpty())
    {
        
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "OFFOR Vocal To MIDI",
            "The update information was received, but the download URL is missing.");

        return;
    }

    if (!juce::URL::isProbablyAWebsiteURL(updateDownloadUrl))
    {
        
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "OFFOR Vocal To MIDI",
            "The update URL is invalid:\n\n"
            + updateDownloadUrl);

        return;
    }

    const auto message =
        "OFFOR Vocal To MIDI "
        + juce::String(OFFOR_VOCAL_TO_MIDI_VERSION_STRING)
        + " is installed.\n\n"
        + "Version "
        + latestVersion
        + " is available."
        + (updateRequired
            ? "\n\nThis update is required."
            : "");

    juce::AlertWindow::showOkCancelBox(
        juce::AlertWindow::InfoIcon,
        "OFFOR Vocal To MIDI Update",
        message,
        "DOWNLOAD UPDATE",
        "CANCEL",
        this,
        juce::ModalCallbackFunction::create(
            [this](int result)
            {
                
                if (result != 1)
                {
                    
                    return;
                }

                
                const bool launched =
                    juce::URL(updateDownloadUrl)
                        .launchInDefaultBrowser();

                
                if (!launched)
                {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "OFFOR Vocal To MIDI",
                        "Windows could not open the update page.\n\n"
                        + updateDownloadUrl);
                }
            }));
}


// ============================================================
// DESTRUCTOR
// ============================================================

OfforVocalToMidiAudioProcessorEditor::
~OfforVocalToMidiAudioProcessorEditor()
{
    stopTimer();
}

// ============================================================
// MAIN PAGE
// ============================================================

void OfforVocalToMidiAudioProcessorEditor::showMainPage()
{
    showingSettings = false;

    // Header
    titleLabel.setVisible (true);
    statusLabel.setVisible (true);
    settingsButton.setVisible (true);

    // These are now drawn manually by paint().
    // Keep the components for internal state/timer compatibility,
    // but do not display them.
    noteLabel.setVisible (false);
    frequencyLabel.setVisible (false);
    confidenceLabel.setVisible (false);
    midiLabel.setVisible (false);

    pianoKeyboard.setVisible (true);
    noteDetection.setVisible(true);

    settingsPage.setVisible (false);

    resized();
    repaint();
}

// ============================================================
// SETTINGS PAGE
// ============================================================

void OfforVocalToMidiAudioProcessorEditor::showSettingsPage()
{
    showingSettings = true;

    titleLabel.setVisible(false);
    statusLabel.setVisible(false);
    settingsButton.setVisible(false);

    noteLabel.setVisible(false);
    frequencyLabel.setVisible(false);
    confidenceLabel.setVisible(false);
    midiLabel.setVisible(false);

    pianoKeyboard.setVisible(false);
    noteDetection.setVisible(false);

    settingsPage.setVisible(true);

    resized();
    repaint();
}

// ============================================================
// TEMPORARY FL STUDIO KEYBOARD TEST
// ============================================================

void OfforVocalToMidiAudioProcessorEditor::sendTestC4()
{
#if JUCE_WINDOWS

    INPUT input {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = 'A';

    SendInput (
        1,
        &input,
        sizeof (INPUT));

    input.ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput (
        1,
        &input,
        sizeof (INPUT));

#endif
}

// ============================================================
// PAINT
// ============================================================

void OfforVocalToMidiAudioProcessorEditor::paint (
    juce::Graphics& g)
{
    g.fillAll (backgroundColour);

    if (showingSettings)
        return;

    // ========================================================
    // HEADER
    // ========================================================


    g.setColour (panelDarkColour);
    g.fillRect (
        0,
        0,
        getWidth(),
        70);

    // subtle bottom line

    g.setColour (juce::Colour (0xff292b31));
    g.fillRect (
        0,
        69,
        getWidth(),
        1);

    // ========================================================
    // LOGO
    // ========================================================

    if (!offorLogo.isNull())
    {
        g.drawImageWithin (
            offorLogo,
            18,
            11,
            46,
            46,
            juce::RectanglePlacement::centred,
            false);
    }

    // ========================================================
    // MAIN DETECTION PANEL
    // ========================================================

    auto detectionPanel =
        juce::Rectangle<int> (
            20,
            88,
            getWidth() - 40,
            235);

    g.setColour (panelColour);

    g.fillRoundedRectangle (
        detectionPanel.toFloat(),
        12.0f);

    // ========================================================
    // PANEL TOP LABEL
    // ========================================================

    g.setColour (mutedColour);

    g.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (10.0f)
                .withStyle ("Bold")));

    g.drawText (
        "VOCAL PITCH",
        detectionPanel.getX() + 18,
        detectionPanel.getY() + 14,
        150,
        20,
        juce::Justification::left);

    // ========================================================
    // LIVE PITCH TRACK
    // ========================================================

    drawPitchDisplay (g);

    // ========================================================
    // CONFIDENCE
    // ========================================================

    drawConfidenceMeter (g);

    // ========================================================
    // PIANO PANEL
    // ========================================================

    auto pianoPanel =
        juce::Rectangle<int> (
            20,
            335,
            getWidth() - 40,
            145);

    g.setColour (panelColour);

    g.fillRoundedRectangle (
        pianoPanel.toFloat(),
        12.0f);

    // Piano title

    g.setColour (mutedColour);

    g.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (10.0f)
                .withStyle ("Bold")));

    g.drawText (
        "PITCH MAP",
        pianoPanel.getX() + 15,
        pianoPanel.getY() + 9,
        120,
        20,
        juce::Justification::left);
}

// ============================================================
// LIVE PITCH DISPLAY
// ============================================================

void OfforVocalToMidiAudioProcessorEditor::
drawPitchDisplay (
    juce::Graphics& g)
{
    const int left = 42;
    const int right = getWidth() - 42;

    const int centerY = 196;

    const int trackWidth = right - left;

    // ========================================================
    // NOTE RANGE
    //
    // Show the current note plus neighboring semitones.
    // ========================================================

    int centerNote = displayedMidiNote;

    if (centerNote < 0)
        centerNote = 60;

    const int firstNote = centerNote - 4;
    const int lastNote  = centerNote + 4;

    // ========================================================
    // PITCH TRACK
    // ========================================================

    g.setColour (juce::Colour (0xff292b31));

    g.fillRoundedRectangle (
        juce::Rectangle<float> (
            static_cast<float> (left),
            static_cast<float> (centerY - 1),
            static_cast<float> (trackWidth),
            2.0f),
        1.0f);

    // ========================================================
    // NOTE POSITIONS
    // ========================================================

    for (int note = firstNote;
         note <= lastNote;
         ++note)
    {
        const float normalized =
            static_cast<float> (note - firstNote)
            / static_cast<float> (lastNote - firstNote);

        const float x =
            static_cast<float> (left)
            + normalized * static_cast<float> (trackWidth);

        const bool isCurrent =
            displayedVoiced
            && displayedMidiNote == note;

        // vertical marker

        g.setColour (
            isCurrent
                ? accentColour
                : juce::Colour (0xff30323a));

        g.fillRoundedRectangle (
            juce::Rectangle<float> (
                x - 1.0f,
                static_cast<float> (centerY - 9),
                2.0f,
                18.0f),
            1.0f);

        // note name

        const char* names[] =
        {
            "C", "C#", "D", "D#", "E", "F",
            "F#", "G", "G#", "A", "A#", "B"
        };

        const int pitchClass =
            ((note % 12) + 12) % 12;

        const int octave =
            (note / 12) - 1;

        juce::String noteName =
            juce::String (names[pitchClass])
            + juce::String (octave);

        g.setColour (
            isCurrent
                ? textColour
                : mutedColour);

        g.setFont (
            juce::Font (
                juce::FontOptions()
                    .withHeight (
                        isCurrent ? 12.0f : 10.0f)
                    .withStyle (
                        isCurrent ? "Bold" : "Regular")));

        g.drawText (
            noteName,
            static_cast<int> (x - 30),
            centerY + 17,
            60,
            18,
            juce::Justification::centred);
    }

    // ========================================================
    // CURRENT PITCH INDICATOR
    // ========================================================

    if (displayedVoiced && displayedMidiNote >= 0)
    {
        const float currentNormalized =
            static_cast<float> (
                displayedMidiNote - firstNote)
            / static_cast<float> (
                lastNote - firstNote);

        const float currentX =
            static_cast<float> (left)
            + currentNormalized
                * static_cast<float> (trackWidth);

        // glow

        g.setColour (
            accentColour.withAlpha (0.10f));

        g.fillEllipse (
            currentX - 17.0f,
            centerY - 17.0f,
            34.0f,
            34.0f);

        // outer ring

        g.setColour (
            accentColour.withAlpha (0.30f));

        g.drawEllipse (
            currentX - 10.0f,
            centerY - 10.0f,
            20.0f,
            20.0f,
            2.0f);

        // center

        g.setColour (accentColour);

        g.fillEllipse (
            currentX - 5.0f,
            centerY - 5.0f,
            10.0f,
            10.0f);
    }

    // ========================================================
    // HERO NOTE
    // ========================================================

    g.setColour (
        displayedVoiced
            ? textColour
            : mutedColour);

    g.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (48.0f)
                .withStyle ("Bold")));

    g.drawText (
        displayedVoiced
            ? noteLabel.getText()
            : "--",
        0,
        112,
        getWidth(),
        55,
        juce::Justification::centred);

    // ========================================================
    // FREQUENCY
    // ========================================================

    g.setColour (
        displayedVoiced
            ? mutedColour
            : juce::Colour (0xff62646c));

    g.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (11.0f)));

    g.drawText (
        displayedVoiced
            ? juce::String (displayedFrequency, 1)
                + " Hz"
            : "-- Hz",
        0,
        157,
        getWidth(),
        20,
        juce::Justification::centred);

    // ========================================================
    // MIDI
    // ========================================================

    g.setColour (
        displayedVoiced
            ? accentColour
            : mutedColour);

    g.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (10.0f)
                .withStyle ("Bold")));

    g.drawText (
        displayedVoiced
            ? "MIDI " + juce::String (displayedMidiNote)
            : "MIDI --",
        0,
        220,
        getWidth(),
        18,
        juce::Justification::centred);
}

// ============================================================
// CONFIDENCE METER
// ============================================================

void OfforVocalToMidiAudioProcessorEditor::
drawConfidenceMeter (
    juce::Graphics& g)
{
    const int x = 155;
    const int y = 264;
    const int width = getWidth() - 310;
    const int height = 5;

    // ========================================================
    // LABEL
    // ========================================================

    g.setColour (mutedColour);

    g.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (9.0f)
                .withStyle ("Bold")));

    g.drawText (
        "SIGNAL",
        x - 55,
        y - 7,
        45,
        18,
        juce::Justification::right);

    // ========================================================
    // BACKGROUND
    // ========================================================

    g.setColour (
        juce::Colour (0xff30323a));

    g.fillRoundedRectangle (
        juce::Rectangle<float> (
            static_cast<float> (x),
            static_cast<float> (y),
            static_cast<float> (width),
            static_cast<float> (height)),
        3.0f);

    // ========================================================
    // ACTIVE LEVEL
    // ========================================================

    const float confidence =
        juce::jlimit (
            0.0f,
            1.0f,
            displayedConfidence);

    const float activeWidth =
        static_cast<float> (width) * confidence;

    if (activeWidth > 0.0f)
    {
        g.setColour (
            displayedVoiced
                ? accentColour
                : accentDarkColour);

        g.fillRoundedRectangle (
            juce::Rectangle<float> (
                static_cast<float> (x),
                static_cast<float> (y),
                activeWidth,
                static_cast<float> (height)),
            3.0f);
    }

    // ========================================================
    // CONFIDENCE TEXT
    // ========================================================

    g.setColour (mutedColour);

    g.setFont (
        juce::Font (
            juce::FontOptions()
                .withHeight (9.0f)));

    g.drawText (
        displayedVoiced
            ? juce::String (
                juce::roundToInt (
                    confidence * 100.0f))
                + "%"
            : "--",
        x + width + 10,
        y - 7,
        35,
        18,
        juce::Justification::left);
}

// ============================================================
// RESIZED
// ============================================================

void OfforVocalToMidiAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // =========================================================
    // HEADER
    // =========================================================

    auto header = bounds.removeFromTop (70);

    // ---------------------------------------------------------
    // HEADER TITLE
    // ---------------------------------------------------------

    titleLabel.setBounds (
        78,
        12,
        300,
        24);

    // ---------------------------------------------------------
    // HEADER STATUS
    // ---------------------------------------------------------

    statusLabel.setBounds (
        78,
        36,
        220,
        18);

    // ---------------------------------------------------------
    // SETTINGS BUTTON
    // ---------------------------------------------------------

    
    // =========================================================
    // UPDATE BUTTON
    // =========================================================

    updateButton.setBounds(
        getWidth() - 175,
        20,
        115,
        30);

    // =========================================================
    // SETTINGS BUTTON
    // =========================================================

    settingsButton.setBounds(
        getWidth() - 54,
        18,
        36,
        36);

    // =========================================================
    // SETTINGS PAGE
    // =========================================================

    settingsPage.setBounds (bounds);

    // =========================================================
    // MAIN PAGE
    // =========================================================

    auto mainArea = bounds.reduced (20);

    if (showingSettings)
        return;

    // =========================================================
    // PIANO AREA
    // =========================================================

    auto pianoArea =
        mainArea.removeFromBottom (120);

    pianoKeyboard.setBounds (
        pianoArea.reduced (4, 8));

    // =========================================================
    // TOP CONTENT
    // =========================================================

    auto contentArea = mainArea;

    // Left = Vocal Pitch
    // Right = Note Detection

    auto leftArea =
        contentArea.removeFromLeft (
            contentArea.getWidth() * 0.62f);

    auto rightArea = contentArea;

    // =========================================================
    // VOCAL PITCH
    // =========================================================

    // Vocal Pitch is drawn manually in paint().
    // No component bounds are required here.

    // =========================================================
    // NOTE DETECTION
    // =========================================================

    noteDetection.setBounds (
        rightArea.reduced (8, 0));

    licenseOverlay.setBounds(
        getLocalBounds());

    licenseOverlay.toFront(false);
}

// ============================================================
// TIMER
// ============================================================

void OfforVocalToMidiAudioProcessorEditor::timerCallback()
{
    // ========================================================
    // LICENSE UI
    // ========================================================

    if (audioProcessor.isLicenseCheckInProgress())
    {
        licenseOverlay.setVisible(true);
        licenseOverlay.toFront(false);

        licenseOverlay.setStatusMessage(
            "Checking license...");
    }
    else if (audioProcessor.isLicenseActivated())
    {
        licenseOverlay.setVisible(false);
    }
    else
    {
        const int remaining =
            audioProcessor.getLicenseFreeUsesRemaining();

        if (remaining > 0)
        {
            // User still has free trial uses.
            licenseOverlay.setVisible(false);
        }
        else
        {
            // Trial exhausted.
            licenseOverlay.setTrialRemaining(0);
            licenseOverlay.setTrialExpired(true);

            licenseOverlay.setVisible(true);
            licenseOverlay.toFront(false);
        }
    }

    const int midiNote =
        audioProcessor.getDetectedMidiNote();

    const float frequency =
        audioProcessor.getDetectedFrequency();

    const float confidence =
        audioProcessor.getPitchConfidence();

    const bool isVoiced =
        audioProcessor.isCurrentlyVoiced();

    // ========================================================
    // CACHE LIVE STATE
    // ========================================================

    displayedMidiNote =
        midiNote;

    displayedFrequency =
        frequency;

    displayedConfidence =
        confidence;

    displayedVoiced =
        isVoiced
        && midiNote >= 0;

    // ========================================================
    // PIANO
    // ========================================================

    pianoKeyboard.setMidiNote (
        displayedVoiced
            ? midiNote
            : -1);

    noteDetection.setMidiNote (
    displayedVoiced
        ? midiNote
        : -1);

    noteDetection.setVoiced(displayedVoiced);

    // ========================================================
    // NOTE NAME
    // ========================================================

    if (displayedVoiced)
    {
        const char* names[] =
        {
            "C", "C#", "D", "D#", "E", "F",
            "F#", "G", "G#", "A", "A#", "B"
        };

        const int pitchClass =
            ((midiNote % 12) + 12) % 12;

        const int octave =
            (midiNote / 12) - 1;

        const juce::String noteName =
            juce::String (names[pitchClass])
            + juce::String (octave);

        noteLabel.setText (
            noteName,
            juce::dontSendNotification);

        frequencyLabel.setText (
            "FREQUENCY  "
            + juce::String (frequency, 1)
            + " Hz",
            juce::dontSendNotification);

        midiLabel.setText (
            "MIDI  "
            + juce::String (midiNote),
            juce::dontSendNotification);

        statusLabel.setText (
            "NOTE DETECTED",
            juce::dontSendNotification);

        statusLabel.setColour (
            juce::Label::textColourId,
            accentColour);
    }
    else
    {
        noteLabel.setText (
            "--",
            juce::dontSendNotification);

        frequencyLabel.setText (
            "FREQUENCY  -- Hz",
            juce::dontSendNotification);

        midiLabel.setText (
            "MIDI  --",
            juce::dontSendNotification);

        statusLabel.setText (
            "LISTENING",
            juce::dontSendNotification);

        statusLabel.setColour (
            juce::Label::textColourId,
            mutedColour);
    }

    // ========================================================
    // REDRAW LIVE INSTRUMENT
    // ========================================================

    repaint (
        juce::Rectangle<int> (
            20,
            88,
            getWidth() - 40,
            235));
}