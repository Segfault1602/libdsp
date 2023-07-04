#include "midi_controller.h"

#include <cassert>
#include <vector>

bool MidiController::Init()
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

    for (size_t i = 0; i < midi_in_->getPortCount(); ++i)
    {
        printf("Port %zu: %s\n", i, midi_in_->getPortName(i).c_str());
    }

    midi_in_->openPort(0);
    return true;
}

bool MidiController::GetMessage(MidiMessage& message)
{
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
        default:
            message.type = MidiType::Unknown;
            break;
        }

        message.channel = data[0] & 0x0F;

        if (message.type == MidiType::NoteOff || message.type == MidiType::NoteOn ||
            message.type == MidiType::ControllerChange)
        {
            assert(data.size() == 3);
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