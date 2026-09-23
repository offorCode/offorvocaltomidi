#include "PianoKeyPage.h"

// ==========================================================
// PIANO KEYBOARD
// ==========================================================

PianoKeyboardComponent::PianoKeyboardComponent()
{
}

void PianoKeyboardComponent::setMidiNote(int midiNote)
{
    if (midiNote < firstMidiNote ||
        midiNote > lastMidiNote)
    {
        midiNote = -1;
    }

    if (activeMidiNote != midiNote)
    {
        activeMidiNote = midiNote;
        repaint();
    }
}

bool PianoKeyboardComponent::isBlackKey(int midiNote) const
{
    const int note = midiNote % 12;

    return note == 1 ||
           note == 3 ||
           note == 6 ||
           note == 8 ||
           note == 10;
}

juce::String PianoKeyboardComponent::getNoteName(
    int midiNote) const
{
    static const char* names[] =
    {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    const int note = midiNote % 12;
    const int octave = (midiNote / 12) - 1;

    return juce::String(names[note]) +
           juce::String(octave);
}

void PianoKeyboardComponent::paint(
    juce::Graphics& g)
{
    g.fillAll(
        juce::Colour(0xff111216));

    // ------------------------------------------------------
    // Count white keys
    // ------------------------------------------------------

    int totalWhiteKeys = 0;

    for (int note = firstMidiNote;
         note <= lastMidiNote;
         ++note)
    {
        if (!isBlackKey(note))
            ++totalWhiteKeys;
    }

    if (totalWhiteKeys <= 0)
        return;

    const float whiteWidth =
        static_cast<float>(getWidth()) /
        static_cast<float>(totalWhiteKeys);

    const float height =
        static_cast<float>(getHeight());

    // ------------------------------------------------------
    // White keys
    // ------------------------------------------------------

    int whiteIndex = 0;

    for (int midiNote = firstMidiNote;
         midiNote <= lastMidiNote;
         ++midiNote)
    {
        if (isBlackKey(midiNote))
            continue;

        const float x =
            whiteIndex * whiteWidth;

        const bool active =
            activeMidiNote == midiNote;

        g.setColour(
            active
                ? juce::Colour(0xffdf513e)
                : juce::Colour(0xfff2f2f4));

        g.fillRect(
            x + 1.0f,
            0.0f,
            whiteWidth - 2.0f,
            height);

        g.setColour(
            juce::Colour(0xff30323a));

        g.drawRect(
            x,
            0.0f,
            whiteWidth,
            height,
            1.0f);

        // Note name
        if (whiteWidth >= 35.0f)
        {
            g.setColour(
                active
                    ? juce::Colours::white
                    : juce::Colour(0xff30323a));

            g.setFont(
                juce::FontOptions(11.0f));

            g.drawText(
                getNoteName(midiNote),
                static_cast<int>(x),
                getHeight() - 30,
                static_cast<int>(whiteWidth),
                20,
                juce::Justification::centred);
        }

        ++whiteIndex;
    }

    // ------------------------------------------------------
    // Black keys
    // ------------------------------------------------------

    whiteIndex = 0;

    for (int midiNote = firstMidiNote;
         midiNote <= lastMidiNote;
         ++midiNote)
    {
        if (isBlackKey(midiNote))
            continue;

        const int blackNote = midiNote + 1;

        if (blackNote <= lastMidiNote &&
            isBlackKey(blackNote))
        {
            const float blackWidth =
                whiteWidth * 0.62f;

            const float blackHeight =
                height * 0.60f;

            const float x =
                (whiteIndex + 1) * whiteWidth -
                blackWidth * 0.5f;

            const bool active =
                activeMidiNote == blackNote;

            g.setColour(
                active
                    ? juce::Colour(0xffdf513e)
                    : juce::Colour(0xff17181d));

            g.fillRoundedRectangle(
                x,
                0.0f,
                blackWidth,
                blackHeight,
                4.0f);

            g.setColour(
                juce::Colour(0xff30323a));

            g.drawRoundedRectangle(
                x,
                0.0f,
                blackWidth,
                blackHeight,
                4.0f,
                1.0f);
        }

        ++whiteIndex;
    }
}


// ==========================================================
// PIANO PAGE
// ==========================================================

PianoKeyPage::PianoKeyPage()
{
    // ------------------------------------------------------
    // Back button
    // ------------------------------------------------------

    backButton.setButtonText("<  BACK");

    backButton.setColour(
        juce::TextButton::buttonColourId,
        juce::Colour(0xff1d1f25));

    backButton.setColour(
        juce::TextButton::textColourOffId,
        juce::Colour(0xfff2f2f4));

    backButton.onClick = [this]()
    {
        if (onBack)
            onBack();
    };

    addAndMakeVisible(backButton);

    // ------------------------------------------------------
    // Title
    // ------------------------------------------------------

    titleLabel.setText(
        "PIANO KEYBOARD",
        juce::dontSendNotification);

    titleLabel.setFont(
        juce::FontOptions(22.0f)
            .withStyle("Bold"));

    titleLabel.setColour(
        juce::Label::textColourId,
        juce::Colour(0xfff2f2f4));

    titleLabel.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(titleLabel);

    // ------------------------------------------------------
    // Detected note
    // ------------------------------------------------------

    noteLabel.setText(
        "--",
        juce::dontSendNotification);

    noteLabel.setFont(
        juce::FontOptions(36.0f)
            .withStyle("Bold"));

    noteLabel.setColour(
        juce::Label::textColourId,
        juce::Colour(0xffdf513e));

    noteLabel.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(noteLabel);

    addAndMakeVisible(keyboard);

    startTimerHz(30);
}

void PianoKeyPage::setMidiNote(int midiNote)
{
    currentMidiNote = midiNote;

    keyboard.setMidiNote(midiNote);

    if (midiNote >= 0)
    {
        static const char* names[] =
        {
            "C", "C#", "D", "D#", "E", "F",
            "F#", "G", "G#", "A", "A#", "B"
        };

        const int note =
            midiNote % 12;

        const int octave =
            (midiNote / 12) - 1;

        noteLabel.setText(
            juce::String(names[note]) +
            juce::String(octave),
            juce::dontSendNotification);
    }
    else
    {
        noteLabel.setText(
            "--",
            juce::dontSendNotification);
    }
}

void PianoKeyPage::paint(juce::Graphics& g)
{
    g.fillAll(
        juce::Colour(0xff111216));

    // Header
    g.setColour(
        juce::Colour(0xff1d1f25));

    g.fillRoundedRectangle(
        20.0f,
        20.0f,
        static_cast<float>(getWidth() - 40),
        85.0f,
        10.0f);

    // Keyboard panel
    g.setColour(
        juce::Colour(0xff1d1f25));

    g.fillRoundedRectangle(
        20.0f,
        125.0f,
        static_cast<float>(getWidth() - 40),
        static_cast<float>(getHeight() - 145),
        10.0f);
}

void PianoKeyPage::resized()
{
    const int width = getWidth();
    const int height = getHeight();

    backButton.setBounds(
        35,
        40,
        100,
        35);

    titleLabel.setBounds(
        150,
        35,
        width - 300,
        35);

    noteLabel.setBounds(
        width - 145,
        35,
        110,
        35);

    keyboard.setBounds(
        35,
        160,
        width - 70,
        height - 195);
}

void PianoKeyPage::timerCallback()
{
    // Kept ready for future real-time information.
}