//
// Copyright (C) 2011-13 Mark Wiebe, DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _DYND__BYTES_DTYPE_HPP_
#define _DYND__BYTES_DTYPE_HPP_

#include <dynd/dtype.hpp>
#include <dynd/dtypes/base_bytes_dtype.hpp>
#include <dynd/dtype_assign.hpp>

namespace dynd {

struct bytes_dtype_metadata {
    /**
     * A reference to the memory block which contains the byte's data.
     * NOTE: This is identical to string_dtype_metadata, by design. Maybe
     *       both should become a typedef to a common class?
     */
    memory_block_data *blockref;
};

struct bytes_dtype_data {
    char *begin;
    char *end;
};

/**
 * The bytes dtype uses memory_block references to store
 * arbitrarily sized runs of bytes.
 */
class bytes_dtype : public base_bytes_dtype {
    size_t m_alignment;

public:
    bytes_dtype(size_t alignment);

    virtual ~bytes_dtype();

    /** Alignment of the bytes data being pointed to. */
    size_t get_data_alignment() const {
        return m_alignment;
    }

    void print_data(std::ostream& o, const char *metadata, const char *data) const;

    void print_dtype(std::ostream& o) const;

    void get_bytes_range(const char **out_begin, const char**out_end, const char *metadata, const char *data) const;

    bool is_unique_data_owner(const char *metadata) const;
    dtype get_canonical_dtype() const;

    void get_shape(size_t i, intptr_t *out_shape) const;
    void get_shape(size_t i, intptr_t *out_shape, const char *metadata) const;

    bool is_lossless_assignment(const dtype& dst_dt, const dtype& src_dt) const;

    bool operator==(const base_dtype& rhs) const;

    void metadata_default_construct(char *metadata, size_t ndim, const intptr_t* shape) const;
    void metadata_copy_construct(char *dst_metadata, const char *src_metadata, memory_block_data *embedded_reference) const;
    void metadata_reset_buffers(char *metadata) const;
    void metadata_finalize_buffers(char *metadata) const;
    void metadata_destruct(char *metadata) const;
    void metadata_debug_print(const char *metadata, std::ostream& o, const std::string& indent) const;

    size_t make_assignment_kernel(
                    hierarchical_kernel *out, size_t offset_out,
                    const dtype& dst_dt, const char *dst_metadata,
                    const dtype& src_dt, const char *src_metadata,
                    kernel_request_t kernreq, assign_error_mode errmode,
                    const eval::eval_context *ectx) const;
};

inline dtype make_bytes_dtype(size_t alignment) {
    return dtype(new bytes_dtype(alignment), false);
}

} // namespace dynd

#endif // _DYND__BYTES_DTYPE_HPP_
