/*
 * Copyright (c) 2021, antonystevanson
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <base/String.h>
#include <libtextcodec/Decoder.h>
#include <stddef.h>
#include <stdint.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    auto* decoder = TextCodec::decoder_for("windows-1252");
    VERIFY(decoder);
    decoder->to_utf8({ data, size });
    return 0;
}
