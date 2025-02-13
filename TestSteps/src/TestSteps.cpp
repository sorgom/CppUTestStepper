
#include <TestSteps/TestSteps.h>
#include <CppUTest/Utest.h>
#include <ciso646>
#include <cstring>
#include <iostream>

using std::memset;
using std::cout;

bool TestSteps::mDone = false;
TestSteps::StepLevelT TestSteps::mLevel = 0;
TestSteps::StepLevelT TestSteps::mShow = 0;

const char* const TestSteps::c__step = "STEP(";
const char* const TestSteps::c__pre = "PRE";
const char* const TestSteps::c__file = "file: ";
const char* const TestSteps::c__func = "func: ";
const char* const TestSteps::c__line = "line: ";
const char* const TestSteps::c__ErrTestSteps = "TestSteps error";
const char* const TestSteps::c__ErrStep = "TestSteps: STEP(n) n must be greater 0.";
const char* const TestSteps::c__ErrSubSteps = "TestSteps: SUBSTEPS error. Did you forget ENDSTEPS?";
const char* const TestSteps::c__ErrEndSteps = "TestSteps: ENDSTEPS error. Did you forget SUBSTEPS?";

TestSteps::Trace TestSteps::mTrace[cNmuLevels] = {0};
NullStream TestSteps::mNullStream;


void TestSteps::clear()
{
    mShow = 0;
    enterLevel(0, 0, 0, 0);
}

void TestSteps::show(const StepLevelT trcLevel)
{
    mShow = trcLevel;
    if (isShowing())
    {
        cout << '\n';
    }
}

void TestSteps::clearLevel(const StepLevelT level)
{
    for (StepLevelT l = level; l < cNmuLevels; ++l)
    {
        memset(&mTrace[l], 0, sizeof(Trace));
        mTrace[l].step = cStepNone;
    }
    if (level == 0)
    {
        mDone = false;
    }
}

void TestSteps::enterLevel(
    const StepLevelT level,
    const char* const file,
    const TestLineT line,
    const char* const func
)
{
    chk(level < cNmuLevels, c__ErrSubSteps);
    clearLevel(level);
    Trace& trc = mTrace[level];
    trc.file = file;
    trc.line = line;
    trc.func = func;
    trc.step = cStepPre;
    mLevel = level;
}

void TestSteps::step(
    const TestStepT step,
    const char* const file,
    const TestLineT line,
    const char* const func,
    const bool nl
)
{
    UtestShell::getCurrent()->setLineNumber(line);
    Trace& trc = mTrace[mLevel];
    trc.step = step;
    trc.file = file;
    trc.line = line;
    trc.func = func;
    if (mLevel < cMaxLevel)
    {
        mTrace[mLevel + 1].step = cStepNone;
    }
    mDone = true;
    if (mShow > mLevel)
    {
        out(nl);
    }
}

std::ostream& TestSteps::cstep(
    const TestStepT cstep,
    const char* const file,
    const TestLineT line,
    const char* const func
)
{
    const bool show = mShow > mLevel;
    step(cstep, file, line, func, not show);
    return show ? std::cout << ' ' : mNullStream;
}


void TestSteps::setLine(const TestLineT line)
{
    mTrace[mLevel].line = line;
}

void TestSteps::subSteps(
    const char* const file,
    const TestLineT line,
    const char* const func
)
{
    enterLevel(mLevel + 1, file, line, func);
}

void TestSteps::endSteps()
{
    chk(mLevel > 0, c__ErrEndSteps);
    clearLevel(mLevel);
    --mLevel;
}

bool TestSteps::subStepsDone()
{
    return mTrace[1].step != cStepNone;
}

void TestSteps::out(const bool nl)
{
    cout << c__step;
    for (StepLevelT l = 0; l < cNmuLevels; ++l)
    {
        const Trace& trc = mTrace[l];
        if (trc.step != cStepNone)
        {
            if (l > 0)
            {
                cout << '.';
            }
            stepOut(trc.step);
        }
    }
    cout << ')';
    if (nl)
    {
        cout << '\n';
    }
}

void TestSteps::trace(const bool insertLine)
{
    if (insertLine)
    {
        cout << '\n';
    }

    for (StepLevelT l = 0; l < cNmuLevels; ++l)
    {
        const Trace& trc = mTrace[l];
        if (trc.step != cStepNone)
        {
            if (l > 0)
            {
                cout << '\n';
            }
            cout << c__step;
            stepOut(trc.step);
            cout << ")\n";
            if (trc.file != 0)
            {
                cout << c__file << trc.file << '\n';
            }
            if (trc.func != 0)
            {
                cout << c__func << trc.func << '\n';
            }
            cout << c__line << trc.line << '\n';
        }
    }
}

void TestSteps::fail()
{
    if (mDone)
    {
        if (subStepsDone())
        {
            out();
            trace(true);
        }
        else
        {
            trace();
        }
    }
    clear();
}

void TestSteps::chk(const bool ok, const char* const msg)
{
    if (! ok)
    {
        clear();
        FAIL(msg);
    }
}

void TestSteps::stepOut(const TestStepT step)
{
    if (step == cStepPre)
    {
        cout << c__pre;
    }
    else
    {
        cout << step;
    }
}
