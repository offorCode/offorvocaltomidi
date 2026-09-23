cmake_minimum_required(VERSION 3.22)

project(OfforVocalToMidi VERSION 1.0.0)

# ============================================================
# JUCE
# ============================================================

add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE"
    "${CMAKE_BINARY_DIR}/JUCE"
)

# ============================================================
# Binary Assets
# ============================================================

juce_add_binary_data(OfforVocalToMidiAssets
    SOURCES
        Resources/OfforVocalToMidi.png
        Resources/setting.png
)

# ============================================================
# Plugin
# ============================================================

juce_add_plugin(OfforVocalToMidi
    COMPANY_NAME "ONNTECH"
    BUNDLE_ID "com.onntech.offoraudio.vocaltomidi"

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE

    VST3_CATEGORIES "Generator"

    COPY_PLUGIN_AFTER_BUILD FALSE

    PLUGIN_MANUFACTURER_CODE ONNR
    PLUGIN_CODE VO2M

    FORMATS VST3 Standalone

    PRODUCT_NAME "Offor Vocal To MIDI"

    VERSION "1.0.0"
)

# ============================================================
# Generate JuceHeader.h
# ============================================================

juce_generate_juce_header(OfforVocalToMidi)

# ============================================================
# Source Files
# ============================================================

target_sources(OfforVocalToMidi
    PRIVATE

        Source/PluginProcessor.cpp
        Source/PluginProcessor.h

        Source/PluginEditor.cpp
        Source/PluginEditor.h

        Source/PitchDetector.cpp
        Source/PitchDetector.h

        Source/VoiceDetector.cpp
        Source/VoiceDetector.h

        Source/NoteTracker.cpp
        Source/NoteTracker.h

        Source/MidiGenerator.cpp
        Source/MidiGenerator.h

        Source/UI/PianoKeyPage.cpp
        Source/UI/PianoKeyPage.h

        Source/UI/SettingsPage.cpp
        Source/UI/SettingsPage.h

        Source/UI/NoteDetectionComponent.cpp
        Source/UI/NoteDetectionComponent.h

        Source/Scales/MusicalProcessor.h
        Source/Scales/MusicalProcessor.cpp

        Source/Chord/ChordGenerator.h
        Source/Chord/ChordGenerator.cpp

        Source/Midi/VirtualMidiOutput.cpp
        Source/Midi/VirtualMidiOutput.h
)

# ============================================================
# C++ Standard
# ============================================================

target_compile_features(
    OfforVocalToMidi
    PRIVATE
        cxx_std_17
)

# ============================================================
# JUCE Compile Definitions
# ============================================================

target_compile_definitions(
    OfforVocalToMidi
    PRIVATE

        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0

        JUCE_VST3_CAN_REPLACE_VST2=0

        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0

        JUCE_STRICT_REFCOUNTEDPOINTER=1
)

# ============================================================
# teVirtualMIDI
# ============================================================

if (WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_include_directories(
        OfforVocalToMidi
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/teVirtualMIDI"
    )

    target_link_libraries(
        OfforVocalToMidi
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/teVirtualMIDI/teVirtualMIDI64.lib"
    )
endif()

# ============================================================
# JUCE Modules
# ============================================================

target_link_libraries(
    OfforVocalToMidi

    PRIVATE
        OfforVocalToMidiAssets

        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_dsp

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
        juce::juce_recommended_lto_flags
)
