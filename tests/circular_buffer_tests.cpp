#include "gtest/gtest.h"

#include "circular_buffer.h"

TEST(CircularBufferTests, Init)
{
    const size_t max_size = 10;
    dsp::Buffer<max_size> buf;

    ASSERT_TRUE(buf.IsEmpty());
    ASSERT_EQ(buf.Count(), 0);
    ASSERT_FALSE(buf.IsFull());
    ASSERT_EQ(buf.Size(), max_size);
}

TEST(CircularBufferTests, CheckCount)
{
    const size_t max_size = 10;
    dsp::Buffer<max_size> buf;

    for (size_t i = 0; i < buf.Size() - 1; ++i)
    {
        buf.Write(i);
        ASSERT_FALSE(buf.IsFull());
        ASSERT_FALSE(buf.IsEmpty());
    }

    buf.Write(max_size);
    ASSERT_TRUE(buf.IsFull());
    ASSERT_FALSE(buf.IsEmpty());
    ASSERT_EQ(buf.Count(), buf.Size());
}

TEST(CircularBufferTests, CheckWrite)
{
    const size_t max_size = 10;
    dsp::Buffer<max_size> buf;

    float test_values[max_size] = {0};
    for (size_t i = 0; i < max_size; ++i)
    {
        test_values[i] = i;
    }

    for (size_t i = 0; i < buf.Size(); ++i)
    {
        buf.Write(test_values[i]);
    }

    ASSERT_EQ(buf.Peek(), test_values[0]);

    for (size_t i = 0; i < buf.Size(); ++i)
    {
        ASSERT_EQ(buf[i], test_values[i]);
    }
}

TEST(CircularBufferTests, CheckRead)
{
    const size_t max_size = 10;
    dsp::Buffer<max_size> buf;

    float test_values[max_size] = {0};
    for (size_t i = 0; i < max_size; ++i)
    {
        test_values[i] = i;
    }

    for (size_t i = 0; i < buf.Size(); ++i)
    {
        buf.Write(test_values[i]);
    }

    size_t i = 0;
    while (buf.Count() > 0)
    {
        auto preCount = buf.Count();
        ASSERT_EQ(buf.Read(), test_values[i]);
        ASSERT_EQ(preCount - 1, buf.Count());
        ++i;
    }
}

TEST(CircularBufferTests, CheckOverwrite)
{
    const size_t max_size = 10;
    dsp::Buffer<max_size> buf;

    float test_values[max_size] = {0};
    for (size_t i = 0; i < max_size; ++i)
    {
        test_values[i] = i;
    }

    for (size_t i = 0; i < buf.Size(); ++i)
    {
        buf.Write(test_values[i]);
    }


    const float offset = max_size*10;
    for (size_t i = 0; i < buf.Size() - 1; ++i)
    {
        buf.Write(offset + i);
        ASSERT_EQ(buf.Peek(), test_values[i+1]);
    }
}