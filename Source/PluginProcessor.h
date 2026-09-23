#pragma once

#include <JuceHeader.h>

#include "PitchDetector.h"
#include "VoiceDetector.h"
#include "NoteTracker.h"
#include "MidiGenerator.h"
#include "Chord/ChordGenerator.h"
#include "Scales/MusicalProcessor.h"
#include "Midi/VirtualMidiOutput.h"

class OfforVocalToMidiAudioProcessor
    : public juce::AudioProcessor
{
public:
    // ==========================================================
    // PARAMETER IDS
    // ==========================================================

    struct ParameterIDs
    {
        static constexpr const char* midiChannel =
            "midiChannel";

        static constexpr const char* velocityMode =
            "velocityMode";

        static constexpr const char* fixedVelocity =
            "fixedVelocity";

        static constexpr const char* octaveShift =
            "octaveShift";

        static constexpr const char* transpose =
            "transpose";

        static constexpr const char* midiMode =
            "midiMode";

        static constexpr const char* chordGenre =
            "chordGenre";

        static constexpr const char* chordType =
            "chordType";

        static constexpr const char* chordVoicing =
            "chordVoicing";

        static constexpr const char* chordOctave =
            "chordOctave";

        static constexpr const char* scaleLock =
            "scaleLock";

        static constexpr const char* musicalKey =
            "musicalKey";

        static constexpr const char* musicalScale =
            "musicalScale";

        static constexpr const char* noteStability =
            "noteStability";

        static constexpr const char* noteTolerance =
            "noteTolerance";

        static constexpr const char* noteOffDelay =
            "noteOffDelay";

        static constexpr const char* confidenceThreshold =
            "confidenceThreshold";

        static constexpr const char* voiceThreshold =
            "voiceThreshold";

        static constexpr const char* pitchThreshold =
            "pitchThreshold";

        static constexpr const char* pitchRange =
            "pitchRange";

        static constexpr const char* minPitchFrequency =
            "minPitchFrequency";

        static constexpr const char* maxPitchFrequency =
            "maxPitchFrequency";
    };


    // ==========================================================
    // CONSTRUCTOR / DESTRUCTOR
    // ==========================================================

    OfforVocalToMidiAudioProcessor();

    ~OfforVocalToMidiAudioProcessor() override;


    // ==========================================================
    // AUDIO
    // ==========================================================

    void prepareToPlay(
        double sampleRate,
        int samplesPerBlock) override;

    void releaseResources() override;

    bool isBusesLayoutSupported(
        const BusesLayout& layouts) const override;

    void processBlock(
        juce::AudioBuffer<float>&,
        juce::MidiBuffer&) override;


    // ==========================================================
    // EDITOR
    // ==========================================================

    juce::AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override;


    // ==========================================================
    // PLUGIN INFORMATION
    // ==========================================================

    const juce::String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;


    // ==========================================================
    // PROGRAMS
    // ==========================================================

    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const juce::String getProgramName(
        int index) override;

    void changeProgramName(
        int index,
        const juce::String& newName) override;


    // ==========================================================
    // STATE
    // ==========================================================

    void getStateInformation(
        juce::MemoryBlock& destData) override;

    void setStateInformation(
        const void* data,
        int sizeInBytes) override;


    // ==========================================================
    // APVTS
    // ==========================================================

    juce::AudioProcessorValueTreeState apvts;


    // ==========================================================
    // PARAMETER LAYOUT
    // ==========================================================

    static juce::AudioProcessorValueTreeState::ParameterLayout
    createParameterLayout();


    // ==========================================================
    // UI DATA
    // ==========================================================

    float getDetectedFrequency() const;

    float getPitchConfidence() const;

    float getInputLevel() const;

    int getDetectedMidiNote() const;

    bool isCurrentlyVoiced() const;


    // ==========================================================
    // PITCH / VOICE SETTINGS
    // ==========================================================

    void setNoteStability(int frames);

    void setNoteTolerance(float semitones);

    void setNoteOffDelay(float milliseconds);

    void setConfidenceThreshold(float threshold);

    int getNoteStability() const;

    float getNoteTolerance() const;

    float getNoteOffDelay() const;

    float getConfidenceThreshold() const;


    void setVoiceThreshold(float threshold);

    void setPitchThreshold(float threshold);

    float getVoiceThreshold() const;

    float getPitchThreshold() const;


    // ==========================================================
    // PITCH RANGE
    // ==========================================================

    void setPitchRangePreset(int preset);

    void setCustomPitchRange(
        float minFrequency,
        float maxFrequency);

    int getPitchRangePreset() const;

    float getMinPitchFrequency() const;

    float getMaxPitchFrequency() const;


    // ==========================================================
    // MIDI SETTINGS
    // ==========================================================

    void setMidiChannel(int channel);

    void setVelocityMode(int mode);

    void setFixedVelocity(int velocity);

    void setOctaveShift(int octaves);

    void setTranspose(int semitones);

    int getMidiChannel() const;

    int getVelocityMode() const;

    int getFixedVelocity() const;

    int getOctaveShift() const;

    int getTranspose() const;


    // ==========================================================
    // MIDI GENERATION
    // ==========================================================

    void setMidiMode(int mode);

    int getMidiMode() const;

    void setChordGenre(int genre);

    int getChordGenre() const;

    void setChordType(int type);

    int getChordType() const;

    void setChordVoicing(int voicing);

    int getChordVoicing() const;

    void setChordOctave(int octave);

    int getChordOctave() const;


    // ==========================================================
    // MUSICAL FEATURES
    // ==========================================================

    void setScaleLockEnabled(bool enabled);

    bool isScaleLockEnabled() const;

    void setMusicalKey(int key);

    int getMusicalKey() const;

    void setMusicalScale(int scale);

    int getMusicalScale() const;


    //========================================================
    //Vitual Midi
    //================================================================================================

    void setVirtualMidiEnabled(bool enabled);
    bool isVirtualMidiEnabled() const;

private:

    // ==========================================================
    // APVTS HELPERS
    // ==========================================================

    void applyParametersToDSP();

    void applyPitchRangeToDSP();


    // ==========================================================
    // DSP COMPONENTS
    // ==========================================================

    PitchDetector pitchDetector;

    VoiceDetector voiceDetector;

    NoteTracker noteTracker;

    ChordGenerator chordGenerator;

    MidiGenerator midiGenerator;

    MusicalProcessor musicalProcessor;

    VirtualMidiOutput virtualMidiOutput;
    bool virtualMidiEnabled = false;


    // ==========================================================
    // AUDIO
    // ==========================================================

    double currentSampleRate = 44100.0;

    int maximumBlockSize = 0;

    juce::AudioBuffer<float> monoBuffer;


    // ==========================================================
    // CURRENT DSP STATE
    // ==========================================================

    float detectedFrequency = 0.0f;

    float pitchConfidence = 0.0f;

    float inputLevel = 0.0f;

    float lastVelocity = 100.0f;



    // ==========================================================
    // ATOMIC UI VALUES
    // ==========================================================

    std::atomic<int> detectedMidiNote { -1 };

    std::atomic<bool> voiced { false };

    std::atomic<float> atomicFrequency { 0.0f };

    std::atomic<float> atomicConfidence { 0.0f };

    std::atomic<float> atomicInputLevel { 0.0f };

    std::atomic<bool> midiResetRequested { false };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        OfforVocalToMidiAudioProcessor)
};