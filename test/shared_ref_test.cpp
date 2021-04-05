// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

namespace
{
    std::vector<std::string> logs;

    void log(const std::string& str)
    {
        logs.push_back(str);
    }

    struct test
    {
        int m_x;

        test(int x)         :m_x(x)                 { log("test(" + fplus::show(m_x) + ")"); }
        test(const test& t) :m_x(t.m_x)             { log("test(const test& " + fplus::show(m_x) + ")"); }
        test(test&& t)      :m_x(std::move(t.m_x))  { log("test(test&& " + fplus::show(m_x) + ")"); }

        test& operator=(int x)          { m_x = x;                  log("test::operator=(" + fplus::show(m_x) + ")"); return *this;}
        test& operator=(const test& t)  { m_x = t.m_x;              log("test::operator=(const test& " + fplus::show(m_x) + ")"); return *this;}
        test& operator=(test&& t)       { m_x = std::move(t.m_x);   log("test::operator=(test&& " + fplus::show(m_x) + ")"); return *this;}

        ~test()             { log("~test(" + fplus::show(m_x) + ")"); }
    };
}

TEST_CASE("shared_ref_test - full")
{
    using namespace fplus;

    {
        auto ref = make_shared_ref<test>(1);
        auto ref2 = ref;

        *ref2 = test(5);
    }
    {
        test o(2);
        auto ref = make_shared_ref<test>(std::move(o));
    }

    const std::vector<std::string> logs_dest = {
        "test(1)",
        "test(5)",
        "test::operator=(test&& 5)",
        "~test(5)",
        "~test(5)",
        "test(2)",
        "test(test&& 2)",
        "~test(2)",
        "~test(2)"
    };

    REQUIRE_EQ(logs, logs_dest);
}
