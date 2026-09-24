#include "LicenseOverlay.h"

LicenseOverlay::LicenseOverlay()
{
    // ==========================================================
    // TITLE
    // ==========================================================

    titleLabel.setText(
        "OFFOR VOCAL TO MIDI",
        juce::dontSendNotification);

    titleLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(26.0f)
                .withStyle("Bold")));

    titleLabel.setColour(
        juce::Label::textColourId,
        juce::Colour::fromString("fff2f2f4"));

    titleLabel.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(titleLabel);


    // ==========================================================
    // STATUS
    // ==========================================================

    statusLabel.setText(
        "TRIAL VERSION",
        juce::dontSendNotification);

    statusLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(18.0f)
                .withStyle("Bold")));

    statusLabel.setColour(
        juce::Label::textColourId,
        juce::Colour::fromString("ffdf513e"));

    statusLabel.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(statusLabel);


    // ==========================================================
    // TRIAL INFORMATION
    // ==========================================================

    trialLabel.setText(
        "30 FREE USES INCLUDED",
        juce::dontSendNotification);

    trialLabel.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(15.0f)));

    trialLabel.setColour(
        juce::Label::textColourId,
        juce::Colour::fromString("ff858791"));

    trialLabel.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(trialLabel);


    // ==========================================================
    // LICENSE KEY
    // ==========================================================

    licenseKeyEditor.setTextToShowWhenEmpty(
        "Enter your license key",
        juce::Colour::fromString("ff858791"));

    licenseKeyEditor.setFont(
        juce::Font(
            juce::FontOptions{}
                .withHeight(15.0f)));

    licenseKeyEditor.setColour(
        juce::TextEditor::backgroundColourId,
        juce::Colour::fromString("ff17181d"));

    licenseKeyEditor.setColour(
        juce::TextEditor::outlineColourId,
        juce::Colour::fromString("ff30323a"));

    licenseKeyEditor.setColour(
        juce::TextEditor::focusedOutlineColourId,
        juce::Colour::fromString("ffdf513e"));

    licenseKeyEditor.setColour(
        juce::TextEditor::textColourId,
        juce::Colour::fromString("fff2f2f4"));

    licenseKeyEditor.setJustification(
        juce::Justification::centred);

    licenseKeyEditor.setBorder(
        juce::BorderSize<int>(1));

    addAndMakeVisible(licenseKeyEditor);


    // ==========================================================
    // ACTIVATE BUTTON
    // ==========================================================

    activateButton.setButtonText("ACTIVATE");

    activateButton.setColour(
        juce::TextButton::buttonColourId,
        juce::Colour::fromString("ffdf513e"));

    activateButton.setColour(
        juce::TextButton::textColourOffId,
        juce::Colour::fromString("ffffffff"));

    activateButton.setColour(
        juce::TextButton::buttonOnColourId,
        juce::Colour::fromString("ffe89e81"));

    activateButton.onClick = [this]
    {
        const auto key = licenseKeyEditor
            .getText()
            .trim();

        if (key.isEmpty())
        {
            setStatusMessage("Please enter a license key.");
            return;
        }

        if (onActivate != nullptr)
            onActivate(key);
    };

    addAndMakeVisible(activateButton);


    // ==========================================================
    // GET LICENSE BUTTON
    // ==========================================================

    getLicenseButton.setButtonText("GET LICENSE");

    getLicenseButton.setColour(
        juce::TextButton::buttonColourId,
        juce::Colour::fromString("ff1d1f25"));

    getLicenseButton.setColour(
        juce::TextButton::textColourOffId,
        juce::Colour::fromString("fff2f2f4"));

    getLicenseButton.setColour(
        juce::TextButton::buttonOnColourId,
        juce::Colour::fromString("ff30323a"));

    getLicenseButton.onClick = [this]
    {
        if (onGetLicense != nullptr)
            onGetLicense();
    };

    addAndMakeVisible(getLicenseButton);
}


// ==========================================================
// PAINT
// ==========================================================

void LicenseOverlay::paint(juce::Graphics& g)
{
    // Full overlay background
    g.fillAll(
        juce::Colour::fromString("ff111216"));

    // Main panel
    auto panel = getLocalBounds()
        .reduced(70, 45);

    g.setColour(
        juce::Colour::fromString("ff17181d"));

    g.fillRoundedRectangle(
        panel.toFloat(),
        12.0f);

    // Border
    g.setColour(
        juce::Colour::fromString("ff30323a"));

    g.drawRoundedRectangle(
        panel.toFloat(),
        12.0f,
        1.0f);

    // Orange accent line
    auto accent = panel
        .withHeight(3)
        .reduced(1, 0);

    g.setColour(
        juce::Colour::fromString("ffdf513e"));

    g.fillRoundedRectangle(
        accent.toFloat(),
        1.5f);
}


// ==========================================================
// RESIZED
// ==========================================================

void LicenseOverlay::resized()
{
    auto area = getLocalBounds()
        .reduced(70, 45);

    const int centreX = area.getCentreX();

    titleLabel.setBounds(
        centreX - 180,
        area.getY() + 45,
        360,
        35);

    statusLabel.setBounds(
        centreX - 180,
        area.getY() + 92,
        360,
        28);

    trialLabel.setBounds(
        centreX - 180,
        area.getY() + 125,
        360,
        25);

    licenseKeyEditor.setBounds(
        centreX - 150,
        area.getY() + 170,
        300,
        42);

    activateButton.setBounds(
        centreX - 150,
        area.getY() + 225,
        300,
        42);

    getLicenseButton.setBounds(
        centreX - 150,
        area.getY() + 278,
        300,
        42);
}


// ==========================================================
// TRIAL STATUS
// ==========================================================

void LicenseOverlay::setTrialRemaining(int remainingUses)
{
    if (remainingUses <= 0)
    {
        setTrialExpired(true);
        return;
    }

    statusLabel.setText(
        "TRIAL VERSION",
        juce::dontSendNotification);

    trialLabel.setText(
        juce::String(remainingUses)
            + " FREE USE"
            + (remainingUses == 1 ? "" : "S")
            + " REMAINING",
        juce::dontSendNotification);
}

void LicenseOverlay::setTrialExpired(bool expired)
{
    if (expired)
    {
        statusLabel.setText(
            "TRIAL EXPIRED",
            juce::dontSendNotification);

        trialLabel.setText(
            "ACTIVATE TO CONTINUE USING OFFOR VOCAL TO MIDI",
            juce::dontSendNotification);
    }
    else
    {
        statusLabel.setText(
            "TRIAL VERSION",
            juce::dontSendNotification);
    }
}


// ==========================================================
// STATUS MESSAGE
// ==========================================================

void LicenseOverlay::setStatusMessage(
    const juce::String& message)
{
    statusLabel.setText(
        message,
        juce::dontSendNotification);
}

void LicenseOverlay::clearStatusMessage()
{
    statusLabel.setText(
        "TRIAL VERSION",
        juce::dontSendNotification);
}