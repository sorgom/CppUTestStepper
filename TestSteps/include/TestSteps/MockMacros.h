//  ============================================================
//  CppUTest mocking macros
//  ============================================================
//  created by Manfred Sorgo

#ifndef MOCK_MACROS_H
#define MOCK_MACROS_H

#include <CppUTestExt/MockSupport.h>

//  standard check 'n' clear
//  should be called after each test step
//  - check expectations
//  - clear mock
#ifndef CHECK_N_CLEAR
#define CHECK_N_CLEAR() mock().checkExpectations(); mock().clear();
#endif

//  CppUTest parameter macros
//  common
#define TPARAM(TYPE, NAME) \
    withParameterOfType(#TYPE, #NAME, &NAME)

#define PARAM(NAME) \
    withParameter(#NAME, NAME)

#define PPARAM(NAME) \
    withPointerParameter(#NAME, (void*) NAME)

//  call return defaults
#define RETURN_DEF_INT(VAL) \
    returnIntValueOrDefault(VAL)

#define RETURN_DEF_BOOL(VAL) \
    returnUnsignedIntValueOrDefault(VAL)

#define RETURN_DEF_UNSIGNED(VAL) \
    returnUnsignedIntValueOrDefault(VAL)

//  expectations
#define AND_RETURN(VAL) \
    andReturnValue(VAL)

#define AND_RETURN_BOOL(VAL) \
    andReturnValue(static_cast<unsigned>(VAL))

#define IGNORE() \
    ignoreOtherParameters()

#endif // _H
