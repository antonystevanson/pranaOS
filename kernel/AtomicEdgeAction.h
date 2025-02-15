/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <base/Atomic.h>
#include <kernel/arch/x86/Processor.h>

namespace Kernel {

template<typename AtomicRefCountType>
class AtomicEdgeAction {
public:
    template<typename FirstRefAction>
    bool ref(FirstRefAction first_ref_action)
    {
        AtomicRefCountType expected = 0;
        AtomicRefCountType desired = (1 << 1) | 1;

        for (;;) {
            if (m_atomic_ref_count.compare_exchange_strong(expected, desired, Base::memory_order_relaxed))
                break;

            Processor::wait_check();

            expected &= ~1;
            desired = expected + (1 << 1);
            VERIFY(desired > expected);
            if (expected == 0)
                desired |= 1;
        }

        atomic_thread_fence(Base::memory_order_acquire);

        if (expected == 0) {
            first_ref_action();

            m_atomic_ref_count.store(desired & ~1, Base::memory_order_release);
            return true;
        }
        return false;
    }

    template<typename LastRefAction>
    bool unref(LastRefAction last_ref_action)
    {
        AtomicRefCountType expected = 1 << 1;
        AtomicRefCountType desired = (1 << 1) | 1;
        for (;;) {
            if (m_atomic_ref_count.compare_exchange_strong(expected, desired, Base::memory_order_relaxed))
                break;

            Processor::wait_check();

            expected &= ~1;
            VERIFY(expected != 0); 

            if (expected == 1 << 1) {
                desired = (1 << 1) | 1;
            } else {
                desired = expected - (1 << 1);
                VERIFY(desired < expected);
            }
        }

        Base::atomic_thread_fence(Base::memory_order_release);

        if (expected == 1 << 1) {
            last_ref_action();

            m_atomic_ref_count.store(0, Base::memory_order_release);
            return true;
        }
        return false;
    }

private:
    Atomic<AtomicRefCountType> m_atomic_ref_count { 0 };
};

}