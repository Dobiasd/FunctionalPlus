// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <fplus/shared_ref.hpp>

namespace fplus
{

// A generic RAII class.
// It is recommended to use make_shared_ref for construction an instance.
template <typename INIT, typename QUIT>
class raii
{
public:
    raii(INIT init, QUIT quit) :
        quit_(quit)
    {
        init();
    }
    ~raii()
    {
        quit_();
    }
    raii(const raii&) = delete;
    raii(raii&&) = default;
    raii& operator=(const raii&) = delete;
    raii& operator=(raii&&) = default;
private:
    QUIT quit_;
};

template <typename INIT, typename QUIT>
shared_ref<raii<INIT, QUIT>> make_raii(INIT init, QUIT quit)
{
    return make_shared_ref<raii<INIT, QUIT>>(init, quit);
}


} // namespace fplus
