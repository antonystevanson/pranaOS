/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <libabi/Result.h>
#include <libio/Path.h>
#include <libio/Reader.h>
#include <libio/Writer.h>
struct Archive : public RefCounted<Archive>
{
public:
    struct Entry
    {
        String name;
        size_t uncompressed_size;
        size_t compressed_size;
        size_t archive_offset;
        unsigned int compression;
    };

protected:
    Vector<Entry> _entries;
    IO::Path _path;
    bool _valid = true;

public:
    static RefPtr<Archive> open(IO::Path path, bool read = true);

    Archive(IO::Path path) : _path(path)
    {
    }

    inline const Vector<Entry> &entries()
    {
        return _entries;
    }

    virtual JResult extract(unsigned int entry_index, IO::Writer &writer) = 0;
    virtual JResult insert(const char *entry_name, IO::Reader &reader) = 0;

    inline const IO::Path &get_path()
    {
        return _path;
    }

    inline bool valid()
    {
        return _valid;
    }
};