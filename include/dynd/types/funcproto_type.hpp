//
// Copyright (C) 2011-14 Mark Wiebe, Irwin Zaid, DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _DYND__FUNCPROTO_TYPE_HPP_
#define _DYND__FUNCPROTO_TYPE_HPP_

#include <vector>
#include <string>

#include <dynd/array.hpp>
#include <dynd/buffer.hpp>
#include <dynd/funcproto.hpp>
#include <dynd/pp/comparison.hpp>
#include <dynd/pp/list.hpp>
#include <dynd/pp/meta.hpp>
#include <dynd/types/fixed_dimsym_type.hpp>

namespace dynd {

class funcproto_type : public base_type {
    intptr_t m_param_count;
    intptr_t m_aux_param_count;
    // This is always a contiguous immutable "N * type" array
    nd::array m_param_types;
    ndt::type m_return_type;

public:
    funcproto_type(const nd::array &param_types, const ndt::type &return_type, intptr_t aux_param_count = 0);

    virtual ~funcproto_type() {}

    intptr_t get_param_count() const {
        return m_param_count;
    }

    intptr_t get_nsrc() const {
        return m_param_count - m_aux_param_count;
    }

    intptr_t get_naux() const {
        return m_aux_param_count;
    }

    intptr_t get_narg() const {
        return m_param_count;
    }

    intptr_t get_aux_param_count() const {
        return m_aux_param_count;
    }

    const nd::array& get_param_types() const {
        return m_param_types;
    }

    const ndt::type *get_param_types_raw() const {
        return reinterpret_cast<const ndt::type *>(
            m_param_types.get_readonly_originptr());
    }
    const ndt::type &get_param_type(intptr_t i) const {
        return get_param_types_raw()[i];
    }

    const ndt::type& get_return_type() const {
        return m_return_type;
    }

    void print_data(std::ostream& o, const char *arrmeta, const char *data) const;

    void print_type(std::ostream& o) const;

    void transform_child_types(type_transform_fn_t transform_fn,
                               intptr_t arrmeta_offset, void *extra,
                               ndt::type &out_transformed_tp,
                               bool &out_was_transformed) const;
    ndt::type get_canonical_type() const;

    ndt::type apply_linear_index(intptr_t nindices, const irange *indices,
                size_t current_i, const ndt::type& root_tp, bool leading_dimension) const;
    intptr_t apply_linear_index(intptr_t nindices, const irange *indices, const char *arrmeta,
                    const ndt::type& result_tp, char *out_arrmeta,
                    memory_block_data *embedded_reference,
                    size_t current_i, const ndt::type& root_tp,
                    bool leading_dimension, char **inout_data,
                    memory_block_data **inout_dataref) const;

    bool is_lossless_assignment(const ndt::type& dst_tp, const ndt::type& src_tp) const;

    bool operator==(const base_type& rhs) const;

    void arrmeta_default_construct(char *arrmeta, bool blockref_alloc) const;
    void arrmeta_copy_construct(char *dst_arrmeta, const char *src_arrmeta, memory_block_data *embedded_reference) const;
    void arrmeta_destruct(char *arrmeta) const;

    void get_dynamic_type_properties(
        const std::pair<std::string, gfunc::callable> **out_properties,
        size_t *out_count) const;
}; // class typevar_type

namespace ndt {
    /** Makes a funcproto type with the specified types */
    inline ndt::type make_funcproto(const nd::array &param_types,
                                    const ndt::type &return_type,
                                    intptr_t aux_param_count = 0)
    {
        return ndt::type(
            new funcproto_type(param_types, return_type, aux_param_count), false);
    }

    /** Makes a funcproto type with the specified types */
    inline ndt::type make_funcproto(intptr_t param_count,
                                    const ndt::type *param_types,
                                    const ndt::type &return_type)
    {
        nd::array tmp = nd::empty(param_count, ndt::make_type());
        ndt::type *tmp_vals =
            reinterpret_cast<ndt::type *>(tmp.get_readwrite_originptr());
        for (intptr_t i = 0; i != param_count; ++i) {
            tmp_vals[i] = param_types[i];
        }
        tmp.flag_as_immutable();
        return ndt::type(
            new funcproto_type(tmp, return_type), false);
    }

    /** Makes a unary funcproto type with the specified types */
    inline ndt::type make_funcproto(const ndt::type& single_param_type,
                                    const ndt::type &return_type)
    {
        ndt::type param_types[1] = {single_param_type};
        return ndt::type(
            new funcproto_type(param_types, return_type), false);
    }

namespace detail {

#define PARTIAL_DECAY(TYPENAME) std::remove_cv<typename std::remove_reference<TYPENAME>::type>::type
#define MAKE_TYPE(TYPENAME) make_type<TYPENAME>()

template <typename func_type>
struct funcproto_type_factory;

#define FUNCPROTO_TYPE_FACTORY_MAKE(OLD_NARG, NEW_NARG) \
    DYND_PP_IF(DYND_PP_NE(NEW_NARG, OLD_NARG))(template <DYND_PP_JOIN_MAP_2(DYND_PP_META_TYPENAME, (,), DYND_PP_META_NAME_RANGE(A, OLD_NARG, NEW_NARG))>) \
    static ndt::type make(intptr_t naux) { \
        DYND_PP_JOIN_ELWISE_2(DYND_PP_META_TYPEDEF_TYPENAME, (;), \
            DYND_PP_MAP_2(PARTIAL_DECAY, DYND_PP_META_NAME_RANGE(A, NEW_NARG)), DYND_PP_META_NAME_RANGE(D, NEW_NARG)); \
        DYND_PP_IF_ELSE(NEW_NARG)( \
            ndt::type arg_tp[NEW_NARG] = {DYND_PP_JOIN_MAP_2(MAKE_TYPE, (,), DYND_PP_META_NAME_RANGE(D, NEW_NARG))}; \
        )( \
            nd::array arg_tp = nd::empty(0, ndt::make_type()); \
            arg_tp.flag_as_immutable(); \
        ) \
        return make_funcproto(arg_tp, make_type<R>(), naux); \
    }

#define FUNCPROTO_TYPE_FACTORY(OLD_NARG) \
    template <DYND_PP_JOIN_MAP_1(DYND_PP_META_TYPENAME, (,), DYND_PP_PREPEND(R, DYND_PP_META_NAME_RANGE(A, OLD_NARG)))> \
    struct funcproto_type_factory<R DYND_PP_META_NAME_RANGE(A, OLD_NARG)> { \
        DYND_PP_JOIN_ELWISE_1(FUNCPROTO_TYPE_FACTORY_MAKE, (), \
            DYND_PP_REPEAT_1(OLD_NARG, DYND_PP_INC(DYND_PP_SUB(DYND_ARG_MAX, OLD_NARG))), DYND_PP_RANGE(OLD_NARG, DYND_PP_INC(DYND_ARG_MAX))) \
    };

DYND_PP_JOIN_MAP(FUNCPROTO_TYPE_FACTORY, (), DYND_PP_RANGE(DYND_PP_INC(DYND_ARG_MAX)))

#undef FUNCPROTO_TYPE_FACTORY

#define FUNCPROTO_TYPE_FACTORY(OLD_NARG) \
    template <DYND_PP_JOIN_MAP_1(DYND_PP_META_TYPENAME, (,), DYND_PP_PREPEND(R, DYND_PP_META_NAME_RANGE(A, OLD_NARG)))> \
    struct funcproto_type_factory<void DYND_PP_PREPEND(R &, DYND_PP_META_NAME_RANGE(A, OLD_NARG))> { \
        DYND_PP_JOIN_ELWISE_1(FUNCPROTO_TYPE_FACTORY_MAKE, (), \
            DYND_PP_REPEAT_1(OLD_NARG, DYND_PP_INC(DYND_PP_SUB(DYND_ARG_MAX, OLD_NARG))), DYND_PP_RANGE(OLD_NARG, DYND_PP_INC(DYND_ARG_MAX))) \
    };

DYND_PP_JOIN_MAP(FUNCPROTO_TYPE_FACTORY, (), DYND_PP_RANGE(DYND_PP_INC(DYND_ARG_MAX)))

#undef FUNCPROTO_TYPE_FACTORY

#undef FUNCPROTO_TYPE_FACTORY_MAKE

#undef PARTIAL_DECAY
#undef MAKE_TYPE

} // namespace detail

template <typename func_type>
ndt::type make_funcproto(intptr_t naux = 0) {
    return detail::funcproto_type_factory<func_type>::make(naux);
}

#define MAKE_FUNCPROTO(NARG) \
    template <DYND_PP_JOIN_MAP_1(DYND_PP_META_TYPENAME, (,), DYND_PP_PREPEND(func_type, DYND_PP_META_NAME_RANGE(A, NARG)))> \
    ndt::type make_funcproto(intptr_t naux = 0) { \
        return detail::funcproto_type_factory<func_type>::template make<DYND_PP_JOIN_1((,), DYND_PP_META_NAME_RANGE(A, NARG))>(naux); \
    }

DYND_PP_JOIN_MAP(MAKE_FUNCPROTO, (), DYND_PP_RANGE(1, DYND_PP_INC(DYND_ARG_MAX)))

#undef MAKE_FUNCPROTO

ndt::type make_generic_funcproto(intptr_t nargs);

} // namespace ndt

} // namespace dynd

#endif // _DYND__FUNCPROTO_TYPE_HPP_
