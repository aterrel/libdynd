//
// Copyright (C) 2011-13 Mark Wiebe, DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _DYND__EXPRESSION_COMPARISON_KERNELS_HPP_
#define _DYND__EXPRESSION_COMPARISON_KERNELS_HPP_

#include <dynd/kernels/comparison_kernels.hpp>

namespace dynd {

/**
 * Makes a kernel which does a comparison when
 * at least one of src0_dt and src1_dt is an
 * expression_kind dtype.
 */
size_t make_expression_comparison_kernel(
                hierarchical_kernel *out, size_t offset_out,
                const dtype& src0_dt, const char *src0_metadata,
                const dtype& src1_dt, const char *src1_metadata,
                comparison_type_t comptype,
                const eval::eval_context *ectx);

} // namespace dynd

#endif // _DYND__EXPRESSION_COMPARISON_KERNELS_HPP_

