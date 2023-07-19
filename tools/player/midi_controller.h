#pragma once

#include <RtMidi.h>
#include <memory>

enum class MidiType : uint8_t
{
    NoteOff = 0x80,
    NoteOn = 0x90,
    ControllerChange = 0xB0,
    ChannelAftertouch = 0xD0,
    PitchBend = 0xE0,
    Unknown = 0x00,
};

struct MidiMessage
{
    MidiType type = MidiType::Unknown;
    uint8_t channel = 0;
    uint8_t byte1 = 0;
    uint8_t byte2 = 0;
};

uint16_t GetPitchBendValue(const MidiMessage& message);

class MidiController
{
  public:
    MidiController() = default;
    ~MidiController() = default;

    bool Init(uint8_t port);

    bool GetMidiMessage(MidiMessage& message);

  private:
    std::unique_ptr<RtMidiIn> midi_in_;
};
