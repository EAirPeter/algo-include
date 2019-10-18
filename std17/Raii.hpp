// RAII Features
#pragma once

#include <functional>
#include <utility>

namespace sqwh {

template<class Ty>
class DelayedTask {
public:
  using Task = TyTask;
private:
  Task x_fnTask;
public:
  DelayedTask() = delete;
  DelayedTask(const DelayedTask&) = delete;
  DelayedTask(DelayedTask&&) = delete;
  template<class Ty_>
  DelayedTask(Ty_&& fnTask) noexcept :
    x_fnTask(::std::forward<Ty_>(fnTask)) {}
  ~DelayedTask() {
    ::std::invoke(x_fnTask);
  }
};

template<class TyRes, class TyRel>
class AutoRelease {
public:
  using Resource = TyRes;
  using Releaser = TyRel;
public:
  AutoRelease(tResource, tRelease
private:
  tResource x_vRes;
};

}
