#include "gamepad.h"

#include <Windows.h>
#include <Xinput.h>

#include <cmath>

void NormalizeJoystick(float x, float y, float deadzone, float& normalized_x, float& normalized_y,
                    float& normalized_magnitude)
{
    float magnitude = std::sqrt(x * x + y * y);

    if (magnitude > deadzone)
    {
        // clip the magnitude at its expected maximum value
        if (magnitude > 32767)
        {
            magnitude = 32767;
        }

        // adjust magnitude relative to the end of the dead zone
        magnitude -= deadzone;

        // optionalleft_y normalize the magnitude with respect to its expected range
        // giving a magnitude value of 0.0 to 1.0
        normalized_magnitude = magnitude / (32767 - deadzone);
    }
    else // if the controller is in the deadzone zero out the magnitude
    {
        magnitude = 0.0;
        normalized_magnitude = 0.0;
    }

    normalized_x = (x / magnitude) * normalized_magnitude;
    normalized_y = (y / magnitude) * normalized_magnitude;
}

float NormalizeTrigger(uint8_t trigger, uint8_t deadzone)
{
    float normalized_trigger = 0.0f;
    if (trigger > deadzone)
    {
        // adjust magnitude relative to the end of the dead zone
        trigger -= deadzone;

        // optionalleft_y normalize the magnitude with respect to its expected range
        // giving a magnitude value of 0.0 to 1.0
        normalized_trigger = static_cast<float>(trigger) / (255.f - static_cast<float>(deadzone));
    }

    return normalized_trigger;
}

class Gamepad::Impl
{
  public:
    Impl()
    {
        XInputEnable(TRUE);
    }

    ~Impl()
    {
        XInputEnable(FALSE);
    }

    bool Poll(GamepadState& state)
    {
        ZeroMemory(&state, sizeof(GamepadState));

        XINPUT_STATE xinput_state;
        ZeroMemory(&xinput_state, sizeof(XINPUT_STATE));
        DWORD result = XInputGetState(0, &xinput_state);
        if (result == ERROR_SUCCESS)
        {
            // Controller is connected
            if (xinput_state.dwPacketNumber != last_packet_number_)
            {
                last_packet_number_ = xinput_state.dwPacketNumber;

                state.a = xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A;
                state.b = xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B;
                state.x = xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X;
                state.y = xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y;

                NormalizeJoystick(xinput_state.Gamepad.sThumbLX, xinput_state.Gamepad.sThumbLY,
                               XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, state.thumb_left_x, state.thumb_left_y,
                               state.thumb_left_magnitude);

                NormalizeJoystick(xinput_state.Gamepad.sThumbRX, xinput_state.Gamepad.sThumbRY,
                               XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, state.thumb_right_x, state.thumb_right_y,
                               state.thumb_right_magnitude);

                state.left_trigger = NormalizeTrigger(xinput_state.Gamepad.bLeftTrigger, 10);

                state.right_trigger = NormalizeTrigger(xinput_state.Gamepad.bRightTrigger, 10);

                return true;
            }
        }
        else
        {
            printf("Controller is not connected!\n");
        }

        return false;
    }

  private:
    DWORD last_packet_number_ = 0;
};

Gamepad::Gamepad() : impl_(std::make_unique<Impl>())
{
}

Gamepad::~Gamepad() = default;

bool Gamepad::Poll(GamepadState& state)
{
    return impl_->Poll(state);
}