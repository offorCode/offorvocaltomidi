#pragma once

#include <JuceHeader.h>

#include "../Version.h"


// ============================================================
// OFFOR VOCAL TO MIDI LICENSE MANAGER
// ============================================================
// Handles:
//
// 1. Permanent installation ID
// 2. Server registration
// 3. Free-use tracking
// 4. Server usage validation
// 5. License activation
// 6. Local license persistence
//
// ============================================================

class LicenseManager
{
public:

    // ========================================================
    // PRODUCT INFORMATION
    // ========================================================

    static constexpr const char* PRODUCT_NAME =
        "OfforVocalToMidi";

    static constexpr const char* PRODUCT_VERSION =
        OFFOR_VOCAL_TO_MIDI_VERSION_STRING;


    // ========================================================
    // FREE TRIAL
    // ========================================================
    //
    // Offor Vocal To MIDI gives users 30 free uses.
    //
    // IMPORTANT:
    //
    // The server also enforces this limit.
    // This value is mainly used by the client for local
    // calculations and UI.
    //
    // ========================================================

    static constexpr int FREE_USES = 30;


    // ========================================================
    // LICENSE SERVER
    // ========================================================

    static constexpr const char* LICENSE_SERVER =
        "https://chezchris.onrender.com";


    // ========================================================
    // CONSTRUCTOR
    // ========================================================

    LicenseManager();


    // ========================================================
    // INSTALLATION
    // ========================================================

    juce::String getInstallationId() const;


    // ========================================================
    // LICENSE STATUS
    // ========================================================

    bool isActivated() const;

    juce::String getStoredLicense() const;


    // ========================================================
    // LOCAL USAGE
    // ========================================================

    int getUsageCount() const;

    int getFreeUsesRemaining() const;

    bool hasFreeUsesRemaining() const;


    // ========================================================
    // SERVER USAGE INFORMATION
    // ========================================================
    //
    // Runtime information only.
    //
    // These values are NOT saved as separate XML properties.
    //
    // ========================================================

    int getServerFreeUses() const;

    int getServerFreeUsesLimit() const;

    bool wasLastServerCheckAllowed() const;


    // ========================================================
    // USAGE
    // ========================================================

    void incrementUsage();


    // ========================================================
    // SERVER
    // ========================================================

    bool registerInstallation();

    bool checkUsage();


    // ========================================================
    // LICENSE ACTIVATION
    // ========================================================

    bool activate(
        const juce::String& licenseKey
    );


    enum class ServerStatus
    {
        Unknown,
        Success,
        NoConnection,
        ServerRejected,
        InvalidResponse
    };

    ServerStatus getLastServerStatus() const;


private:

    // ========================================================
    // JUCE APPLICATION PROPERTIES
    // ========================================================

    juce::ApplicationProperties properties;


    // ========================================================
    // RUNTIME SERVER STATE
    // ========================================================
    //
    // These are kept in memory only.
    //
    // They are NOT written into the user's XML file.
    //
    // ========================================================

    bool lastServerAllowed = false;

    int lastServerFreeUses = 0;

    int lastServerFreeUsesLimit = FREE_USES;


    ServerStatus lastServerStatus =
    ServerStatus::Unknown;


    // ========================================================
    // INSTALLATION ID
    // ========================================================

    juce::String createInstallationId();


    // ========================================================
    // PROPERTY ACCESS
    // ========================================================

    juce::PropertiesFile* getProperties();


    // ========================================================
    // NON-COPYABLE
    // ========================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        LicenseManager
    )
};