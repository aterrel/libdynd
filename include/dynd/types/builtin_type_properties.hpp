//
// Copyright (C) 2011-14 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/types/type_id.hpp>
#include <dynd/func/callable.hpp>

namespace dynd {

void get_builtin_type_dynamic_array_properties(
                type_id_t builtin_type_id,
                const std::pair<std::string, gfunc::callable> **out_properties,
                size_t *out_count);

size_t get_builtin_type_elwise_property_index(
                type_id_t builtin_type_id,
                const std::string& property_name);

ndt::type get_builtin_type_elwise_property_type(
                type_id_t builtin_type_id,
                size_t elwise_property_index,
                bool& out_readable, bool& out_writable);

size_t make_builtin_type_elwise_property_getter_kernel(
                void *ckb, intptr_t ckb_offset,
                type_id_t builtin_type_id,
                const char *dst_arrmeta,
                const char *src_arrmeta, size_t src_elwise_property_index,
                kernel_request_t kernreq, const eval::eval_context *ectx);

size_t make_builtin_type_elwise_property_setter_kernel(
                void *ckb, intptr_t ckb_offset,
                type_id_t builtin_type_id,
                const char *dst_arrmeta, size_t dst_elwise_property_index,
                const char *src_arrmeta,
                kernel_request_t kernreq, const eval::eval_context *ectx);

namespace init {
  void builtins_type_init();
  void builtins_type_cleanup();
} // namespace init

} // namespace dynd
