/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <sys/cdefs.h>

__BEGIN_DECLS

#define bswap_16(x) (__builtin_bswap16(x))
#define bswap_32(x) (__builtin_bswap32(x))
#define bswap_64(x) (__builtin_bswap64(x))

__END_DECLS