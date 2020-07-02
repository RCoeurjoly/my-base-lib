#pragma once
#include "impl/scope_guard.hpp"

#define DEFER_CONCATENATE_IMPL(x, y) x##y
#define DEFER_CONCATENATE(x, y) DEFER_CONCATENATE_IMPL(x, y)
#define defer(...) auto DEFER_CONCATENATE(__my_scoped_guard_, __COUNTER__) = sg::make_scope_guard((__VA_ARGS__))
//#define defer(fn) auto DEFER_CONCATENATE(__my_scoped_guard_, __COUNTER__) = sg::make_scope_guard((fn))
