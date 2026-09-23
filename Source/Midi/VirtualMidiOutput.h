#pragma once

#include <JuceHeader.h>
#include "teVirtualMIDI.h"

class VirtualMidiOutput
    : private juce::Thread
{
public:
    VirtualMidiOutput();
    ~VirtualMidiOutput() override;

    bool open(const juce::String& portName);
    void close();

    bool sendNoteOn(int channel, int note, int velocity);
    bool sendNoteOff(int channel, int note);

    bool isOpen() const;

private:
    // ==========================================================
    // MIDI EVENT
    // ==========================================================

    struct MidiEvent
    {
        BYTE data[3] {};
        DWORD length = 0;
    };

    // ==========================================================
    // THREAD
    // ==========================================================

    void run() override;

    // ==========================================================
    // QUEUE
    //
    // Single producer:
    //     OFFOR audio thread
    //
    // Single consumer:
    //     Virtual MIDI worker thread
    //
    // No mutex is used by sendNoteOn/sendNoteOff.
    // ==========================================================

    static constexpr int queueSize = 1024;

    juce::AbstractFifo midiFifo { queueSize };

    std::array<MidiEvent, queueSize> midiQueue {};

    // ==========================================================
    // VIRTUAL MIDI PORT
    //
    // Only the worker thread accesses the port after creation.
    // ==========================================================

    LPVM_MIDI_PORT midiPort = nullptr;

    std::atomic<bool> portOpen { false };

    // ==========================================================
    // LIFETIME
    // ==========================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VirtualMidiOutput)
};