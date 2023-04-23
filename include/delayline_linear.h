#ifndef STK_DELAYL_H
#define STK_DELAYL_H

#include <cassert>

namespace dsp
{

/***************************************************/
/*! \class DelayL
    \brief STK linear interpolating delay line class.

    This class implements a fractional-length digital delay-line using
    first-order linear interpolation.  If the delay and maximum length
    are not specified during instantiation, a fixed maximum length of
    4095 and a delay of zero is set.

    Linear interpolation is an efficient technique for achieving
    fractional delay lengths, though it does introduce high-frequency
    signal attenuation to varying degrees depending on the fractional
    delay setting.  The use of higher order Lagrange interpolators can
    typically improve (minimize) this attenuation characteristic.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
    https://github.com/thestk/stk/blob/master/include/DelayL.h

    Modified and adapted by alexstonge
*/
/***************************************************/

template <size_t MAX_DELAY> class DelayL
{
  public:
    //! Default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
    /*!
      An StkError will be thrown if the delay parameter is less than
      zero, the maximum delay parameter is less than one, or the delay
      parameter is greater than the maxDelay value.
     */
    DelayL(float delay = 0.0)
    {
        memset(inputs_, 0, sizeof(inputs_));
        if (delay > MAX_DELAY)
        {
            delay = MAX_DELAY;
        }

        inPoint_ = 0;
        this->SetDelay(delay, false);
        doNextOut_ = true;
    }

    //! Class destructor.
    ~DelayL() = default;

    //! Get the maximum delay-line length.
    unsigned long getMaximumDelay(void)
    {
        return MAX_DELAY - 1;
    };

    //! Set the delay-line length.
    /*!
      The valid range for \e delay is from 0 to the maximum delay-line length.
    */
    void SetDelay(float delay, bool unsused = false)
    {
        if (delay + 1 > MAX_DELAY)
        { // The value is too big.
            assert(false);
            return;
        }

        if (delay < 0)
        {
            assert(false);
            return;
        }

        float outPointer = inPoint_ - delay; // read chases write
        delay_ = delay;

        while (outPointer < 0)
            outPointer += MAX_DELAY; // modulo maximum length

        outPoint_ = (long)outPointer; // integer part

        alpha_ = outPointer - outPoint_; // fractional part
        omAlpha_ = (float)1.0 - alpha_;

        if (outPoint_ == MAX_DELAY)
            outPoint_ = 0;
        doNextOut_ = true;
    }

    //! Return the current delay-line length.
    float getDelay(void) const
    {
        return delay_;
    };

    //! Return the value at \e tapDelay samples from the delay-line input.
    /*!
      The tap point is determined modulo the delay-line length and is
      relative to the last input value (i.e., a tapDelay of zero returns
      the last input value).
    */
    float TapOut(unsigned long tapDelay)
    {
        long tap = inPoint_ - tapDelay - 1;
        while (tap < 0) // Check for wraparound.
            tap += MAX_DELAY;

        return inputs_[tap];
    }

    //! Return the value which will be output by the next call to tick().
    /*!
      This method is valid only for delay settings greater than zero!
     */
    float nextOut(void)
    {
        if (doNextOut_)
        {
            // First 1/2 of interpolation
            nextOutput_ = inputs_[outPoint_] * omAlpha_;
            // Second 1/2 of interpolation
            if (outPoint_ + 1 < MAX_DELAY)
                nextOutput_ += inputs_[outPoint_ + 1] * alpha_;
            else
                nextOutput_ += inputs_[0] * alpha_;
            doNextOut_ = false;
        }

        return nextOutput_;
    }

    //! Input one sample to the filter and return one output.
    float Tick(float input)
    {
        inputs_[inPoint_++] = input;

        // Increment input pointer modulo length.
        if (inPoint_ == MAX_DELAY)
            inPoint_ = 0;

        lastFrame_ = nextOut();
        doNextOut_ = true;

        // Increment output pointer modulo length.
        if (++outPoint_ == MAX_DELAY)
            outPoint_ = 0;

        return lastFrame_;
    }

  protected:
    unsigned long inPoint_;
    unsigned long outPoint_;
    float delay_;
    float alpha_;
    float omAlpha_;
    float nextOutput_;
    bool doNextOut_;

    float inputs_[MAX_DELAY];
    float lastFrame_ = 0.f;
};

} // namespace dsp

#endif