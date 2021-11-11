/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-common-lib
 * Created on: 3 апр. 2020 г.
 *
 * lsp-common-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-common-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-common-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef LSP_PLUG_IN_COMMON_ALLOC_H_
#define LSP_PLUG_IN_COMMON_ALLOC_H_

#include <lsp-plug.in/common/version.h>
#include <lsp-plug.in/common/types.h>
#include <stdlib.h>

namespace lsp
{
    inline size_t align_size(size_t size, size_t align)
    {
        size_t off = size % align;
        return (off) ? (size + align - off) : size;
    }

    template <class T>
        inline T *align_ptr(T *src, size_t align = DEFAULT_ALIGN)
        {
            uintptr_t x     = uintptr_t(src);
            uintptr_t off   = x % align;
            return (off) ?
                reinterpret_cast<T *>(x + align - off) :
                src;
        }

    template <class T>
        inline bool is_ptr_aligned(T *src, size_t align = DEFAULT_ALIGN)
        {
            ptrdiff_t x     = ptrdiff_t(src);
            return !(x % align);
        }

    /** Allocate aligned pointer
     *
     * @param ptr reference to pointer to store allocated pointer for future free() operation
     * @param count number of elements to allocate
     * @param align alignment, should be power of 2, by default DEFAULT_ALIGN
     * @return aligned pointer as a result of alignment of ptr to align boundary or NULL if allocation failed
     * @example
     *      void *x = NULL;
     *      float *a = alloc_aligned<float>(x, 1000); // Allocate 1000 floats aligned to DEFAULT_ALIGN boundary
     *      if (a == NULL)
     *          return ERROR;
     *      // Do some stuff
     *      free_aligned(x);
     *      a = NULL;
     */
    template <class T, class P>
        inline T *alloc_aligned(P * &ptr, size_t count, size_t align=DEFAULT_ALIGN)
        {
            // Check for power of 2
            if ((!align) || (align & (align-1)))
                return NULL;

            // Allocate data
            void *p         = ::malloc((count * sizeof(T)) + align);
            if (p == NULL)
                return NULL;

            // Store pointer
            ptr             = reinterpret_cast<P *>(p);

            // Return aligned pointer
            ptrdiff_t x     = ptrdiff_t(p);
            ptrdiff_t mask  = align-1;
            return reinterpret_cast<T *>((x & mask) ? ((x + align)&(~mask)) : x);
        }

    /** Free aligned pointer and write NULL to it
     *
     * @param ptr pointer to free
     */
    template <class P>
        inline void free_aligned(P * &ptr)
        {
            if (ptr == NULL)
                return;
            P *tptr = ptr;
            ptr = NULL;
            ::free(tptr);
        }

    /**
     * Seed the address
     * @param ptr address to seed
     * @return seed value
     */
    inline uint32_t seed_addr(const void *ptr)
    {
    #if defined(ARCH_I386)
        return uint32_t(ptr);
    #elif defined(ARCH_X86_64)
        return uint32_t(ptrdiff_t(ptr)) ^ uint32_t(ptrdiff_t(ptr) >> 32);
    #else
        return uint32_t(ptrdiff_t(ptr));
    #endif
    }
}

#endif /* LSP_PLUG_IN_COMMON_ALLOC_H_ */
