#include "VirtualMidiOutput.h"

#include <iostream>
#include <windows.h>

// ==========================================================
// CONSTRUCTOR
// ==========================================================

VirtualMidiOutput::VirtualMidiOutput()
    : juce::Thread("OFFOR Virtual MIDI")
{
}

// ==========================================================
// DESTRUCTOR
// ==========================================================

VirtualMidiOutput::~VirtualMidiOutput()
{
    close();
}

// ==========================================================
// OPEN
// ==========================================================

bool VirtualMidiOutput::open(const juce::String& portName)
{
    close();

    if (portName.isEmpty())
    {
        std::cerr
            << "[OFFOR MIDI] Cannot open MIDI port: empty name."
            << std::endl;

        return false;
    }

    const std::wstring wideName =
        portName.toWideCharPointer();

    std::cout
        << "[OFFOR MIDI] Creating virtual MIDI port: "
        << portName.toStdString()
        << std::endl;

    midiPort =
        virtualMIDICreatePortEx2(
            wideName.c_str(),
            nullptr,
            0,
            1024,
            TE_VM_FLAGS_PARSE_RX);

    if (midiPort == nullptr)
    {
        const DWORD errorCode = GetLastError();

        std::cerr
            << "[OFFOR MIDI] virtualMIDICreatePortEx2 FAILED. "
            << "GetLastError = "
            << errorCode
            << std::endl;

        return false;
    }

    std::cout
        << "[OFFOR MIDI] Virtual MIDI port created successfully."
        << std::endl;

    // Clear any stale events from a previous session.
    midiFifo.reset();

    portOpen.store(
        true,
        std::memory_order_release);

    // Start the worker thread.
    startThread();

    std::cout
        << "[OFFOR MIDI] MIDI worker thread started."
        << std::endl;

    return true;
}

// ==========================================================
// CLOSE
// ==========================================================

void VirtualMidiOutput::close()
{
    // ========================================================
    // STOP ACCEPTING NEW EVENTS
    // ========================================================

    portOpen.store(
        false,
        std::memory_order_release);

    // ========================================================
    // STOP WORKER
    // ========================================================

    if (isThreadRunning())
    {
        std::cout
            << "[OFFOR MIDI] Stopping MIDI worker thread..."
            << std::endl;

        signalThreadShouldExit();

        // IMPORTANT:
        // Wait until the worker has completely exited.
        //
        // The MIDI port must NOT be closed while the worker
        // could still call virtualMIDISendData().
        stopThread(-1);

        std::cout
            << "[OFFOR MIDI] MIDI worker thread stopped."
            << std::endl;
    }

    // ========================================================
    // NOW IT IS SAFE TO CLOSE THE PORT
    // ========================================================

    if (midiPort != nullptr)
    {
        std::cout
            << "[OFFOR MIDI] Closing virtual MIDI port..."
            << std::endl;

        virtualMIDIClosePort(
            midiPort);

        midiPort = nullptr;

        std::cout
            << "[OFFOR MIDI] Virtual MIDI port closed."
            << std::endl;
    }

    // ========================================================
    // CLEAR QUEUE
    // ========================================================

    midiFifo.reset();
}

// ==========================================================
// SEND NOTE ON
//
// IMPORTANT:
// This function can be called from processBlock().
//
// It DOES NOT call virtualMIDISendData().
// It only places the MIDI event into the lock-free queue.
// ==========================================================

bool VirtualMidiOutput::sendNoteOn(
    int channel,
    int note,
    int velocity)
{
    if (!portOpen.load(std::memory_order_acquire))
        return false;

    channel =
        juce::jlimit(
            1,
            16,
            channel);

    note =
        juce::jlimit(
            0,
            127,
            note);

    velocity =
        juce::jlimit(
            1,
            127,
            velocity);

    MidiEvent event;

    event.data[0] =
        static_cast<BYTE>(
            0x90 | (channel - 1));

    event.data[1] =
        static_cast<BYTE>(note);

    event.data[2] =
        static_cast<BYTE>(velocity);

    event.length = 3;

    // ======================================================
    // LOCK-FREE QUEUE WRITE
    // ======================================================

    int start1 = 0;
    int size1 = 0;
    int start2 = 0;
    int size2 = 0;

    midiFifo.prepareToWrite(
        1,
        start1,
        size1,
        start2,
        size2);

    if (size1 <= 0)
    {
        // Queue is full.
        return false;
    }

    midiQueue[
        static_cast<size_t>(start1)] = event;

    midiFifo.finishedWrite(1);

    return true;
}

// ==========================================================
// SEND NOTE OFF
//
// IMPORTANT:
// This function can be called from processBlock().
//
// It DOES NOT call virtualMIDISendData().
// ==========================================================

bool VirtualMidiOutput::sendNoteOff(
    int channel,
    int note)
{
    if (!portOpen.load(std::memory_order_acquire))
        return false;

    channel =
        juce::jlimit(
            1,
            16,
            channel);

    note =
        juce::jlimit(
            0,
            127,
            note);

    MidiEvent event;

    event.data[0] =
        static_cast<BYTE>(
            0x80 | (channel - 1));

    event.data[1] =
        static_cast<BYTE>(note);

    event.data[2] = 0;

    event.length = 3;

    // ======================================================
    // LOCK-FREE QUEUE WRITE
    // ======================================================

    int start1 = 0;
    int size1 = 0;
    int start2 = 0;
    int size2 = 0;

    midiFifo.prepareToWrite(
        1,
        start1,
        size1,
        start2,
        size2);

    if (size1 <= 0)
    {
        // Queue is full.
        return false;
    }

    midiQueue[
        static_cast<size_t>(start1)] = event;

    midiFifo.finishedWrite(1);

    return true;
}

// ==========================================================
// IS OPEN
// ==========================================================

bool VirtualMidiOutput::isOpen() const
{
    return portOpen.load(
        std::memory_order_acquire);
}

// ==========================================================
// WORKER THREAD
//
// ONLY THIS THREAD calls:
//
//     virtualMIDISendData()
//
// This keeps the Windows virtual MIDI API away from
// processBlock().
// ==========================================================

void VirtualMidiOutput::run()
{
    while (!threadShouldExit())
    {
        int start1 = 0;
        int size1 = 0;
        int start2 = 0;
        int size2 = 0;

        midiFifo.prepareToRead(
            1,
            start1,
            size1,
            start2,
            size2);

        if (size1 <= 0)
        {
            midiFifo.finishedRead(0);

            wait(1);

            continue;
        }

        MidiEvent event =
            midiQueue[
                static_cast<size_t>(start1)];

        midiFifo.finishedRead(1);

        if (midiPort != nullptr)
        {
            const BOOL result =
                virtualMIDISendData(
                    midiPort,
                    event.data,
                    event.length);

            if (result == FALSE)
            {
                const DWORD errorCode =
                    GetLastError();

                std::cerr
                    << "[OFFOR MIDI] "
                    << "virtualMIDISendData FAILED. "
                    << "GetLastError = "
                    << errorCode
                    << std::endl;
            }
        }
    }
}