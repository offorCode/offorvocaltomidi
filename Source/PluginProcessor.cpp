#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>
#include <iostream>

// =============================================================
// PARAMETER LAYOUT
// =============================================================

juce::AudioProcessorValueTreeState::ParameterLayout
OfforVocalToMidiAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;


    // =========================================================
    // MIDI
    // =========================================================

    layout.add(
        std::make_unique<juce::AudioParameterInt>(
            ParameterIDs::midiChannel,
            "MIDI Channel",
            1,
            16,
            1));


    layout.add(
        std::make_unique<juce::AudioParameterChoice>(
            ParameterIDs::velocityMode,
            "Velocity Mode",
            juce::StringArray
            {
                "Dynamic",
                "Fixed"
            },
            0));


    layout.add(
        std::make_unique<juce::AudioParameterInt>(
            ParameterIDs::fixedVelocity,
            "Fixed Velocity",
            1,
            127,
            100));


    layout.add(
        std::make_unique<juce::AudioParameterInt>(
            ParameterIDs::octaveShift,
            "Octave Shift",
            -3,
            3,
            0));


    layout.add(
        std::make_unique<juce::AudioParameterInt>(
            ParameterIDs::transpose,
            "Transpose",
            -12,
            12,
            0));


    // =========================================================
    // MIDI GENERATION
    // =========================================================

    layout.add(
        std::make_unique<juce::AudioParameterChoice>(
            ParameterIDs::midiMode,
            "MIDI Mode",
            juce::StringArray
            {
                "Single Note",
                "Chord",
                "Arpeggio"
            },
            0));


    layout.add(
        std::make_unique<juce::AudioParameterInt>(
            ParameterIDs::chordGenre,
            "Chord Genre",
            0,
            9,
            0));


    layout.add(
        std::make_unique<juce::AudioParameterInt>(
            ParameterIDs::chordType,
            "Chord Type",
            0,
            17,
            0));


    layout.add(
        std::make_unique<juce::AudioParameterInt>(
            ParameterIDs::chordVoicing,
            "Chord Voicing",
            0,
            2,
            0));


    layout.add(
        std::make_unique<juce::AudioParameterInt>(
            ParameterIDs::chordOctave,
            "Chord Octave",
            -3,
            3,
            0));


    // =========================================================
    // SCALE / MUSICAL PROCESSING
    // =========================================================

    layout.add(
        std::make_unique<juce::AudioParameterBool>(
            ParameterIDs::scaleLock,
            "Scale Lock",
            false));


    layout.add(
        std::make_unique<juce::AudioParameterChoice>(
            ParameterIDs::musicalKey,
            "Key",
            juce::StringArray
            {
                "C",
                "C#",
                "D",
                "D#",
                "E",
                "F",
                "F#",
                "G",
                "G#",
                "A",
                "A#",
                "B"
            },
            0));


    layout.add(
        std::make_unique<juce::AudioParameterChoice>(
            ParameterIDs::musicalScale,
            "Scale",
            juce::StringArray
            {
                "Chromatic",
                "Major",
                "Minor",
                "Major Pentatonic",
                "Minor Pentatonic",
                "Blues"
            },
            0));


    // =========================================================
    // PITCH / NOTE TRACKING
    // =========================================================

    layout.add(
        std::make_unique<juce::AudioParameterInt>(
            ParameterIDs::noteStability,
            "Note Stability",
            1,
            10,
            2));


    layout.add(
        std::make_unique<juce::AudioParameterFloat>(
            ParameterIDs::noteTolerance,
            "Note Tolerance",
            juce::NormalisableRange<float>(
                0.05f,
                1.0f,
                0.01f),
            0.35f));


    layout.add(
        std::make_unique<juce::AudioParameterFloat>(
            ParameterIDs::noteOffDelay,
            "Note-Off Delay",
            juce::NormalisableRange<float>(
                10.0f,
                500.0f,
                1.0f),
            80.0f));


    layout.add(
        std::make_unique<juce::AudioParameterFloat>(
            ParameterIDs::confidenceThreshold,
            "Confidence Threshold",
            juce::NormalisableRange<float>(
                0.30f,
                0.95f,
                0.01f),
            0.55f));


    layout.add(
        std::make_unique<juce::AudioParameterFloat>(
            ParameterIDs::voiceThreshold,
            "Voice Threshold",
            juce::NormalisableRange<float>(
                0.001f,
                0.100f,
                0.001f),
            0.008f));


    layout.add(
        std::make_unique<juce::AudioParameterFloat>(
            ParameterIDs::pitchThreshold,
            "Pitch Threshold",
            juce::NormalisableRange<float>(
                0.01f,
                1.0f,
                0.01f),
            0.15f));


    // =========================================================
    // PITCH RANGE
    // =========================================================

    layout.add(
        std::make_unique<juce::AudioParameterChoice>(
            ParameterIDs::pitchRange,
            "Pitch Range",
            juce::StringArray
            {
                "Bass",
                "Low Male",
                "Male",
                "Female",
                "High Female",
                "Custom"
            },
            2));


    layout.add(
        std::make_unique<juce::AudioParameterFloat>(
            ParameterIDs::minPitchFrequency,
            "Minimum Pitch Frequency",
            juce::NormalisableRange<float>(
                40.0f,
                1000.0f,
                1.0f),
            100.0f));


    layout.add(
        std::make_unique<juce::AudioParameterFloat>(
            ParameterIDs::maxPitchFrequency,
            "Maximum Pitch Frequency",
            juce::NormalisableRange<float>(
                60.0f,
                2000.0f,
                1.0f),
            400.0f));


    return layout;
}

// =============================================================
// CONSTRUCTOR
// =============================================================

OfforVocalToMidiAudioProcessor::
OfforVocalToMidiAudioProcessor()
    : AudioProcessor(
        BusesProperties()

#if ! JucePlugin_IsMidiEffect

        .withInput(
            "Audio Input",
            juce::AudioChannelSet::stereo(),
            true)

#endif
    ),
    apvts(
        *this,
        nullptr,
        "Parameters",
        createParameterLayout())
{
    // ========================================================
    // LICENSE REGISTRATION
    // ========================================================

    if (licenseManager.isActivated())
    {
        licenseAllowed.store(
            true,
            std::memory_order_release);
    }

    // ========================================================
    // CREATE LICENSE WORKER
    //
    // IMPORTANT:
    //
    // The worker is created here, NOT from processBlock().
    //
    // processBlock() will only start the already-existing
    // worker thread when the license check is required.
    // ========================================================

    licenseWorker =
        std::make_unique<LicenseWorker>(*this);
}

// =============================================================
// START LICENSE CHECK
// =============================================================

void
OfforVocalToMidiAudioProcessor::
startLicenseCheck()
{
    // ========================================================
    // Already completed?
    // ========================================================

    if (licenseUseChecked.load(
            std::memory_order_acquire))
    {
        return;
    }


    // ========================================================
    // Already running?
    // ========================================================

    if (licenseCheckInProgress.exchange(
            true,
            std::memory_order_acq_rel))
    {
        return;
    }


    // ========================================================
    // SAFETY
    //
    // The worker should have been created in the constructor.
    //
    // If it somehow does not exist, cancel this attempt.
    // ========================================================

    if (licenseWorker == nullptr)
    {
        licenseCheckInProgress.store(
            false,
            std::memory_order_release);

        return;
    }


    // ========================================================
    // START WORKER
    // ========================================================

    if (!licenseWorker->isThreadRunning())
    {
        licenseWorker->startThread();
    }
}

// =============================================================
// PERFORM LICENSE USE CHECK
//
// Runs ONLY on LicenseWorker.
//
// NEVER call this directly from processBlock().
// =============================================================

void
OfforVocalToMidiAudioProcessor::
performLicenseUseCheck()
{
    bool allowed = false;

    try
    {
        // ====================================================
        // REGISTER INSTALLATION
        //
        // This must happen before /use.
        // ====================================================

        licenseManager.registerInstallation();


        // ====================================================
        // CHECK / CONSUME FREE USE
        // ====================================================

        allowed =
            licenseManager.checkUsage();
    }
    catch (...)
    {
        // ====================================================
        // NEVER ALLOW A NETWORK EXCEPTION TO ESCAPE
        // THE WORKER THREAD.
        // ====================================================

        allowed = false;
    }


    // ========================================================
    // PUBLISH RESULT
    //
    // These are atomics because processBlock() reads them.
    // ========================================================

    licenseAllowed.store(
        allowed,
        std::memory_order_release);

    licenseUseChecked.store(
        true,
        std::memory_order_release);


    // ========================================================
    // WORKER FINISHED
    // ========================================================

    licenseCheckInProgress.store(
        false,
        std::memory_order_release);
}


bool OfforVocalToMidiAudioProcessor::isLicenseCheckInProgress() const
{
    return licenseCheckInProgress.load(
        std::memory_order_acquire);
}

// =============================================================
// APPLY APVTS PARAMETERS TO DSP
// =============================================================

void
OfforVocalToMidiAudioProcessor::applyParametersToDSP()
{
    const int newMidiChannel =
        apvts.getRawParameterValue(
            ParameterIDs::midiChannel)->load();

    const int newVelocityMode =
        apvts.getRawParameterValue(
            ParameterIDs::velocityMode)->load();

    const int newFixedVelocity =
        apvts.getRawParameterValue(
            ParameterIDs::fixedVelocity)->load();

    const int newOctaveShift =
        apvts.getRawParameterValue(
            ParameterIDs::octaveShift)->load();

    const int newTranspose =
        apvts.getRawParameterValue(
            ParameterIDs::transpose)->load();

    const int newMidiMode =
        apvts.getRawParameterValue(
            ParameterIDs::midiMode)->load();

    const int newChordGenre =
        apvts.getRawParameterValue(
            ParameterIDs::chordGenre)->load();

    const int newChordType =
        apvts.getRawParameterValue(
            ParameterIDs::chordType)->load();

    const int newChordVoicing =
        apvts.getRawParameterValue(
            ParameterIDs::chordVoicing)->load();

    const int newChordOctave =
        apvts.getRawParameterValue(
            ParameterIDs::chordOctave)->load();

    const bool newScaleLock =
        apvts.getRawParameterValue(
            ParameterIDs::scaleLock)->load() > 0.5f;

    const int newKey =
        apvts.getRawParameterValue(
            ParameterIDs::musicalKey)->load();

    const int newScale =
        apvts.getRawParameterValue(
            ParameterIDs::musicalScale)->load();

    const int newNoteStability =
        apvts.getRawParameterValue(
            ParameterIDs::noteStability)->load();

    const float newNoteTolerance =
        apvts.getRawParameterValue(
            ParameterIDs::noteTolerance)->load();

    const float newNoteOffDelay =
        apvts.getRawParameterValue(
            ParameterIDs::noteOffDelay)->load();

    const float newConfidenceThreshold =
        apvts.getRawParameterValue(
            ParameterIDs::confidenceThreshold)->load();

    const float newVoiceThreshold =
        apvts.getRawParameterValue(
            ParameterIDs::voiceThreshold)->load();

    const float newPitchThreshold =
        apvts.getRawParameterValue(
            ParameterIDs::pitchThreshold)->load();


    // ---------------------------------------------------------
    // MIDI
    // ---------------------------------------------------------

    midiGenerator.setMidiChannel(
        juce::jlimit(
            1,
            16,
            newMidiChannel));


    // ---------------------------------------------------------
    // NOTE TRACKING
    // ---------------------------------------------------------

    noteTracker.setStabilityRequired(
        juce::jlimit(
            1,
            10,
            newNoteStability));

    noteTracker.setNoteTolerance(
        juce::jlimit(
            0.05f,
            1.0f,
            newNoteTolerance));

    noteTracker.setNoteOffDelayMs(
        juce::jlimit(
            10.0f,
            500.0f,
            newNoteOffDelay));

    noteTracker.setConfidenceThreshold(
        juce::jlimit(
            0.30f,
            0.95f,
            newConfidenceThreshold));


    // ---------------------------------------------------------
    // VOICE / PITCH
    // ---------------------------------------------------------

    voiceDetector.setThreshold(
        juce::jlimit(
            0.001f,
            0.100f,
            newVoiceThreshold));

    pitchDetector.setThreshold(
        juce::jlimit(
            0.01f,
            1.0f,
            newPitchThreshold));


    // ---------------------------------------------------------
    // CHORD GENERATION
    // ---------------------------------------------------------

    chordGenerator.setMode(
        static_cast<ChordGenerator::Mode>(
            juce::jlimit(
                0,
                2,
                newMidiMode)));

    chordGenerator.setGenre(
        static_cast<ChordGenerator::Genre>(
            juce::jlimit(
                0,
                9,
                newChordGenre)));

    chordGenerator.setChordType(
        static_cast<ChordGenerator::ChordType>(
            juce::jlimit(
                0,
                17,
                newChordType)));

    chordGenerator.setVoicing(
        static_cast<ChordGenerator::Voicing>(
            juce::jlimit(
                0,
                2,
                newChordVoicing)));

    chordGenerator.setOctave(
        juce::jlimit(
            -3,
            3,
            newChordOctave));


    // ---------------------------------------------------------
    // MUSICAL PROCESSING
    // ---------------------------------------------------------

    musicalProcessor.setScaleLockEnabled(
        newScaleLock);

    musicalProcessor.setKey(
        static_cast<MusicalProcessor::Key>(
            juce::jlimit(
                0,
                11,
                newKey)));

    musicalProcessor.setScale(
        static_cast<MusicalProcessor::Scale>(
            juce::jlimit(
                0,
                5,
                newScale)));

    musicalProcessor.setTranspose(
        juce::jlimit(
            -12,
            12,
            newTranspose));

    musicalProcessor.setOctaveShift(
        juce::jlimit(
            -3,
            3,
            newOctaveShift));
}

// =============================================================
// APPLY PITCH RANGE
// =============================================================

void
OfforVocalToMidiAudioProcessor::applyPitchRangeToDSP()
{
    const int preset =
        juce::jlimit(
            0,
            5,
            static_cast<int>(
                apvts.getRawParameterValue(
                    ParameterIDs::pitchRange)->load()));

    float minFrequency = 100.0f;
    float maxFrequency = 400.0f;


    switch (preset)
    {
        case 0: // Bass
            minFrequency = 60.0f;
            maxFrequency = 250.0f;
            break;

        case 1: // Low Male
            minFrequency = 75.0f;
            maxFrequency = 300.0f;
            break;

        case 2: // Male
            minFrequency = 100.0f;
            maxFrequency = 400.0f;
            break;

        case 3: // Female
            minFrequency = 150.0f;
            maxFrequency = 700.0f;
            break;

        case 4: // High Female
            minFrequency = 200.0f;
            maxFrequency = 1000.0f;
            break;

        case 5: // Custom
        {
            minFrequency =
                apvts.getRawParameterValue(
                    ParameterIDs::minPitchFrequency)->load();

            maxFrequency =
                apvts.getRawParameterValue(
                    ParameterIDs::maxPitchFrequency)->load();

            minFrequency =
                juce::jlimit(
                    40.0f,
                    1000.0f,
                    minFrequency);

            maxFrequency =
                juce::jlimit(
                    minFrequency + 20.0f,
                    2000.0f,
                    maxFrequency);

            break;
        }

        default:
            break;
    }


    pitchDetector.setMinFrequency(
        minFrequency);

    pitchDetector.setMaxFrequency(
        maxFrequency);
}

// =============================================================
// DESTRUCTOR
// =============================================================

// =============================================================
// DESTRUCTOR
// =============================================================

OfforVocalToMidiAudioProcessor::
~OfforVocalToMidiAudioProcessor()
{
    // ========================================================
    // STOP LICENSE WORKER
    //
    // This MUST happen before LicenseManager is destroyed.
    //
    // performLicenseUseCheck() accesses licenseManager.
    // Therefore the worker must be completely finished before
    // this processor continues destruction.
    // ========================================================

    if (licenseWorker != nullptr)
    {
        licenseWorker->stopThread(-1);

        licenseWorker.reset();
    }

    // ========================================================
    // Nothing asynchronous may still reference this processor.
    // ========================================================
}

// =============================================================
// BASIC PLUGIN INFORMATION
// =============================================================

const juce::String
OfforVocalToMidiAudioProcessor::getName() const
{
    return JucePlugin_Name;
}


bool
OfforVocalToMidiAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}


bool
OfforVocalToMidiAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}


bool
OfforVocalToMidiAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}


double
OfforVocalToMidiAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}


// =============================================================
// PROGRAMS
// =============================================================

int
OfforVocalToMidiAudioProcessor::getNumPrograms()
{
    return 1;
}


int
OfforVocalToMidiAudioProcessor::getCurrentProgram()
{
    return 0;
}


void
OfforVocalToMidiAudioProcessor::
setCurrentProgram(int)
{
}


const juce::String
OfforVocalToMidiAudioProcessor::
getProgramName(int)
{
    return {};
}


void
OfforVocalToMidiAudioProcessor::
changeProgramName(
    int,
    const juce::String&)
{
}


// =============================================================
// PREPARE TO PLAY
// =============================================================

// void
// OfforVocalToMidiAudioProcessor::
// prepareToPlay(
//     double sampleRate,
//     int samplesPerBlock)
// {
//     currentSampleRate = sampleRate;

//     maximumBlockSize = samplesPerBlock;


//     // ---------------------------------------------------------
//     // Prepare our preallocated mono buffer.
//     // ---------------------------------------------------------

//     monoBuffer.setSize(
//         1,
//         maximumBlockSize,
//         false,
//         false,
//         true);


//     // ---------------------------------------------------------
//     // Prepare DSP
//     // ---------------------------------------------------------

//     pitchDetector.prepare(sampleRate);

//     pitchDetector.setMinFrequency(
//         minPitchFrequency);

//     pitchDetector.setMaxFrequency(
//         maxPitchFrequency);

//     voiceDetector.prepare(sampleRate);

//     voiceDetector.setThreshold(
//         voiceDetectorThreshold);

//     pitchDetector.setThreshold(
//         pitchDetectorThreshold);

//     noteTracker.reset();

//     noteTracker.setStabilityRequired(
//         noteStability);

//     noteTracker.setNoteTolerance(
//         noteTolerance);

//     noteTracker.setNoteOffDelayMs(
//         noteOffDelay);

//     noteTracker.setConfidenceThreshold(
//         confidenceThreshold);

    
//     midiGenerator.setMidiChannel(midiChannel);
//     midiGenerator.reset();

//     chordGenerator.reset();

//     chordGenerator.setMode(
//         static_cast<ChordGenerator::Mode>(midiMode));

//     chordGenerator.setGenre(
//         static_cast<ChordGenerator::Genre>(chordGenre));

//     chordGenerator.setChordType(
//         static_cast<ChordGenerator::ChordType>(chordType));

//     chordGenerator.setVoicing(
//         static_cast<ChordGenerator::Voicing>(chordVoicing));

//     chordGenerator.setOctave(
//         chordOctave);


//     musicalProcessor.setScaleLockEnabled(
//         scaleLockEnabled);

//     musicalProcessor.setKey(
//         static_cast<MusicalProcessor::Key>(
//             musicalKey));

//     musicalProcessor.setScale(
//         static_cast<MusicalProcessor::Scale>(
//             musicalScale));

//     musicalProcessor.setTranspose(
//         transpose);

//     musicalProcessor.setOctaveShift(
//         octaveShift);


//     // ---------------------------------------------------------
//     // Reset state
//     // ---------------------------------------------------------

//     detectedFrequency = 0.0f;

//     pitchConfidence = 0.0f;

//     inputLevel = 0.0f;

//     lastVelocity = 64.0f;


//     detectedMidiNote.store(
//         -1,
//         std::memory_order_relaxed);

//     voiced.store(
//         false,
//         std::memory_order_relaxed);

//     atomicFrequency.store(
//         0.0f,
//         std::memory_order_relaxed);

//     atomicConfidence.store(
//         0.0f,
//         std::memory_order_relaxed);

//     atomicInputLevel.store(
//         0.0f,
//         std::memory_order_relaxed);



    
// }


void
OfforVocalToMidiAudioProcessor::
prepareToPlay(
    double sampleRate,
    int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    maximumBlockSize = samplesPerBlock;


    monoBuffer.setSize(
        1,
        maximumBlockSize,
        false,
        false,
        true);


    // ---------------------------------------------------------
    // Prepare DSP
    // ---------------------------------------------------------

    pitchDetector.prepare(sampleRate);

    voiceDetector.prepare(sampleRate);


    noteTracker.reset();

    midiGenerator.reset();

    chordGenerator.reset();


    // ---------------------------------------------------------
    // Apply APVTS values
    // ---------------------------------------------------------

    applyParametersToDSP();

    applyPitchRangeToDSP();


    // ---------------------------------------------------------
    // Reset state
    // ---------------------------------------------------------

    detectedFrequency = 0.0f;

    pitchConfidence = 0.0f;

    inputLevel = 0.0f;

    lastVelocity = 64.0f;


    detectedMidiNote.store(
        -1,
        std::memory_order_relaxed);

    voiced.store(
        false,
        std::memory_order_relaxed);

    atomicFrequency.store(
        0.0f,
        std::memory_order_relaxed);

    atomicConfidence.store(
        0.0f,
        std::memory_order_relaxed);

    atomicInputLevel.store(
        0.0f,
        std::memory_order_relaxed);

    // Open virtual MIDI only when the host has
    // actually prepared the audio processor.
    
}

// =============================================================
// RELEASE RESOURCES
// =============================================================

void
OfforVocalToMidiAudioProcessor::
releaseResources()
{
   
    // ========================================================
    // RESET DSP
    // ========================================================

    pitchDetector.reset();

    voiceDetector.reset();

    noteTracker.reset();

    midiGenerator.reset();

    monoBuffer.setSize(
        0,
        0
    );
}


// =============================================================
// BUS LAYOUT
// =============================================================

bool
OfforVocalToMidiAudioProcessor::
isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
#if ! JucePlugin_IsMidiEffect

    const auto input =
        layouts.getMainInputChannelSet();

    const auto output =
        layouts.getMainOutputChannelSet();


    // We require an audio input.
    if (input == juce::AudioChannelSet::disabled())
        return false;


    // OFFOR accepts mono or stereo input.
    if (input != juce::AudioChannelSet::mono() &&
        input != juce::AudioChannelSet::stereo())
    {
        return false;
    }


    // OFFOR intentionally has no audio output.
    if (output != juce::AudioChannelSet::disabled())
        return false;


    return true;

#else

    juce::ignoreUnused(layouts);

    return true;

#endif
}


// =============================================================
// PROCESS BLOCK
// =============================================================

void
OfforVocalToMidiAudioProcessor::
processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;


    const int numSamples =
        buffer.getNumSamples();

    const int numChannels =
        buffer.getNumChannels();


    // =========================================================
    // LICENSE SESSION CHECK
    //
    // Start the background license check BEFORE the license
    // gate. Otherwise a new/unverified installation would
    // return before the check could ever start.
    // =========================================================

    if (!licenseUseChecked.load(
            std::memory_order_acquire)
        && !licenseCheckInProgress.load(
            std::memory_order_acquire)
        && numSamples > 0
        && numChannels > 0)
    {
        startLicenseCheck();
    }


    // =========================================================
    // LICENSE GATE
    //
    // Do not process audio until the background license check
    // has completed and access has been granted.
    // =========================================================

    if (!licenseAllowed.load(
            std::memory_order_acquire))
    {
        return;
    }


    // =========================================================
    // SAFETY CHECK
    // =========================================================

    if (numSamples <= 0 ||
        numChannels <= 0)
    {
        return;
    }


    if (numSamples > monoBuffer.getNumSamples())
    {
        return;
    }


    applyParametersToDSP();


    // =========================================================
    // GENERATED MIDI BUFFER
    // =========================================================

    juce::MidiBuffer generatedMidi;


    // =========================================================
    // HELPER: SEND GENERATED MIDI TO VIRTUAL MIDI + HOST
    // =========================================================

    auto flushGeneratedMidi =
    [&]()
    {
        if (generatedMidi.isEmpty())
            return;

        // -----------------------------------------------------
        // Preserve existing VST3 MIDI output
        // -----------------------------------------------------

        midiMessages.addEvents(
            generatedMidi,
            0,
            buffer.getNumSamples(),
            0);
    };


    // =========================================================
    // MIDI RESET
    // =========================================================

    if (midiResetRequested.exchange(
            false,
            std::memory_order_acquire))
    {
        midiGenerator.noteOff(
            generatedMidi,
            0);

        flushGeneratedMidi();
    }


    // =========================================================
    // CREATE MONO SIGNAL
    // =========================================================

    float* monoData =
        monoBuffer.getWritePointer(0);


    const float channelGain =
        1.0f /
        static_cast<float>(numChannels);


    std::fill(
        monoData,
        monoData + numSamples,
        0.0f);


    for (int channel = 0;
         channel < numChannels;
         ++channel)
    {
        const float* channelData =
            buffer.getReadPointer(channel);


        for (int sample = 0;
             sample < numSamples;
             ++sample)
        {
            monoData[sample] +=
                channelData[sample] *
                channelGain;
        }
    }


    // =========================================================
    // VOICE DETECTION
    // =========================================================

    float level = 0.0f;

    const bool isVoiced =
        voiceDetector.isVoiced(
            monoData,
            numSamples,
            level);


    inputLevel = level;


    voiced.store(
        isVoiced,
        std::memory_order_relaxed);


    atomicInputLevel.store(
        level,
        std::memory_order_relaxed);


    // =========================================================
    // NO VOICE
    // =========================================================

    if (!isVoiced)
    {
        atomicFrequency.store(
            0.0f,
            std::memory_order_relaxed);

        atomicConfidence.store(
            0.0f,
            std::memory_order_relaxed);


        noteTracker.processVoiceState(false);


        if (noteTracker.shouldNoteOff())
        {
            if (midiGenerator.hasActiveNote())
            {
                midiGenerator.noteOff(
                    generatedMidi,
                    0);

                flushGeneratedMidi();
            }


            noteTracker.reset();


            detectedMidiNote.store(
                -1,
                std::memory_order_relaxed);
        }


        return;
    }


    noteTracker.processVoiceState(true);


    // =========================================================
    // PITCH DETECTION
    // =========================================================

    float frequency = 0.0f;

    float confidence = 0.0f;


    const bool pitchFound =
        pitchDetector.processBlock(
            monoData,
            numSamples,
            frequency,
            confidence);


    if (!pitchFound)
        return;


    // =========================================================
    // STORE PITCH INFORMATION
    // =========================================================

    detectedFrequency = frequency;

    pitchConfidence = confidence;


    atomicFrequency.store(
        frequency,
        std::memory_order_relaxed);


    atomicConfidence.store(
        confidence,
        std::memory_order_relaxed);


    // =========================================================
    // NOTE TRACKING
    // =========================================================

    const int note =
        noteTracker.processFrequency(
            frequency,
            confidence);


    // =========================================================
    // VELOCITY
    // =========================================================

    const float normalizedLevel =
        juce::jlimit(
            0.0f,
            1.0f,
            level * 8.0f);


    const float curvedLevel =
        std::sqrt(normalizedLevel);


    const float targetVelocity =
        juce::jmap(
            curvedLevel,
            0.0f,
            1.0f,
            20.0f,
            127.0f);


    constexpr float velocitySmoothing = 0.20f;


    lastVelocity =
        lastVelocity +
        (targetVelocity - lastVelocity) *
        velocitySmoothing;


    const float velocity =
        juce::jlimit(
            1.0f,
            127.0f,
            lastVelocity);


    // =========================================================
    // MIDI OUTPUT
    // =========================================================

    if (note >= 0)
    {
        const int detectedNote =
            noteTracker.getCurrentNote();


        const int processedNote =
            musicalProcessor.processNote(
                detectedNote);


        float outputVelocity =
            velocity;


        const int velocityMode =
            static_cast<int>(
                apvts.getRawParameterValue(
                    ParameterIDs::velocityMode)->load());


        const int fixedVelocity =
            static_cast<int>(
                apvts.getRawParameterValue(
                    ParameterIDs::fixedVelocity)->load());


        if (velocityMode == 1)
        {
            outputVelocity =
                static_cast<float>(
                    fixedVelocity);
        }


        const int midiMode =
            static_cast<int>(
                apvts.getRawParameterValue(
                    ParameterIDs::midiMode)->load());


        switch (midiMode)
        {
            case 0:
            {
                midiGenerator.processNote(
                    processedNote,
                    outputVelocity,
                    generatedMidi,
                    0);

                break;
            }


            case 1:
            {
                std::array<int,
                    ChordGenerator::maxChordNotes>
                    chordNotes {};


                const int chordNoteCount =
                    chordGenerator.generateChord(
                        processedNote,
                        chordNotes);


                midiGenerator.processChord(
                    chordNotes,
                    chordNoteCount,
                    outputVelocity,
                    generatedMidi,
                    0);

                break;
            }


            case 2:
            default:
            {
                midiGenerator.processNote(
                    processedNote,
                    outputVelocity,
                    generatedMidi,
                    0);

                break;
            }
        }


        flushGeneratedMidi();


        detectedMidiNote.store(
            note,
            std::memory_order_relaxed);
    }
    else
    {
        detectedMidiNote.store(
            -1,
            std::memory_order_relaxed);
    }
}

// =============================================================
// EDITOR
// =============================================================

bool
OfforVocalToMidiAudioProcessor::
hasEditor() const
{
    return true;
}


juce::AudioProcessorEditor*
OfforVocalToMidiAudioProcessor::
createEditor()
{
    return new OfforVocalToMidiAudioProcessorEditor(*this);
}


// =============================================================
// STATE
// =============================================================

void
OfforVocalToMidiAudioProcessor::
getStateInformation(
    juce::MemoryBlock& destData)
{
    const auto state = apvts.copyState();

    std::unique_ptr<juce::XmlElement> xml =
        state.createXml();

    copyXmlToBinary(
        *xml,
        destData);
}


void
OfforVocalToMidiAudioProcessor::
setStateInformation(
    const void* data,
    int sizeInBytes)
{
    if (data == nullptr ||
        sizeInBytes <= 0)
    {
        return;
    }


    std::unique_ptr<juce::XmlElement> xml =
        getXmlFromBinary(
            data,
            sizeInBytes);


    if (xml != nullptr &&
        xml->hasTagName(
            apvts.state.getType()))
    {
        apvts.replaceState(
            juce::ValueTree::fromXml(
                *xml));
    }


    // Make sure the restored values reach
    // the actual DSP components.
    applyParametersToDSP();

    applyPitchRangeToDSP();
}

// =============================================================
// UI DATA
// =============================================================

float
OfforVocalToMidiAudioProcessor::
getDetectedFrequency() const
{
    return atomicFrequency.load(
        std::memory_order_relaxed);
}


float
OfforVocalToMidiAudioProcessor::
getPitchConfidence() const
{
    return atomicConfidence.load(
        std::memory_order_relaxed);
}


float
OfforVocalToMidiAudioProcessor::
getInputLevel() const
{
    return atomicInputLevel.load(
        std::memory_order_relaxed);
}


int
OfforVocalToMidiAudioProcessor::
getDetectedMidiNote() const
{
    return detectedMidiNote.load(
        std::memory_order_relaxed);
}


bool
OfforVocalToMidiAudioProcessor::
isCurrentlyVoiced() const
{
    return voiced.load(
        std::memory_order_relaxed);
}


// =============================================================
// SETTINGS
// =============================================================



// =============================================================
// PLUGIN ENTRY POINT
// =============================================================

juce::AudioProcessor*
JUCE_CALLTYPE createPluginFilter()
{
    return new OfforVocalToMidiAudioProcessor();
}




// =============================================================
// VOICE / PITCH THRESHOLDS
// =============================================================

void OfforVocalToMidiAudioProcessor::setVoiceThreshold(
    float threshold)
{
    threshold =
        juce::jlimit(
            0.001f,
            0.100f,
            threshold);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::voiceThreshold))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(threshold));
    }
}


void OfforVocalToMidiAudioProcessor::setPitchThreshold(
    float threshold)
{
    threshold =
        juce::jlimit(
            0.01f,
            1.0f,
            threshold);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::pitchThreshold))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(threshold));
    }
}


float
OfforVocalToMidiAudioProcessor::getVoiceThreshold() const
{
    return apvts.getRawParameterValue(
        ParameterIDs::voiceThreshold)->load();
}


float
OfforVocalToMidiAudioProcessor::getPitchThreshold() const
{
    return apvts.getRawParameterValue(
        ParameterIDs::pitchThreshold)->load();
}


// =============================================================
// MIDI SETTINGS
// =============================================================

void OfforVocalToMidiAudioProcessor::setMidiChannel(
    int channel)
{
    channel =
        juce::jlimit(
            1,
            16,
            channel);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::midiChannel))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(channel)));
    }
}


void OfforVocalToMidiAudioProcessor::setVelocityMode(
    int mode)
{
    mode =
        juce::jlimit(
            0,
            1,
            mode);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::velocityMode))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(mode)));
    }
}


void OfforVocalToMidiAudioProcessor::setFixedVelocity(
    int velocity)
{
    velocity =
        juce::jlimit(
            1,
            127,
            velocity);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::fixedVelocity))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(velocity)));
    }
}


void OfforVocalToMidiAudioProcessor::setOctaveShift(
    int octaves)
{
    octaves =
        juce::jlimit(
            -3,
            3,
            octaves);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::octaveShift))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(octaves)));
    }
}


void OfforVocalToMidiAudioProcessor::setTranspose(
    int semitones)
{
    semitones =
        juce::jlimit(
            -12,
            12,
            semitones);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::transpose))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(semitones)));
    }
}


int
OfforVocalToMidiAudioProcessor::getMidiChannel() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::midiChannel)->load());
}


int
OfforVocalToMidiAudioProcessor::getVelocityMode() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::velocityMode)->load());
}


int
OfforVocalToMidiAudioProcessor::getFixedVelocity() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::fixedVelocity)->load());
}


int
OfforVocalToMidiAudioProcessor::getOctaveShift() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::octaveShift)->load());
}


int
OfforVocalToMidiAudioProcessor::getTranspose() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::transpose)->load());
}


// =============================================================
// MIDI GENERATION
// =============================================================

void OfforVocalToMidiAudioProcessor::setMidiMode(
    int mode)
{
    mode =
        juce::jlimit(
            0,
            2,
            mode);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::midiMode))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(mode)));
    }

    midiResetRequested.store(
        true,
        std::memory_order_release);
}


int
OfforVocalToMidiAudioProcessor::getMidiMode() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::midiMode)->load());
}


void OfforVocalToMidiAudioProcessor::setChordGenre(
    int genre)
{
    genre =
        juce::jlimit(
            0,
            9,
            genre);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::chordGenre))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(genre)));
    }
}


int
OfforVocalToMidiAudioProcessor::getChordGenre() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::chordGenre)->load());
}


void OfforVocalToMidiAudioProcessor::setChordType(
    int type)
{
    type =
        juce::jlimit(
            0,
            17,
            type);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::chordType))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(type)));
    }
}


int
OfforVocalToMidiAudioProcessor::getChordType() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::chordType)->load());
}


void OfforVocalToMidiAudioProcessor::setChordVoicing(
    int voicing)
{
    voicing =
        juce::jlimit(
            0,
            2,
            voicing);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::chordVoicing))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(voicing)));
    }
}


int
OfforVocalToMidiAudioProcessor::getChordVoicing() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::chordVoicing)->load());
}


void OfforVocalToMidiAudioProcessor::setChordOctave(
    int octave)
{
    octave =
        juce::jlimit(
            -3,
            3,
            octave);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::chordOctave))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(octave)));
    }
}


int
OfforVocalToMidiAudioProcessor::getChordOctave() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::chordOctave)->load());
}


// =============================================================
// MUSICAL FEATURES
// =============================================================

void OfforVocalToMidiAudioProcessor::setScaleLockEnabled(
    bool enabled)
{
    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::scaleLock))
    {
        parameter->setValueNotifyingHost(
            enabled ? 1.0f : 0.0f);
    }
}


bool
OfforVocalToMidiAudioProcessor::isScaleLockEnabled() const
{
    return apvts.getRawParameterValue(
        ParameterIDs::scaleLock)->load() > 0.5f;
}


void OfforVocalToMidiAudioProcessor::setMusicalKey(
    int key)
{
    key =
        juce::jlimit(
            0,
            11,
            key);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::musicalKey))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(key)));
    }
}


int
OfforVocalToMidiAudioProcessor::getMusicalKey() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::musicalKey)->load());
}


void OfforVocalToMidiAudioProcessor::setMusicalScale(
    int scale)
{
    scale =
        juce::jlimit(
            0,
            5,
            scale);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::musicalScale))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(scale)));
    }
}


int
OfforVocalToMidiAudioProcessor::getMusicalScale() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::musicalScale)->load());
}


// =============================================================
// PITCH / NOTE TRACKING
// =============================================================

void OfforVocalToMidiAudioProcessor::setNoteStability(
    int frames)
{
    frames =
        juce::jlimit(
            1,
            10,
            frames);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::noteStability))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(frames)));
    }
}


void OfforVocalToMidiAudioProcessor::setNoteTolerance(
    float semitones)
{
    semitones =
        juce::jlimit(
            0.05f,
            1.0f,
            semitones);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::noteTolerance))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(semitones));
    }
}


void OfforVocalToMidiAudioProcessor::setNoteOffDelay(
    float milliseconds)
{
    milliseconds =
        juce::jlimit(
            10.0f,
            500.0f,
            milliseconds);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::noteOffDelay))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(milliseconds));
    }
}


void OfforVocalToMidiAudioProcessor::setConfidenceThreshold(
    float threshold)
{
    threshold =
        juce::jlimit(
            0.30f,
            0.95f,
            threshold);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::confidenceThreshold))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(threshold));
    }
}


int
OfforVocalToMidiAudioProcessor::getNoteStability() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::noteStability)->load());
}


float
OfforVocalToMidiAudioProcessor::getNoteTolerance() const
{
    return apvts.getRawParameterValue(
        ParameterIDs::noteTolerance)->load();
}


float
OfforVocalToMidiAudioProcessor::getNoteOffDelay() const
{
    return apvts.getRawParameterValue(
        ParameterIDs::noteOffDelay)->load();
}


float
OfforVocalToMidiAudioProcessor::getConfidenceThreshold() const
{
    return apvts.getRawParameterValue(
        ParameterIDs::confidenceThreshold)->load();
}


// =============================================================
// PITCH RANGE
// =============================================================

void OfforVocalToMidiAudioProcessor::setPitchRangePreset(
    int preset)
{
    preset =
        juce::jlimit(
            0,
            5,
            preset);

    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::pitchRange))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                static_cast<float>(preset)));
    }
}


void OfforVocalToMidiAudioProcessor::setCustomPitchRange(
    float minFrequency,
    float maxFrequency)
{
    minFrequency =
        juce::jlimit(
            40.0f,
            1000.0f,
            minFrequency);

    maxFrequency =
        juce::jlimit(
            minFrequency + 20.0f,
            2000.0f,
            maxFrequency);


    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::minPitchFrequency))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                minFrequency));
    }


    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::maxPitchFrequency))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(
                maxFrequency));
    }


    if (auto* parameter =
            apvts.getParameter(
                ParameterIDs::pitchRange))
    {
        parameter->setValueNotifyingHost(
            parameter->convertTo0to1(5.0f));
    }
}


int
OfforVocalToMidiAudioProcessor::getPitchRangePreset() const
{
    return static_cast<int>(
        apvts.getRawParameterValue(
            ParameterIDs::pitchRange)->load());
}


float
OfforVocalToMidiAudioProcessor::getMinPitchFrequency() const
{
    return apvts.getRawParameterValue(
        ParameterIDs::minPitchFrequency)->load();
}


float
OfforVocalToMidiAudioProcessor::getMaxPitchFrequency() const
{
    return apvts.getRawParameterValue(
        ParameterIDs::maxPitchFrequency)->load();
}



// =============================================================
// LICENSING
// =============================================================

bool
OfforVocalToMidiAudioProcessor::isLicenseActivated() const
{
    return licenseManager.isActivated();
}


bool
OfforVocalToMidiAudioProcessor::isLicenseAllowed() const
{
    return licenseAllowed.load(
        std::memory_order_acquire);
}


int
OfforVocalToMidiAudioProcessor::getLicenseUsageCount() const
{
    return licenseManager.getUsageCount();
}


int
OfforVocalToMidiAudioProcessor::getLicenseFreeUsesRemaining() const
{
    return licenseManager.getFreeUsesRemaining();
}


int
OfforVocalToMidiAudioProcessor::getLicenseFreeUsesLimit() const
{
    return licenseManager.getServerFreeUsesLimit();
}


bool
OfforVocalToMidiAudioProcessor::activateLicense(
    const juce::String& licenseKey)
{
    const bool activated =
        licenseManager.activate(licenseKey);

    if (activated)
    {
        licenseAllowed.store(
            true,
            std::memory_order_release);
    }

    return activated;
}


const juce::String
OfforVocalToMidiAudioProcessor::getInstallationId() const
{
    return licenseManager.getInstallationId();
}