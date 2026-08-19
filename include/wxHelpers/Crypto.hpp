#pragma once

#include <wx/wx.h>
#include <wx/base64.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <random>

namespace wxHelpers::Crypto {

// Base64 Encoding and Decoding
inline wxString Base64Encode(const wxString& text) {
    auto utf8 = text.ToUTF8();
    return wxBase64Encode(utf8.data(), utf8.length());
}

inline wxString Base64Decode(const wxString& base64Text) {
    wxMemoryBuffer buf = wxBase64Decode(base64Text);
    return wxString::FromUTF8(static_cast<const char*>(buf.GetData()), buf.GetDataLen());
}

// Generate random UUID v4
inline wxString GenerateUUID() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;

    uint64_t ab = dis(gen);
    uint64_t cd = dis(gen);

    // Set version 4 and variant RFC 4122
    ab = (ab & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    cd = (cd & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;

    return wxString::Format("%08x-%04x-%04x-%04x-%012llx",
        static_cast<uint32_t>(ab >> 32),
        static_cast<uint16_t>((ab >> 16) & 0xFFFF),
        static_cast<uint16_t>(ab & 0xFFFF),
        static_cast<uint16_t>(cd >> 48),
        static_cast<unsigned long long>(cd & 0xFFFFFFFFFFFFULL));
}

// Fast Simple Hash (DJB2) for non-cryptographic checksums
inline wxString FastHash(const wxString& text) {
    auto utf8 = text.ToUTF8();
    const char* str = utf8.data();
    unsigned long hash = 5381;
    int c = 0;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return wxString::Format("%08lx", hash);
}

} // namespace wxHelpers::Crypto
