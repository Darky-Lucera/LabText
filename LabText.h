#pragma once

/*
 __  __           _           _
|  \/  | ___  ___| |__  _   _| | __ _
| |\/| |/ _ \/ __| '_ \| | | | |/ _` |
| |  | |  __/\__ \ | | | |_| | | (_| |
|_|  |_|\___||___/_| |_|\__,_|_|\__,_|
 _____         _
|_   _|____  _| |_
  | |/ _ \ \/ / __\
  | |  __/>  <| |_
  |_|\___/_/\_\\__|

This parser was written in the distant past by Nick Porcino and is freely
available on an as is basis. It is meant for educational
purposes and is not suitable for any particular purpose. No warranty is
expressed or implied. Use at your own risk. Do not operate heavy machinery or
governments while using this code.

License BSD-2 Clause.

*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef _MSC_VER
    #define strcmp _strcmp
    #define strlen _strlen
#endif

#ifndef EXTERNC
    #ifdef __cplusplus
        #define EXTERNC extern "C"
    #else
        #define EXTERNC
    #endif
#endif

// Get Token
EXTERNC const char* tsGetToken                      (const char* pCurr, const char* pEnd, char delim, const char** resultStringBegin, uint32_t* stringLength);
EXTERNC const char* tsGetTokenWSDelimited           (const char* pCurr, const char* pEnd, const char** resultStringBegin, uint32_t* stringLength);
EXTERNC const char* tsGetTokenAlphaNumeric          (const char* pCurr, const char* pEnd, const char** resultStringBegin, uint32_t* stringLength);
EXTERNC const char* tsGetTokenAlphaNumericExt       (const char* pCurr, const char* pEnd, const char* ext, const char** resultStringBegin, uint32_t* stringLength);
EXTERNC const char* tsGetTokenExt                   (const char* pCurr, const char* pEnd, const char* ext, const char** resultStringBegin, uint32_t* stringLength);

EXTERNC const char* tsGetNameSpacedTokenAlphaNumeric(const char* pCurr, const char* pEnd, char namespaceChar, const char** resultStringBegin, uint32_t* stringLength);

// Get Value
EXTERNC const char* tsGetString                     (const char* pCurr, const char* pEnd, bool recognizeEscapes, const char** resultStringBegin, uint32_t* stringLength);
EXTERNC const char* tsGetStringQuoted               (const char* pCurr, const char* pEnd, char strDelim, bool recognizeEscapes, const char** resultStringBegin, uint32_t* stringLength);
EXTERNC const char* tsGetInt16                      (const char* pCurr, const char* pEnd, int16_t* result);
EXTERNC const char* tsGetInt32                      (const char* pCurr, const char* pEnd, int32_t* result);
EXTERNC const char* tsGetUInt32                     (const char* pCurr, const char* pEnd, uint32_t* result);
EXTERNC const char* tsGetHex                        (const char* pCurr, const char* pEnd, uint32_t* result);
EXTERNC const char* tsGetFloat                      (const char* pcurr, const char* pEnd, float* result);
EXTERNC const char* tsGetDouble						(const char* pcurr, const char* pEnd, double* result);

EXTERNC const char* tsScanForCharacter              (const char* pCurr, const char* pEnd, char delim);
EXTERNC const char* tsScanBackwardsForCharacter     (const char* pCurr, const char* pEnd, char delim);
EXTERNC const char* tsScanPastString				(const char* pCurr, const char* pEnd, char *pDelim);
EXTERNC const char* tsScanForWhiteSpace             (const char* pCurr, const char* pEnd);
EXTERNC const char* tsScanBackwardsForWhiteSpace    (const char* pCurr, const char* pStart);
EXTERNC const char* tsScanForNonWhiteSpace          (const char* pCurr, const char* pEnd);
EXTERNC const char* tsScanForTrailingNonWhiteSpace  (const char* pCurr, const char* pEnd);
EXTERNC const char* tsScanForQuote                  (const char* pCurr, const char* pEnd, char delim, bool recognizeEscapes);
EXTERNC const char* tsScanForEndOfLine              (const char* pCurr, const char* pEnd);
EXTERNC const char* tsScanForLastCharacterOnLine    (const char* pCurr, const char* pEnd);
EXTERNC const char* tsScanForBeginningOfNextLine    (const char* pCurr, const char* pEnd);
EXTERNC const char* tsScanPastCPPComments           (const char* pCurr, const char* pEnd);

EXTERNC const char* tsSkipCommentsAndWhitespace     (const char* pCurr, const char*const pEnd);

EXTERNC const char* tsExpect                        (const char* pCurr, const char*const pEnd, const char* pExpect);

EXTERNC bool        tsIsWhiteSpace                  (char test);
EXTERNC bool        tsIsEndOfLine                   (char test);
EXTERNC bool        tsIsNumeric                     (char test);
EXTERNC bool        tsIsAlpha                       (char test);
EXTERNC bool        tsIsIn                          (const char* testString, char test);

#ifdef __cplusplus

#include <vector>

namespace lab { namespace Text {

struct StrView {
    const char *current;
    size_t      length;

    StrView() : current(0x0), length(0) { }
    StrView(const char *str) : current(str), length(strlen(str)) { }
    StrView(const char *str, size_t len) : current(str), length(len) { }
    ~StrView() { current = 0x0, length = 0; }

    bool operator==(const StrView &rhs) const {
        return length == rhs.length && !strncmp(current, rhs.current, length);
    }

    bool operator!=(const StrView &rhs) const {
        return !(*this == rhs);
    }

    bool operator==(const char* rhs) const {
        if (!rhs)
            return false;

        return strlen(rhs) == length && !strncmp(rhs, current, length);
    }

    bool operator<(const StrView &rhs) {
        return strncmp(current, rhs.current, length<rhs.length?length:rhs.length) < 0;
    }
};

inline bool
IsEmpty(const StrView& s) {
    return (s.current == nullptr) || (s.length == 0);
}

inline StrView
GetToken(StrView s, char delim, StrView& result) {
    uint32_t sz;
    const char* next = tsGetToken(s.current, s.current + s.length, delim, &result.current, &sz);
    result.length = sz;
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetTokenWSDelimited(StrView s, char delim, StrView& result) {
    uint32_t sz;
    const char* next = tsGetTokenWSDelimited(s.current, s.current + s.length, &result.current, &sz);
    result.length = sz;
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetTokenAlphaNumeric(StrView s, StrView& result) {
    uint32_t sz;
    const char* next = tsGetTokenAlphaNumeric(s.current, s.current + s.length, &result.current, &sz);
    result.length = sz;
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetTokenAlphaNumericExt (StrView s, const char* ext, StrView& result) {
    uint32_t sz;
    const char* next = tsGetTokenAlphaNumericExt(s.current, s.current + s.length, ext, &result.current, &sz);
    result.length = sz;
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetTokenExt(StrView s, const char* ext, StrView& result) {
    uint32_t sz;
    const char* next = tsGetTokenExt(s.current, s.current + s.length, ext, &result.current, &sz);
    result.length = sz;
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetNameSpacedTokenAlphaNumeric(StrView s, char namespaceChar, StrView& result) {
    uint32_t sz;
    const char* next = tsGetNameSpacedTokenAlphaNumeric(s.current, s.current + s.length, namespaceChar, &result.current, &sz);
    result.length = sz;
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetString(StrView s, bool recognizeEscapes, StrView& result) {
    uint32_t sz;
    const char* next = tsGetString(s.current, s.current + s.length, recognizeEscapes, &result.current, &sz);
    result.length = sz;
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetString2(StrView s, char namespaceChar, char strDelim, bool recognizeEscapes, StrView& result) {
    uint32_t sz;
    const char* next = tsGetStringQuoted(s.current, s.current + s.length, strDelim, recognizeEscapes, &result.current, &sz);
    result.length = sz;
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetInt16(StrView s, int16_t& result) {
    const char* next = tsGetInt16(s.current, s.current + s.length, &result);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetInt32(StrView s, int32_t& result) {
    const char* next = tsGetInt32(s.current, s.current + s.length, &result);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetUInt32(StrView s, uint32_t& result) {
    const char* next = tsGetUInt32(s.current, s.current + s.length, &result);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetHex(StrView s, uint32_t& result) {
    const char* next = tsGetHex(s.current, s.current + s.length, &result);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
GetFloat(StrView s, float& result) {
    const char* next = tsGetFloat(s.current, s.current + s.length, &result);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanForCharacter(StrView s, char delim) {
    const char* next = tsScanForCharacter(s.current, s.current + s.length, delim);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanBackwardsForCharacter(StrView s, char delim) {
    const char* next = tsScanBackwardsForCharacter(s.current, s.current + s.length, delim);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanForWhiteSpace(StrView s) {
    const char* next = tsScanForWhiteSpace(s.current, s.current + s.length);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanBackwardsForWhiteSpace(StrView s) {
    const char* next = tsScanBackwardsForWhiteSpace(s.current, s.current + s.length);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanForNonWhiteSpace(StrView s) {
    const char* next = tsScanForNonWhiteSpace(s.current, s.current + s.length);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanForTrailingNonWhiteSpace(StrView s) {
    const char* next = tsScanForTrailingNonWhiteSpace(s.current, s.current + s.length);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanForQuote(StrView s, char delim, bool recognizeEscapes) {
    const char* next = tsScanForQuote(s.current, s.current + s.length, delim, recognizeEscapes);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanForEndOfLine(StrView s) {
    const char* next = tsScanForEndOfLine(s.current, s.current + s.length);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanForEndOfLine(StrView s, StrView& skipped) {
    skipped = s;
    const char* next = tsScanForEndOfLine(s.current, s.current + s.length);
    skipped.length = next - skipped.current;
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanForLastCharacterOnLine(StrView s) {
    const char* next = tsScanForLastCharacterOnLine(s.current, s.current + s.length);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanForBeginningOfNextLine(StrView s) {
    const char* next = tsScanForBeginningOfNextLine(s.current, s.current + s.length);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
ScanPastCPPComments(StrView s) {
    const char* next = tsScanPastCPPComments(s.current, s.current + s.length);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
SkipCommentsAndWhitespace(StrView s) {
    const char* next = tsSkipCommentsAndWhitespace(s.current, s.current + s.length);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
Expect(StrView s, StrView expect) {
    const char* next = tsExpect(s.current, s.current + s.length, expect.current);
    return { next, static_cast<size_t>(s.current + s.length - next) };
}

inline StrView
Strip(StrView s) {
    StrView result = ScanForNonWhiteSpace(s);
    while (result.length > 0) {
        if (!tsIsWhiteSpace(result.current[result.length - 1]))
            break;
        --result.length;
    }
    return result;
}

inline std::vector<StrView>
Split(StrView s, char splitter) {
    std::vector<StrView> result;
    const char* src = s.current;
    const char* curr = src;
    size_t remaining = s.length;
    size_t sz = 0;
    while (remaining > 0) {
        if (*src != splitter) {
            ++sz;
            ++src;
        }
        else {
            result.emplace_back(StrView{curr, sz});
            sz = 0;
            ++src;
            curr = src;
        }
        --remaining;
    }

    // capture last crumb
    if (sz > 0)
        result.emplace_back(StrView{curr, sz});

    return result;
}

}} // lab::Text

#endif // cplusplus
