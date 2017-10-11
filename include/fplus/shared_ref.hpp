// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <memory>

namespace fplus
{

// A std::shared_ptr expresses
// optionality of the contained value (can be nullptr)
// and shared ownership that can be transferred.
// A std::optional expresses optionality only.
// The standard does not provide a class to
// express only shared ownership without optionality.
// shared_ref fills this gap.
template <typename T>
class shared_ref
{
public:
    shared_ref(const shared_ref&) = default;
    shared_ref(shared_ref&&) = default;
    shared_ref& operator=(const shared_ref&) = default;
    shared_ref& operator=(shared_ref&&) = default;
    ~shared_ref() = default;

    T* operator->() { return m_ptr.get(); }
    const T* operator->() const { return m_ptr.get(); }

    T& operator*() { return *m_ptr.get(); }
    const T& operator*() const { return *m_ptr.get(); }

    template <typename XT, typename...XTypes>
    friend shared_ref<XT> make_shared_ref(XTypes&&...args);

private:
    std::shared_ptr<T> m_ptr;
    shared_ref(T* value) :m_ptr(value) { assert(value != nullptr);  }
};

// http://stackoverflow.com/a/41976419/1866775
template <typename T, typename...Types>
shared_ref<T> make_shared_ref(Types&&...args)
{
    return shared_ref<T>(new T(std::forward<Types>(args)...));
}

} // namespace fplus
