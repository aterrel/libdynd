//
// Copyright (C) 2011-14 Mark Wiebe, DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _DYND__ROLLING_CKERNEL_DEFERRED_HPP_
#define _DYND__ROLLING_CKERNEL_DEFERRED_HPP_

#include <dynd/config.hpp>
#include <dynd/array.hpp>
#include <dynd/kernels/ckernel_deferred.hpp>

namespace dynd {

/**
 * Create a deferred ckernel which applies a given window_op in a
 * rolling window fashion.
 *
 * \param out_ckd  The output ckernel_deferred which is filled.
 * \param dst_tp  The destination type for the resulting ckernel_deferred.
 * \param src_tp  The source type for the resulting ckernel_deferred.
 * \param window_op  A ckernel_deferred object which should be applied to each
 *                   window. The types of this ckernel must match appropriately
 *                   with `dst_tp` and `src_tp`.
 * \param window_size  The size of the rolling window.
 */
void make_rolling_ckernel_deferred(ckernel_deferred *out_ckd,
                                   const ndt::type &dst_tp,
                                   const ndt::type &src_tp,
                                   const nd::array &window_op, int window_size);

} // namespace dynd

#endif // _DYND__ROLLING_CKERNEL_DEFERRED_HPP_
