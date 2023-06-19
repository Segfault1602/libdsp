#pragma once

#include <memory>

struct GamepadState
{
    bool a = false;
    bool b = false;
    bool x = false;
    bool y = false;

    float thumb_left_x = 0.0f;
    float thumb_left_y = 0.0f;
    float thumb_left_magnitude = 0.0f;

    float thumb_right_x = 0.0f;
    float thumb_right_y = 0.0f;
    float thumb_right_magnitude = 0.0f;
};

class Gamepad
{
  public:
    Gamepad();
    ~Gamepad();

    bool Poll(GamepadState& state);

  private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};