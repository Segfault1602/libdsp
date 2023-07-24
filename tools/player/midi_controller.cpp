#include "midi_controller.h"

#include <cassert>
#include <vector>

uint16_t GetPitchBendValue(const MidiMessage& message)
{
    return (message.byte2 << 7) | message.byte1;
}

bool MidiController::Init(uint8_t port)
{
    try
    {
        midi_in_ = std::make_unique<RtMidiIn>();
    }
    catch (RtMidiError& error)
    {
        error.printMessage();
        return false;
    }

    if (port >= midi_in_->getPortCount())
    {
        printf("Invalid port number!\n");
        return false;
    }

    for (size_t i = 0; i < midi_in_->getPortCount(); ++i)
    {
        printf("Port %zu: %s %s\n", i, midi_in_->getPortName(i).c_str(), (port == i) ? "(selected)" : "");
    }

    midi_in_->openPort(port);
    return true;
}

bool MidiController::GetMidiMessage(MidiMessage& message)
{
    if (midi_in_ == nullptr)
    {
        return false;
    }

    std::vector<unsigned char> data;
    try
    {
        midi_in_->getMessage(&data);
        if (data.size() < 2)
        {
            // This means there is no more midi message in the queue or that the message is invalid.
            return false;
        }

        uint8_t type = data[0] & 0xF0;
        switch (type)
        {
        case 0x80:
            message.type = MidiType::NoteOff;
            break;
        case 0x90:
            message.type = MidiType::NoteOn;
            break;
        case 0xB0:
            message.type = MidiType::ControllerChange;
            break;
        case 0xD0:
            message.type = MidiType::ChannelAftertouch;
            break;
        case 0xE0:
            message.type = MidiType::PitchBend;
            break;
        default:
            message.type = MidiType::Unknown;
            break;
        }

        message.channel = data[0] & 0x0F;

        if (data.size() == 2)
        {
            message.byte1 = data[1] & 0x7F;
            message.byte2 = 0;
        }
        else if (data.size() == 3)
        {
            message.byte1 = data[1] & 0x7F;
            message.byte2 = data[2] & 0x7F;
        }
    }
    catch (RtMidiError& error)
    {
        error.printMessage();
        return false;
    }

    return true;
}