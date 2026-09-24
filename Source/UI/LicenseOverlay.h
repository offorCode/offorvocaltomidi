#pragma once

#include <JuceHeader.h>

class LicenseOverlay
    : public juce::Component
{
public:
    LicenseOverlay();
    ~LicenseOverlay() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setTrialRemaining(int remainingUses);
    void setTrialExpired(bool expired);

    void setStatusMessage(const juce::String& message);
    void clearStatusMessage();

    std::function<void(const juce::String&)> onActivate;
    std::function<void()> onGetLicense;

private:
    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::Label trialLabel;

    juce::TextEditor licenseKeyEditor;
    juce::TextButton activateButton;
    juce::TextButton getLicenseButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseOverlay)
};