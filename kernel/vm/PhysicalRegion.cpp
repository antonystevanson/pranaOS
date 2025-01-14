/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

// includes
#include <base/NonnullRefPtr.h>
#include <base/RefPtr.h>
#include <kernel/Assertions.h>
#include <kernel/Random.h>
#include <kernel/vm/MemoryManager.h>
#include <kernel/vm/PhysicalRegion.h>
#include <kernel/vm/PhysicalZone.h>

namespace Kernel {

static constexpr u32 next_power_of_two(u32 value)
{
    value--;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value++;
    return value;
}

PhysicalRegion::~PhysicalRegion()
{
}

PhysicalRegion::PhysicalRegion(PhysicalAddress lower, PhysicalAddress upper)
    : m_lower(lower)
    , m_upper(upper)
{
    m_pages = (m_upper.get() - m_lower.get()) / PAGE_SIZE;
}

void PhysicalRegion::initialize_zones()
{
    size_t remaining_pages = m_pages;
    auto base_address = m_lower;

    auto make_zones = [&](size_t pages_per_zone) {
        size_t zone_count = 0;
        auto first_address = base_address;
        while (remaining_pages >= pages_per_zone) {
            m_zones.append(make<PhysicalZone>(base_address, pages_per_zone));
            base_address = base_address.offset(pages_per_zone * PAGE_SIZE);
            m_usable_zones.append(m_zones.last());
            remaining_pages -= pages_per_zone;
            ++zone_count;
        }
        if (zone_count)
            dmesgln(" * {}x PhysicalZone ({} MiB) @ {:016x}-{:016x}", zone_count, pages_per_zone / 256, first_address.get(), base_address.get() - pages_per_zone * PAGE_SIZE - 1);
    };

    make_zones(4096);

    make_zones(256);
}

OwnPtr<PhysicalRegion> PhysicalRegion::try_take_pages_from_beginning(unsigned page_count)
{
    VERIFY(page_count > 0);
    VERIFY(page_count < m_pages);
    auto taken_lower = m_lower;
    auto taken_upper = taken_lower.offset((PhysicalPtr)page_count * PAGE_SIZE);
    m_lower = m_lower.offset((PhysicalPtr)page_count * PAGE_SIZE);
    m_pages = (m_upper.get() - m_lower.get()) / PAGE_SIZE;

    return try_create(taken_lower, taken_upper);
}

NonnullRefPtrVector<PhysicalPage> PhysicalRegion::take_contiguous_free_pages(size_t count)
{
    auto rounded_page_count = next_power_of_two(count);
    auto order = __builtin_ctz(rounded_page_count);

    Optional<PhysicalAddress> page_base;
    for (auto& zone : m_usable_zones) {
        page_base = zone.allocate_block(order);
        if (page_base.has_value()) {
            if (zone.is_empty()) {
                m_full_zones.append(zone);
            }
            break;
        }
    }

    if (!page_base.has_value())
        return {};

    NonnullRefPtrVector<PhysicalPage> physical_pages;
    physical_pages.ensure_capacity(count);

    for (size_t i = 0; i < count; ++i)
        physical_pages.append(PhysicalPage::create(page_base.value().offset(i * PAGE_SIZE)));
    return physical_pages;
}

RefPtr<PhysicalPage> PhysicalRegion::take_free_page()
{
    if (m_usable_zones.is_empty())
        return nullptr;

    auto& zone = *m_usable_zones.first();
    auto page = zone.allocate_block(0);
    VERIFY(page.has_value());

    if (zone.is_empty()) {

        m_full_zones.append(zone);
    }

    return PhysicalPage::create(page.value());
}

void PhysicalRegion::return_page(PhysicalAddress paddr)
{
    for (auto& zone : m_zones) {
        if (zone.contains(paddr)) {
            zone.deallocate_block(paddr, 0);
            if (m_full_zones.contains(zone))
                m_usable_zones.append(zone);
            return;
        }
    }

    VERIFY_NOT_REACHED();
}

}