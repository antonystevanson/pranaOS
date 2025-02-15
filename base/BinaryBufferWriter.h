/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

#include <base/Span.h>
#include <base/StdLibExtraDetails.h>

namespace Base {

class BinaryBufferWriter {
public:
    BinaryBufferWriter(Bytes target)
        : m_target(target)
    {
    }

    template<typename T>
    requires(Base::Detail::IsTriviallyConstructible<T>) T& append_structure()
    {
        VERIFY((reinterpret_cast<FlatPtr>(m_target.data()) + m_offset) % alignof(T) == 0);
        VERIFY(m_offset + sizeof(T) <= m_target.size());
        T* allocated = new (m_target.data() + m_offset) T;
        m_offset += sizeof(T);
        return *allocated;
    }

    void skip_bytes(size_t num_bytes)
    {
        VERIFY(m_offset + num_bytes <= m_target.size());
        m_offset += num_bytes;
    }

    [[nodiscard]] size_t current_offset() const
    {
        return m_offset;
    }

private:
    Bytes m_target;
    size_t m_offset { 0 };
};

}