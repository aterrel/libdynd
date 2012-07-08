//
// Copyright (C) 2011-12, Dynamic NDArray Developers
// BSD 2-Clause License, see LICENSE.txt
//

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include "inc_gtest.hpp"

#include <dnd/ndarray.hpp>
#include <dnd/dtypes/string_dtype.hpp>
#include <dnd/dtypes/fixedstring_dtype.hpp>
#include <dnd/dtypes/conversion_dtype.hpp>

using namespace std;
using namespace dnd;

TEST(StringDType, Create) {
    dtype d;

    // Strings with various encodings and sizes
    d = make_string_dtype(string_encoding_utf_8);
    EXPECT_EQ(string_type_id, d.type_id());
    EXPECT_EQ(string_kind, d.kind());
    EXPECT_EQ(sizeof(void *), d.alignment());
    EXPECT_EQ(2*sizeof(void *), d.element_size());

    d = make_string_dtype(string_encoding_utf_8);
    EXPECT_EQ(string_type_id, d.type_id());
    EXPECT_EQ(string_kind, d.kind());
    EXPECT_EQ(sizeof(void *), d.alignment());
    EXPECT_EQ(2*sizeof(void *), d.element_size());

    d = make_string_dtype(string_encoding_ascii);
    EXPECT_EQ(string_type_id, d.type_id());
    EXPECT_EQ(string_kind, d.kind());
    EXPECT_EQ(sizeof(void *), d.alignment());
    EXPECT_EQ(2*sizeof(void *), d.element_size());

    d = make_string_dtype(string_encoding_utf_16);
    EXPECT_EQ(string_type_id, d.type_id());
    EXPECT_EQ(string_kind, d.kind());
    EXPECT_EQ(sizeof(void *), d.alignment());
    EXPECT_EQ(2*sizeof(void *), d.element_size());

    d = make_string_dtype(string_encoding_utf_32);
    EXPECT_EQ(string_type_id, d.type_id());
    EXPECT_EQ(string_kind, d.kind());
    EXPECT_EQ(sizeof(void *), d.alignment());
    EXPECT_EQ(2*sizeof(void *), d.element_size());
}

TEST(StringDType, Basic) {
    ndarray a, b;

    // std::string goes in as a utf8 fixed string
    a = std::string("abcdefg");
    EXPECT_EQ(make_fixedstring_dtype(string_encoding_utf_8, 7), a.get_dtype());
    EXPECT_EQ(std::string("abcdefg"), a.as<std::string>());

    // Convert to a blockref string dtype with the same utf8 codec
    b = a.as_dtype(make_string_dtype(string_encoding_utf_8));
    EXPECT_EQ(make_conversion_dtype(make_string_dtype(string_encoding_utf_8), make_fixedstring_dtype(string_encoding_utf_8, 7)),
                b.get_dtype());
    b = b.vals();
    EXPECT_EQ(make_string_dtype(string_encoding_utf_8),
                    b.get_dtype());
    EXPECT_EQ(std::string("abcdefg"), b.as<std::string>());

    // Convert to a blockref string dtype with the utf16 codec
    b = a.as_dtype(make_string_dtype(string_encoding_utf_16));
    EXPECT_EQ(make_conversion_dtype(make_string_dtype(string_encoding_utf_16), make_fixedstring_dtype(string_encoding_utf_8, 7)),
                b.get_dtype());
    b = b.vals();
    EXPECT_EQ(make_string_dtype(string_encoding_utf_16),
                    b.get_dtype());
    EXPECT_EQ(std::string("abcdefg"), b.as<std::string>());

    // Convert to a blockref string dtype with the utf32 codec
    b = a.as_dtype(make_string_dtype(string_encoding_utf_32));
    EXPECT_EQ(make_conversion_dtype(make_string_dtype(string_encoding_utf_32), make_fixedstring_dtype(string_encoding_utf_8, 7)),
                b.get_dtype());
    b = b.vals();
    EXPECT_EQ(make_string_dtype(string_encoding_utf_32),
                    b.get_dtype());
    EXPECT_EQ(std::string("abcdefg"), b.as<std::string>());

    // Convert to a blockref string dtype with the ascii codec
    b = a.as_dtype(make_string_dtype(string_encoding_ascii));
    EXPECT_EQ(make_conversion_dtype(make_string_dtype(string_encoding_ascii), make_fixedstring_dtype(string_encoding_utf_8, 7)),
                b.get_dtype());
    b = b.vals();
    EXPECT_EQ(make_string_dtype(string_encoding_ascii),
                    b.get_dtype());
    EXPECT_EQ(std::string("abcdefg"), b.as<std::string>());
}
