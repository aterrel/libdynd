//
// Copyright (C) 2011-14 Mark Wiebe, DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#include <iostream>
#include <sstream>
#include <stdexcept>
#include "inc_gtest.hpp"
#include "dynd_assertions.hpp"

#include <dynd/array.hpp>
#include <dynd/view.hpp>
#include <dynd/types/option_type.hpp>
#include <dynd/types/string_type.hpp>
#include <dynd/json_parser.hpp>

using namespace std;
using namespace dynd;

TEST(OptionType, Create) {
    ndt::type d;

    d = ndt::make_option<int16_t>();
    EXPECT_EQ(option_type_id, d.get_type_id());
    EXPECT_EQ(option_kind, d.get_kind());
    EXPECT_EQ(2u, d.get_data_alignment());
    EXPECT_EQ(2u, d.get_data_size());
    EXPECT_EQ(ndt::make_type<int16_t>(),
              d.tcast<option_type>()->get_value_type());
    EXPECT_TRUE(d.is_scalar());
    EXPECT_FALSE(d.is_expression());
    // Roundtripping through a string
    EXPECT_EQ(d, ndt::type(d.str()));
    EXPECT_EQ("?int16", d.str());
    EXPECT_EQ(d, ndt::type("?int16"));
    EXPECT_EQ(d, ndt::type("option[int16]"));

    d = ndt::make_option(ndt::make_string());
    EXPECT_EQ(option_type_id, d.get_type_id());
    EXPECT_EQ(option_kind, d.get_kind());
    EXPECT_EQ(ndt::make_string().get_data_alignment(), d.get_data_alignment());
    EXPECT_EQ(ndt::make_string().get_data_size(), d.get_data_size());
    EXPECT_EQ(ndt::make_string(), d.tcast<option_type>()->get_value_type());
    EXPECT_FALSE(d.is_expression());
    // Roundtripping through a string
    EXPECT_EQ(d, ndt::type(d.str()));
    EXPECT_EQ("?string", d.str());

    // No option of option allowed
    EXPECT_THROW(ndt::make_option(ndt::make_option(ndt::make_type<int>())), type_error);
    EXPECT_THROW(ndt::type("option[option[bool]]"), type_error);
}

TEST(OptionType, OptionIntAssign) {
    nd::array a, b, c;
    eval::eval_context tmp_ectx;

    // Assignment from option[S] to option[T]
    a = parse_json("2 * ?int8", "[-10, null]");
    b = nd::empty("2 * ?int16");
    b.vals() = a;
    EXPECT_JSON_EQ_ARR("[-10, -32768]", nd::view(b, "2 * int16"));
    tmp_ectx.errmode = assign_error_none;
    b.val_assign(a, &tmp_ectx);
    EXPECT_JSON_EQ_ARR("[-10, -32768]", nd::view(b, "2 * int16"));

    // Assignment from option[T] to T without any NAs
    a = parse_json("3 * ?int32", "[1, 2, 3]");
    b = nd::empty("3 * int32");
    b.vals() = a;
    EXPECT_ARR_EQ(nd::view(a, "3 * int32"), b);

    // Assignment from T to option[T]
    a = parse_json("3 * int32", "[1, 3, 5]");
    b = nd::empty("3 * ?int32");
    b.vals() = a;
    EXPECT_ARR_EQ(a, nd::view(b, "3 * int32"));

    // Assignment from string to option[int]
    a = parse_json("5 * string", "[\"null\", \"12\", \"NA\", \"34\", \"\"]");
    b = nd::empty("5 * ?int32");
    b.vals() = a;
    c = parse_json("5 * ?int32", "[null, 12, null, 34, null]");
    EXPECT_ARR_EQ(nd::view(c, "strided * int32"),
                  nd::view(b, "strided * int32"));
}

TEST(OptionType, Cast) {
    nd::array a, b;

    a = parse_json("3 * string", "[\"null\", \"NA\", \"25\"]");
    b = a.ucast(ndt::type("?int"));
    b = b.eval();
    EXPECT_ARR_EQ(
        nd::view(parse_json("3 * ?int", "[null, null, 25]"), "3 * int"),
        nd::view(b, "3 * int"));
}

TEST(OptionType, Date) {
    nd::array a;

    a = parse_json("5 * ?date",
                   "[null, \"2013-04-05\", \"NA\", \"\", \"Jan 3, 2020\"]");
    EXPECT_FALSE(nd::is_scalar_avail(a(0)));
    EXPECT_EQ("2013-04-05", a(1).as<string>());
    EXPECT_FALSE(nd::is_scalar_avail(a(2)));
    EXPECT_FALSE(nd::is_scalar_avail(a(3)));
    EXPECT_EQ("2020-01-03", a(4).as<string>());
}

TEST(OptionType, Time) {
    nd::array a;

    a = parse_json("5 * ?time",
                   "[null, \"3:45\", \"NA\", \"\", \"05:17:33.1234 PM\"]");
    EXPECT_FALSE(nd::is_scalar_avail(a(0)));
    EXPECT_EQ("03:45", a(1).as<string>());
    EXPECT_FALSE(nd::is_scalar_avail(a(2)));
    EXPECT_FALSE(nd::is_scalar_avail(a(3)));
    EXPECT_EQ("17:17:33.1234", a(4).as<string>());
}

TEST(OptionType, DateTime) {
    nd::array a;

    a = parse_json("5 * ?datetime", "[null, \"2013-04-05 3:45\", \"NA\", \"\","
                                    " \"Jan 3, 2020 05:17:33.1234 PM\"]");
    EXPECT_FALSE(nd::is_scalar_avail(a(0)));
    EXPECT_EQ("2013-04-05T03:45", a(1).as<string>());
    EXPECT_FALSE(nd::is_scalar_avail(a(2)));
    EXPECT_FALSE(nd::is_scalar_avail(a(3)));
    EXPECT_EQ("2020-01-03T17:17:33.1234", a(4).as<string>());
}