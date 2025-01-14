/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <base/Types.h>
#include <kernel/Arch/x86/Processor.h>

namespace Kernel {

class ScopedCritical {
    BASE_MAKE_NONCOPYABLE(ScopedCritical);

public:
    ScopedCritical()
    {
        enter();
    }

    ~ScopedCritical()
    {
        if (m_valid)
            leave();
    }

    ScopedCritical(ScopedCritical&& from)
        : m_prev_flags(exchange(from.m_prev_flags, 0))
        , m_valid(exchange(from.m_valid, false))
    {
    }

    ScopedCritical& operator=(ScopedCritical&& from)
    {
        if (&from != this) {
            m_prev_flags = exchange(from.m_prev_flags, 0);
            m_valid = exchange(from.m_valid, false);
        }
        return *this;
    }

    void leave()
    {
        VERIFY(m_valid);
        m_valid = false;
        Processor::current().leave_critical(m_prev_flags);
    }

    void enter()
    {
        VERIFY(!m_valid);
        m_valid = true;
        Processor::current().enter_critical(m_prev_flags);
    }

private:
    u32 m_prev_flags { 0 };
    bool m_valid { false };
};

}