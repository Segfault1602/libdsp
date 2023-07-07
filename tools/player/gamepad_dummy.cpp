#include "gamepad.h"

class Gamepad::Impl
{
  public:
    Impl()
    {
    }

    ~Impl()
    {
    }

    bool Poll(GamepadState& state)
    {
        return false;
    }
};

Gamepad::Gamepad() : impl_(std::make_unique<Impl>())
{
}

Gamepad::~Gamepad() = default;

bool Gamepad::Poll(GamepadState& state)
{
    return impl_->Poll(state);
}