//
// Copyright (C) 2011-13 Mark Wiebe, DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _DYND__BASE_BYTES_DTYPE_HPP_
#define _DYND__BASE_BYTES_DTYPE_HPP_

#include <dynd/dtypes/base_dtype.hpp>

namespace dynd {


/**
 * Base class for all bytes dtypes. If a dtype
 * has kind bytes_kind, it must be a subclass of
 * base_bytes_dtype.
 */
class base_bytes_dtype : public base_dtype {
public:
    inline base_bytes_dtype(type_id_t type_id, dtype_kind_t kind, size_t data_size,
                    size_t alignment, flags_type flags, size_t metadata_size)
        : base_dtype(type_id, kind, data_size, alignment, flags, metadata_size, 0)
    {}

    virtual ~base_bytes_dtype();

    /** Retrieves the data range in which a bytes object is stored */
    virtual void get_bytes_range(const char **out_begin, const char**out_end, const char *metadata, const char *data) const = 0;

    // Bytes dtypes stop the iterdata chain
    // TODO: Maybe it should be more flexible?
    size_t get_iterdata_size(size_t ndim) const;
};


} // namespace dynd

#endif // _DYND__BASE_BYTES_DTYPE_HPP_
