//  ============================================================
//  stream with no output
//  ============================================================
//  created by Manfred Sorgo

#ifndef NULLSTREAM_H
#define NULLSTREAM_H

#include <ostream>
#include <streambuf>

//  NullBuffer and NullStream are used to prevent from output
//  thanks to ChatGPT for the basics of this
class NullBuffer : public std::streambuf
{
public:
    inline int overflow(int c)
    {
        return c; // Do nothing with the character
    }
};

class NullStream : public std::ostream
{
public:
    inline NullStream() : std::ostream(&mBuffer) {}
private:
    NullBuffer mBuffer;
};

#endif // _H
