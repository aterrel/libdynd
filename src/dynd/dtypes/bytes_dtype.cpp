//
// Copyright (C) 2011-12, Dynamic NDArray Developers
// BSD 2-Clause License, see LICENSE.txt
//

#include <dynd/dtypes/bytes_dtype.hpp>
#include <dynd/memblock/pod_memory_block.hpp>
#include <dynd/kernels/single_compare_kernel_instance.hpp>
#include <dynd/kernels/bytes_assignment_kernels.hpp>
#include <dynd/dtypes/fixedbytes_dtype.hpp>
#include <dynd/exceptions.hpp>

#include <algorithm>

using namespace std;
using namespace dynd;

bytes_dtype::bytes_dtype(size_t alignment)
    : m_alignment(alignment)
{
    if (alignment != 1 && alignment != 2 && alignment != 4 && alignment != 8 && alignment != 16) {
        std::stringstream ss;
        ss << "Cannot make a bytes<" << alignment << "> dtype, its alignment is not a small power of two";
        throw std::runtime_error(ss.str());
    }
}

void bytes_dtype::print_element(std::ostream& o, const char *data, const char *DYND_UNUSED(metadata)) const
{
    const char *begin = reinterpret_cast<const char * const *>(data)[0];
    const char *end = reinterpret_cast<const char * const *>(data)[1];

    // Print as hexadecimal
    o << "0x";
    hexadecimal_print(o, begin, end - begin);
}

void bytes_dtype::print_dtype(std::ostream& o) const {

    o << "bytes<" << m_alignment << ">";

}

dtype bytes_dtype::apply_linear_index(int nindices, const irange *indices, int current_i, const dtype& DYND_UNUSED(root_dt)) const
{
    if (nindices == 0) {
        return dtype(this, true);
    } else {
        throw too_many_indices(nindices, current_i + 1);
    }
}

intptr_t bytes_dtype::apply_linear_index(int nindices, const irange *indices, char *data, const char *metadata,
                const dtype& result_dtype, char *out_metadata, int current_i, const dtype& root_dt) const
{
    const bytes_dtype_metadata *md = reinterpret_cast<const bytes_dtype_metadata *>(metadata);
    bytes_dtype_metadata *out_md = reinterpret_cast<bytes_dtype_metadata *>(out_metadata);
    // Just copy the blockref
    out_md->blockref = md->blockref;
    memory_block_incref(out_md->blockref);
    return 0;
}

dtype bytes_dtype::get_canonical_dtype() const
{
    return dtype(this, true);
}


void bytes_dtype::get_shape(int DYND_UNUSED(i), std::vector<intptr_t>& DYND_UNUSED(out_shape)) const
{
}

bool bytes_dtype::is_lossless_assignment(const dtype& dst_dt, const dtype& src_dt) const
{
    if (dst_dt.extended() == this) {
        if (src_dt.get_kind() == bytes_kind) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void bytes_dtype::get_single_compare_kernel(single_compare_kernel_instance& DYND_UNUSED(out_kernel)) const {
    throw std::runtime_error("bytes_dtype::get_single_compare_kernel not supported yet");
}

void bytes_dtype::get_dtype_assignment_kernel(const dtype& dst_dt, const dtype& src_dt,
                assign_error_mode errmode,
                unary_specialization_kernel_instance& out_kernel) const
{
    if (this == dst_dt.extended()) {
        switch (src_dt.get_type_id()) {
            case bytes_type_id: {
                const bytes_dtype *src_fs = static_cast<const bytes_dtype *>(src_dt.extended());
                get_blockref_bytes_assignment_kernel(m_alignment, errmode, out_kernel);
                break;
            }
            case fixedbytes_type_id: {
                const fixedbytes_dtype *src_fs = static_cast<const fixedbytes_dtype *>(src_dt.extended());
                get_fixedbytes_to_blockref_bytes_assignment_kernel(m_alignment,
                                        src_fs->get_element_size(), src_fs->get_alignment(), out_kernel);
                break;
            }
            default: {
                if (src_dt.extended()) {
                    src_dt.extended()->get_dtype_assignment_kernel(dst_dt, src_dt, errmode, out_kernel);
                } else {
                    stringstream ss;
                    ss << "assignment from " << src_dt << " to " << dst_dt << " is not implemented yet";
                    throw runtime_error(ss.str());
                }
                break;
            }
        }
    } else {
        stringstream ss;
        ss << "assignment from " << src_dt << " to " << dst_dt << " is not implemented yet";
        throw runtime_error(ss.str());
    }
}


bool bytes_dtype::operator==(const extended_dtype& rhs) const
{
    if (this == &rhs) {
        return true;
    } else if (rhs.get_type_id() != bytes_type_id) {
        return false;
    } else {
        const bytes_dtype *dt = static_cast<const bytes_dtype*>(&rhs);
        return m_alignment == dt->m_alignment;
    }
}

void bytes_dtype::prepare_kernel_auxdata(const char *metadata, AuxDataBase *auxdata) const
{
    const bytes_dtype_metadata *md = reinterpret_cast<const bytes_dtype_metadata *>(metadata);
    auxdata = reinterpret_cast<AuxDataBase *>(reinterpret_cast<uintptr_t>(auxdata)&~1);
    if (auxdata->kernel_api) {
        auxdata->kernel_api->set_dst_memory_block(auxdata, md->blockref);
    }
}

size_t bytes_dtype::get_metadata_size() const
{
    return sizeof(bytes_dtype_metadata);
}

void bytes_dtype::metadata_default_construct(char *metadata, int DYND_UNUSED(ndim), const intptr_t* DYND_UNUSED(shape)) const
{
    // Simply allocate a POD memory block
    bytes_dtype_metadata *md = reinterpret_cast<bytes_dtype_metadata *>(metadata);
    md->blockref = make_pod_memory_block().release();
}

void bytes_dtype::metadata_copy_construct(char *dst_metadata, const char *src_metadata, memory_block_data *embedded_reference) const
{
    // Copy the blockref, switching it to the embedded_reference if necessary
    const bytes_dtype_metadata *src_md = reinterpret_cast<const bytes_dtype_metadata *>(src_metadata);
    bytes_dtype_metadata *dst_md = reinterpret_cast<bytes_dtype_metadata *>(dst_metadata);
    dst_md->blockref = src_md->blockref ? src_md->blockref : embedded_reference;
    memory_block_incref(dst_md->blockref);
}

void bytes_dtype::metadata_destruct(char *metadata) const
{
    bytes_dtype_metadata *md = reinterpret_cast<bytes_dtype_metadata *>(metadata);
    if (md->blockref) {
        memory_block_decref(md->blockref);
    }
}

void bytes_dtype::metadata_debug_print(const char *metadata, std::ostream& o, const std::string& indent) const
{
    const bytes_dtype_metadata *md = reinterpret_cast<const bytes_dtype_metadata *>(metadata);
    o << indent << "bytes metadata\n";
    memory_block_debug_print(md->blockref, o, indent + " ");
}
