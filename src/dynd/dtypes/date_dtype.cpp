//
// Copyright (C) 2011-12, Dynamic NDArray Developers
// BSD 2-Clause License, see LICENSE.txt
//

#include <algorithm>

#include <dynd/dtypes/date_dtype.hpp>
#include <dynd/kernels/single_compare_kernel_instance.hpp>
#include <dynd/kernels/string_assignment_kernels.hpp>
#include <dynd/exceptions.hpp>

#define DYND_DATE_ORIGIN_YEAR 1970

using namespace std;
using namespace dynd;

dynd::date_dtype::date_dtype(date_unit_t unit)
    : m_unit(unit)
{
    switch (unit) {
        case date_unit_day:
        case date_unit_week:
        case date_unit_month:
        case date_unit_year:
            break;
        default:
            throw runtime_error("Unrecognized date unit in date dtype constructor");
    }
}

void dynd::date_dtype::print_element(std::ostream& o, const char *data, const char *DYND_UNUSED(metadata)) const
{
    int32_t value = *reinterpret_cast<const int32_t *>(data);
    switch (m_unit) {
        case date_unit_day:
            break;
        case date_unit_week:
            break;
        case date_unit_month:
            break;
        case date_unit_year:
            o << (DYND_DATE_ORIGIN_YEAR + value);
            break;
    }
}

void dynd::date_dtype::print_dtype(std::ostream& o) const
{
    o << "date<" << m_unit << ">";
}

dtype dynd::date_dtype::apply_linear_index(int nindices, const irange *indices, int current_i, const dtype& DYND_UNUSED(root_dt)) const
{
    // TODO: Could treat "year", "month", "day" or "year", "week" as a dimension.
    if (nindices == 0) {
        return dtype(this);
    } else {
        throw too_many_indices(current_i + nindices, current_i);
    }
}

void dynd::date_dtype::get_shape(int DYND_UNUSED(i), std::vector<intptr_t>& DYND_UNUSED(out_shape)) const
{
}

bool dynd::date_dtype::is_lossless_assignment(const dtype& dst_dt, const dtype& src_dt) const
{
    if (dst_dt.extended() == this) {
        if (src_dt.extended() == this) {
            return true;
        } else if (src_dt.type_id() == date_type_id) {
            const date_dtype *src_fs = static_cast<const date_dtype*>(src_dt.extended());
            return src_fs->m_unit == m_unit;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void dynd::date_dtype::get_single_compare_kernel(single_compare_kernel_instance& out_kernel) const {
    throw runtime_error("get_single_compare_kernel for date are not implemented");
}

void dynd::date_dtype::get_dtype_assignment_kernel(const dtype& dst_dt, const dtype& src_dt,
                assign_error_mode errmode,
                unary_specialization_kernel_instance& out_kernel) const
{
    throw runtime_error("conversions for date are not implemented");
}


bool dynd::date_dtype::operator==(const extended_dtype& rhs) const
{
    if (this == &rhs) {
        return true;
    } else if (rhs.type_id() != date_type_id) {
        return false;
    } else {
        const date_dtype *dt = static_cast<const date_dtype*>(&rhs);
        return m_unit == dt->m_unit;
    }
}
