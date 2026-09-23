#include "NoteDetectionComponent.h"

// =============================================================
// CONSTRUCTOR
// =============================================================

NoteDetectionComponent::NoteDetectionComponent()
{
    setOpaque(false);
}

// =============================================================
// MIDI NOTE
// =============================================================

void NoteDetectionComponent::setMidiNote(int midiNote)
{
    if (midiNote < 0 || midiNote > 127)
        midiNote = -1;

    if (activeMidiNote != midiNote)
    {
        activeMidiNote = midiNote;
        repaint();
    }
}

// =============================================================
// VOICED STATE
// =============================================================

void NoteDetectionComponent::setVoiced(bool isVoiced)
{
    if (voiced != isVoiced)
    {
        voiced = isVoiced;
        repaint();
    }
}

// =============================================================
// PITCH CLASS
// =============================================================

int NoteDetectionComponent::getPitchClass() const
{
    if (activeMidiNote < 0)
        return -1;

    return activeMidiNote % 12;
}

// =============================================================
// OCTAVE
// =============================================================

int NoteDetectionComponent::getOctave() const
{
    if (activeMidiNote < 0)
        return -1;

    return (activeMidiNote / 12) - 1;
}

// =============================================================
// PAINT
// =============================================================

void NoteDetectionComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // =========================================================
    // TITLE
    // =========================================================

    auto titleArea = bounds.removeFromTop(28.0f);

    g.setColour(textColour);

    g.setFont(
        juce::Font(
            juce::FontOptions{}
                .withName("Poppins")
                .withHeight(14.0f)
                .withStyle("Bold")));

    g.drawText(
        "NOTE DETECTION",
        titleArea,
        juce::Justification::centred,
        false);

    // =========================================================
    // MIDI INFORMATION AREA
    // =========================================================

    auto midiArea = bounds.removeFromBottom(42.0f);

    // =========================================================
    // CIRCULAR NOTE WHEEL
    // =========================================================

    auto wheelArea = bounds.reduced(5.0f);

    const float centreX =
        wheelArea.getCentreX();

    const float centreY =
        wheelArea.getCentreY();

    // Make the wheel circular even if the component is wider
    // than it is tall.
    const float radius =
        juce::jmin(
            wheelArea.getWidth(),
            wheelArea.getHeight()) * 0.38f;

    // =========================================================
    // NOTE WHEEL RING
    // =========================================================

    g.setColour(
        juce::Colour(0xff30323a));

    g.drawEllipse(
        juce::Rectangle<float>(
            centreX - radius,
            centreY - radius,
            radius * 2.0f,
            radius * 2.0f),
        1.0f);

    // =========================================================
    // ACTIVE NOTE
    // =========================================================

    const int activePitchClass =
        getPitchClass();

    const bool hasActiveNote =
        voiced &&
        activeMidiNote >= 0 &&
        activePitchClass >= 0;

    // =========================================================
    // NOTE POSITIONS
    //
    // C starts at the top.
    //
    // Clockwise:
    //
    // C
    // C#
    // D
    // D#
    // E
    // F
    // F#
    // G
    // G#
    // A
    // A#
    // B
    // =========================================================

    constexpr double startAngle =
        -juce::MathConstants<double>::halfPi;

    constexpr double angleStep =
        juce::MathConstants<double>::twoPi / 12.0;

    for (int i = 0; i < numberOfNotes; ++i)
    {
        const double angle =
            startAngle +
            angleStep * static_cast<double>(i);

        const float x =
            centreX +
            radius * static_cast<float>(
                std::cos(angle));

        const float y =
            centreY +
            radius * static_cast<float>(
                std::sin(angle));

        const bool active =
            hasActiveNote &&
            activePitchClass == i;

        // -----------------------------------------------------
        // CIRCLE SIZE
        // -----------------------------------------------------

        const float diameter =
            active ? 34.0f : 27.0f;

        juce::Rectangle<float> circleBounds(
            x - diameter * 0.5f,
            y - diameter * 0.5f,
            diameter,
            diameter);

        // -----------------------------------------------------
        // ACTIVE NOTE
        // -----------------------------------------------------

        if (active)
        {
            // Outer subtle ring
            g.setColour(
                accentColour.withAlpha(0.15f));

            g.fillEllipse(
                circleBounds.expanded(7.0f));

            // Orange circle
            g.setColour(accentColour);

            g.fillEllipse(circleBounds);

            // Inner highlight ring
            g.setColour(
                juce::Colours::white.withAlpha(0.30f));

            g.drawEllipse(
                circleBounds.reduced(2.0f),
                1.0f);

            // Note text
            g.setColour(
                juce::Colours::white);

            g.setFont(
                juce::Font(
                    juce::FontOptions{}
                        .withName("Poppins")
                        .withHeight(11.0f)
                        .withStyle("Bold")));

            g.drawText(
                noteNames[i],
                circleBounds,
                juce::Justification::centred,
                false);
        }
        else
        {
            // -------------------------------------------------
            // NORMAL NOTE
            // -------------------------------------------------

            g.setColour(
                juce::Colour(0xff30323a));

            g.fillEllipse(circleBounds);

            g.setColour(mutedColour);

            g.drawEllipse(
                circleBounds,
                1.0f);

            g.setFont(
                juce::Font(
                    juce::FontOptions{}
                        .withName("Poppins")
                        .withHeight(10.0f)
                        .withStyle("Medium")));

            g.drawText(
                noteNames[i],
                circleBounds,
                juce::Justification::centred,
                false);
        }
    }

    // =========================================================
    // CENTER NOTE
    // =========================================================

    if (hasActiveNote)
    {
        const int octave =
            getOctave();

        const juce::String noteName =
            noteNames[activePitchClass];

        const juce::String fullNote =
            noteName +
            juce::String(octave);

        g.setColour(textColour);

        g.setFont(
            juce::Font(
                juce::FontOptions{}
                    .withName("Poppins")
                    .withHeight(25.0f)
                    .withStyle("Bold")));

        g.drawText(
            fullNote,
            juce::Rectangle<float>(
                centreX - 45.0f,
                centreY - 20.0f,
                90.0f,
                30.0f),
            juce::Justification::centred,
            false);
    }
    else
    {
        g.setColour(
            mutedColour.withAlpha(0.7f));

        g.setFont(
            juce::Font(
                juce::FontOptions{}
                    .withName("Poppins")
                    .withHeight(13.0f)
                    .withStyle("Medium")));

        g.drawText(
            "--",
            juce::Rectangle<float>(
                centreX - 30.0f,
                centreY - 15.0f,
                60.0f,
                30.0f),
            juce::Justification::centred,
            false);
    }

    // =========================================================
    // MIDI NOTE
    // =========================================================

    g.setFont(
        juce::Font(
            juce::FontOptions{}
                .withName("Poppins")
                .withHeight(11.0f)
                .withStyle("Medium")));

    if (hasActiveNote)
    {
        g.setColour(textColour);

        g.drawText(
            "MIDI NOTE " +
            juce::String(activeMidiNote),
            midiArea,
            juce::Justification::centred,
            false);
    }
    else
    {
        g.setColour(mutedColour);

        g.drawText(
            "MIDI NOTE --",
            midiArea,
            juce::Justification::centred,
            false);
    }
}

// =============================================================
// RESIZED
// =============================================================

void NoteDetectionComponent::resized()
{
}