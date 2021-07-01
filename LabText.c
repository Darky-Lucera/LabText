#include "LabText.h"

//------------------------------------------------------------------------------
// IMPLEMENTATION
//------------------------------------------------------------------------------

#include <math.h>

//! @todo replace Assert with custom error reporting mechanism
#include <assert.h>
#define Assert assert

//----------------------------------------------------------------------------

const char* tsScanForQuote(
    const char* pCurr, const char* pEnd,
    char delim,
    bool recognizeEscapes)
{
    Assert(pCurr && pEnd && pEnd >= pCurr);

    while (pCurr < pEnd) {
        if (*pCurr == '\\' && recognizeEscapes) // not handling multicharacter escapes such as \u23AB
            ++pCurr;
        else if (*pCurr == delim)
            break;
        ++pCurr;
    }

    return pCurr;
}

const char* tsScanForWhiteSpace(
    const char* pCurr, const char* pEnd)
{
    Assert(pCurr && pEnd && pEnd >= pCurr);

    while (pCurr < pEnd && !tsIsWhiteSpace(*pCurr))
        ++pCurr;

    return pCurr+1;
}

const char* tsScanForNonWhiteSpace(
   const char* pCurr, const char* pEnd)
{
    Assert(pCurr && pEnd && pEnd >= pCurr);

    while (pCurr < pEnd && tsIsWhiteSpace(*pCurr))
        ++pCurr;

    return pCurr;
}

const char* tsScanBackwardsForWhiteSpace(
    const char* pCurr, const char* pStart)
{
    Assert(pCurr && pStart && pStart <= pCurr);

    while (pCurr >= pStart && !tsIsWhiteSpace(*pCurr))
        --pCurr;

    return pCurr;
}

const char* tsScanForTrailingNonWhiteSpace(
    const char* pCurr, const char* pEnd)
{
    Assert(pCurr && pEnd && pEnd >= pCurr);

    while (pCurr < pEnd && tsIsWhiteSpace(*pEnd))
        --pEnd;

    return pEnd;
}

const char* tsScanForCharacter(
    const char* pCurr, const char* pEnd,
    char delim)
{
    Assert(pCurr && pEnd);

    while (pCurr < pEnd && *pCurr != delim)
        ++pCurr;

    return pCurr;
}

const char* tsScanBackwardsForCharacter(
    const char* pCurr, const char* pStart,
    char delim)
{
    Assert(pCurr && pStart && pStart <= pCurr);

    while (pCurr >= pStart && *pCurr != delim)
        --pCurr;

    return pCurr;
}

const char*
tsScanPastString(const char* pCurr, const char* pEnd, char *pDelim)
{
    uint32_t	i;

    assert(pCurr && pEnd);

    while(pCurr < pEnd)
    {
        while(pCurr < pEnd && *pCurr != *pDelim)
            ++pCurr;

        i=1;
        while(pCurr < pEnd && *(pDelim+i) != 0)
        {
            if(*(pDelim+i) != *(pCurr+i))
                break;
            ++i;
        }

        pCurr+=i;
        if(pCurr < pEnd && *(pDelim+i) == 0)
            break;
    }

    return pCurr;
}

const char* tsScanForEndOfLine(
    const char* pCurr, const char* pEnd)
{
    while (pCurr < pEnd)
    {
        if (*pCurr == '\r')
        {
            ++pCurr;
            if (*pCurr == '\n')
                ++pCurr;
            break;
        }
        if (*pCurr == '\n')
        {
            ++pCurr;
            if (*pCurr == '\r')
                ++pCurr;
            break;
        }

        ++pCurr;
    }
    return pCurr;
}

const char* tsScanForLastCharacterOnLine(
    const char* pCurr, const char* pEnd)
{
    while (pCurr < pEnd)
    {
        if (pCurr[1] == '\r' || pCurr[1] == '\n' || pCurr[1] == '\0')
        {
            break;
        }

        ++pCurr;
    }
    return pCurr;
}

const char* tsScanForBeginningOfNextLine(
    const char* pCurr, const char* pEnd)
{
    pCurr = tsScanForEndOfLine(pCurr, pEnd);
    return (tsScanForNonWhiteSpace(pCurr, pEnd));
}

const char* tsScanPastCPPComments(
    const char* pCurr, const char* pEnd)
{
    if (*pCurr == '/')
    {
        if (pCurr[1] == '/')
        {
            pCurr = tsScanForEndOfLine(pCurr, pEnd);
        }
        else if (pCurr[1] == '*')
        {
            pCurr = &pCurr[2];
            while (pCurr < pEnd)
            {
                if (pCurr[0] == '*' && pCurr[1] == '/')
                {
                    pCurr = &pCurr[2];
                    break;
                }

                ++pCurr;
            }
        }
    }

    return pCurr;
}

const char* tsSkipCommentsAndWhitespace(
    const char* curr, const char*const end)
{
    bool moved = true;
    while (moved)
    {
        const char* past = tsScanForNonWhiteSpace(curr, end);
        curr = past;

        past = tsScanPastCPPComments(curr, end);
        moved = past != curr;
        curr = past;
    }

    return tsScanForNonWhiteSpace(curr, end);
}

const char* tsGetToken(
    const char* pCurr, const char* pEnd,
    char delim,
    const char** resultStringBegin, uint32_t* stringLength)
{
    Assert(pCurr && pEnd);

    pCurr = tsScanForNonWhiteSpace(pCurr, pEnd);
    *resultStringBegin = pCurr;
    const char* pStringEnd = tsScanForCharacter(pCurr, pEnd, delim);
    *stringLength = (uint32_t)(pStringEnd - *resultStringBegin);
    return pStringEnd;
}

const char* tsGetTokenWSDelimited(
    const char* pCurr, const char* pEnd,
    const char** resultStringBegin, uint32_t* stringLength)
{
    pCurr = tsScanForNonWhiteSpace(pCurr, pEnd);
    *resultStringBegin = pCurr;
    const char* pStringEnd = tsScanForWhiteSpace(pCurr, pEnd);
    *stringLength = (uint32_t)(pStringEnd - *resultStringBegin);
    return pStringEnd;
}

const char* tsGetTokenAlphaNumericExt(
    const char* pCurr, const char* pEnd,
    const char* ext_,
    const char** resultStringBegin, uint32_t* stringLength)
{
    Assert(pCurr && pEnd);

    pCurr = tsScanForNonWhiteSpace(pCurr, pEnd);
    *resultStringBegin = pCurr;
    *stringLength = 0;

    while (pCurr < pEnd)
    {
        char test = pCurr[0];

        if (tsIsWhiteSpace(test))
            break;

        bool accept = tsIsNumeric(test) || tsIsAlpha(test);
        const char* ext = ext_;
        for ( ; *ext && !accept; ++ext)
            accept |= *ext == test;

        if (!accept)
            break;

        ++pCurr;
        *stringLength += 1;
    }

    return pCurr;
}

const char* tsGetTokenExt(
    const char* pCurr, const char* pEnd,
    const char* ext_,
    const char** resultStringBegin, uint32_t* stringLength)
{
    Assert(pCurr && pEnd);

    pCurr = tsScanForNonWhiteSpace(pCurr, pEnd);
    *resultStringBegin = pCurr;
    *stringLength = 0;

    while (pCurr < pEnd)
    {
        char test = pCurr[0];

        if (tsIsWhiteSpace(test))
            break;

        bool accept = false;
        const char* ext = ext_;
        for (; *ext && !accept; ++ext)
            accept |= *ext == test;

        if (!accept)
            break;

        ++pCurr;
        *stringLength += 1;
    }

    return pCurr;
}

const char* tsGetTokenAlphaNumeric(
    const char* pCurr, const char* pEnd,
    const char** resultStringBegin, uint32_t* stringLength)
{
    Assert(pCurr && pEnd);

    pCurr = tsScanForNonWhiteSpace(pCurr, pEnd);
    *resultStringBegin = pCurr;
    *stringLength = 0;

    while (pCurr < pEnd)
    {
        char test = pCurr[0];

        if (tsIsWhiteSpace(test))
            break;

        bool accept = ((test == '_') || tsIsNumeric(test) || tsIsAlpha(test));

        if (!accept)
            break;

        ++pCurr;
        *stringLength += 1;
    }

    return pCurr;
}

const char* tsGetNameSpacedTokenAlphaNumeric(
    const char* pCurr, const char* pEnd,
    char namespaceChar,
    const char** resultStringBegin, uint32_t* stringLength)
{
    Assert(pCurr && pEnd);

    pCurr = tsScanForNonWhiteSpace(pCurr, pEnd);
    *resultStringBegin = pCurr;
    *stringLength = 0;

    while (pCurr < pEnd)
    {
        char test = pCurr[0];

        if (tsIsWhiteSpace(test))
            break;

        // should pass in a string of acceptable characters, ie "$^_"
        bool accept = ((test == namespaceChar) || (test == '$') || (test == '^') || (test == '_') || tsIsNumeric(test) || tsIsAlpha(test));

        if (!accept)
            break;

        ++pCurr;
        *stringLength += 1;
    }

    return pCurr;
}

const char* tsGetString(
    const char* pCurr, const char* pEnd,
    bool recognizeEscapes,
    const char** resultStringBegin, uint32_t* stringLength)
{
    Assert(pCurr && pEnd && pEnd >= pCurr);

    pCurr = tsScanForQuote(pCurr, pEnd, '\"', recognizeEscapes);

    if (pCurr < pEnd)
    {
        ++pCurr;    // skip past quote
        *resultStringBegin = pCurr;

        pCurr = tsScanForQuote(pCurr, pEnd, '\"', recognizeEscapes);

        if (pCurr <= pEnd)
            *stringLength = (uint32_t)(pCurr - *resultStringBegin);
        else
            *stringLength = 0;

        ++pCurr;    // point past closing quote
    }
    else
        *stringLength = 0;

    return pCurr;
}

const char* tsGetStringQuoted(
                        const char* pCurr, const char* pEnd,
                        char delim,
                        bool recognizeEscapes,
                        const char** resultStringBegin, uint32_t* stringLength)
{
    Assert(pCurr && pEnd && pEnd >= pCurr);

    pCurr = tsScanForQuote(pCurr, pEnd, delim, recognizeEscapes);

    if (pCurr < pEnd)
    {
        ++pCurr;    // skip past quote
        *resultStringBegin = pCurr;

        pCurr = tsScanForQuote(pCurr, pEnd, delim, recognizeEscapes);

        if (pCurr <= pEnd)
            *stringLength = (uint32_t)(pCurr - *resultStringBegin);
        else
            *stringLength = 0;

        ++pCurr;    // point past closing quote
    }
    else
        *stringLength = 0;

    return pCurr;
}

// Match pExpect. If pExect is found in the input stream, return pointing
// to the character that follows, otherwise return the start of the input stream

const char* tsExpect(
    const char* pCurr, const char*const pEnd,
    const char* pExpect)
{
    const char* pScan = pCurr;
    while (pScan != pEnd && *pScan == *pExpect && *pExpect != '\0') {
        ++pScan;
        ++pExpect;
    }
    return (*pExpect == '\0' ? pScan : pCurr);
}

const char* tsGetInt16(
    const char* pCurr, const char* pEnd,
    int16_t* result)
{
    int32_t longresult;
    const char* retval = tsGetInt32(pCurr, pEnd, &longresult);
    *result = (int16_t) longresult;
    return retval;
}

const char* tsGetInt32(
    const char* pCurr, const char* pEnd,
    int32_t* result)
{
    pCurr = tsScanForNonWhiteSpace(pCurr, pEnd);

    int ret = 0;

    bool signFlip = false;

    if (*pCurr == '+')
    {
        ++pCurr;
    }
    else if (*pCurr == '-')
    {
        ++pCurr;
        signFlip = true;
    }

    while (pCurr < pEnd)
    {
        if (!tsIsNumeric(*pCurr))
        {
            break;
        }
        ret = ret * 10 + *pCurr - '0';
        ++pCurr;
    }

    if (signFlip)
    {
        ret = -ret;
    }
    *result = ret;
    return pCurr;
}

const char* tsGetUInt32(
    const char* pCurr, const char* pEnd,
    uint32_t* result)
{
    pCurr = tsScanForNonWhiteSpace(pCurr, pEnd);

    uint32_t ret = 0;

    while (pCurr < pEnd)
    {
        if (!tsIsNumeric(*pCurr))
        {
            break;
        }
        ret = ret * 10 + *pCurr - '0';
        ++pCurr;
    }
    *result = ret;
    return pCurr;
}

const char* tsGetFloat(
    const char* pCurr, const char* pEnd,
    float* result)
{
    pCurr = tsScanForNonWhiteSpace(pCurr, pEnd);

    float ret = 0.0f;

    bool signFlip = false;

    if (*pCurr == '+')
    {
        ++pCurr;
    }
    else if (*pCurr == '-')
    {
        ++pCurr;
        signFlip = true;
    }

    // get integer part
    int32_t intPart;
    pCurr = tsGetInt32(pCurr, pEnd, &intPart);
    ret = (float) intPart;

    // get fractional part
    if (*pCurr == '.')
    {
        ++pCurr;

        float scaler = 0.1f;
        while (pCurr < pEnd)
        {
            if (!tsIsNumeric(*pCurr))
            {
                break;
            }
            ret = ret + (float)(*pCurr - '0') * scaler;
            ++pCurr;
            scaler *= 0.1f;
        }
    }

    // get exponent
    if (*pCurr == 'e' || *pCurr == 'E')
    {
        ++pCurr;

        pCurr = tsGetInt32(pCurr, pEnd, &intPart);
        ret *= powf(10.0f, (float) intPart);
    }

    if (signFlip)
    {
        ret = -ret;
    }
    *result = ret;
    return pCurr;
}

const char* tsGetDouble(
    const char* pCurr, const char* pEnd,
    double* result)
{
    pCurr = tsScanForNonWhiteSpace(pCurr, pEnd);

    double ret = 0.0;

    bool signFlip = false;

    if (*pCurr == '+')
    {
        ++pCurr;
    }
    else if (*pCurr == '-')
    {
        ++pCurr;
        signFlip = true;
    }

    // get integer part
    int32_t intPart;
    pCurr = tsGetInt32(pCurr, pEnd, &intPart);
    ret = (double) intPart;

    // get fractional part
    if (*pCurr == '.')
    {
        ++pCurr;

        double scaler = 0.1;
        while (pCurr < pEnd)
        {
            if (!tsIsNumeric(*pCurr))
            {
                break;
            }
            ret = ret + (double)(*pCurr - '0') * scaler;
            ++pCurr;
            scaler *= 0.1f;
        }
    }

    // get exponent
    if (*pCurr == 'e' || *pCurr == 'E')
    {
        ++pCurr;

        pCurr = tsGetInt32(pCurr, pEnd, &intPart);
        ret *= pow(10.0, (double) intPart);
    }

    if (signFlip)
    {
        ret = -ret;
    }
    *result = ret;
    return pCurr;
}

const char* tsGetHex(
    const char* pCurr, const char* pEnd,
    uint32_t* result)
{
    pCurr = tsScanForNonWhiteSpace(pCurr, pEnd);

    int ret = 0;

    while (pCurr < pEnd)
    {
        if (tsIsNumeric(*pCurr))
        {
            ret = ret * 16 + *pCurr - '0';
        }
        else if (*pCurr >= 'A' && *pCurr <= 'F')
        {
            ret = ret * 16 + *pCurr - 'A' + 10;
        }
        else if (*pCurr >= 'a' && *pCurr <= 'f')
        {
            ret = ret * 16 + *pCurr - 'a' + 10;
        }
        else
        {
            break;
        }
        ++pCurr;
    }

    *result = ret;
    return pCurr;
}

bool tsIsIn(const char* testString, char test)
{
    for (; *testString != '\0'; ++testString)
        if (*testString == test)
            return true;
    return false;
}

bool tsIsWhiteSpace(char test)
{
    return (test == 9 || test == ' ' || test == 13 || test == 10);
}

bool tsIsEndOfLine(char test)
{
    return (test == 13 || test == 10);
}

bool tsIsNumeric(char test)
{
    return (test >= '0' && test <= '9');
}

bool tsIsAlpha(char test)
{
    return ((test >= 'a' && test <= 'z') || (test >= 'A' && test <= 'Z'));
}
