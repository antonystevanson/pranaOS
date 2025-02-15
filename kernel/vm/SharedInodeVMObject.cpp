/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

// includes
#include <kernel/filesystem/Inode.h>
#include <kernel/vm/SharedInodeVMObject.h>

namespace Kernel {

RefPtr<SharedInodeVMObject> SharedInodeVMObject::try_create_with_inode(Inode& inode)
{
    size_t size = inode.size();
    if (auto shared_vmobject = inode.shared_vmobject())
        return shared_vmobject.release_nonnull();
    auto vmobject = adopt_ref_if_nonnull(new (nothrow) SharedInodeVMObject(inode, size));
    if (!vmobject)
        return nullptr;
    vmobject->inode().set_shared_vmobject(*vmobject);
    return vmobject;
}

RefPtr<VMObject> SharedInodeVMObject::try_clone()
{
    return adopt_ref_if_nonnull(new (nothrow) SharedInodeVMObject(*this));
}

SharedInodeVMObject::SharedInodeVMObject(Inode& inode, size_t size)
    : InodeVMObject(inode, size)
{
}

SharedInodeVMObject::SharedInodeVMObject(SharedInodeVMObject const& other)
    : InodeVMObject(other)
{
}

}