/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <base/Optional.h>
#include <base/StringView.h>
#include <base/URL.h>

namespace Base {

#define ENUMERATE_STATES                 \
    STATE(SchemeStart)                   \
    STATE(Scheme)                        \
    STATE(NoScheme)                      \
    STATE(SpecialRelativeOrAuthority)    \
    STATE(PathOrAuthority)               \
    STATE(Relative)                      \
    STATE(RelativeSlash)                 \
    STATE(SpecialAuthoritySlashes)       \
    STATE(SpecialAuthorityIgnoreSlashes) \
    STATE(Authority)                     \
    STATE(Host)                          \
    STATE(Hostname)                      \
    STATE(Port)                          \
    STATE(File)                          \
    STATE(FileSlash)                     \
    STATE(FileHost)                      \
    STATE(PathStart)                     \
    STATE(Path)                          \
    STATE(CannotBeABaseUrlPath)          \
    STATE(Query)                         \
    STATE(Fragment)

class URLParser {
public:
    enum class State {
#define STATE(state) state,
        ENUMERATE_STATES
#undef STATE
    };

    static char const* state_name(State const& state)
    {
        switch (state) {
#define STATE(state)   \
    case State::state: \
        return #state;
            ENUMERATE_STATES
#undef STATE
        }
        VERIFY_NOT_REACHED();
    }

    static URL parse(Badge<URL>, StringView const& input, URL const* base_url = nullptr);

private:
    static Optional<URL> parse_data_url(StringView const& raw_input);
};

#undef ENUMERATE_STATES

}

using Base::URLParser;