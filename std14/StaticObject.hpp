#include <new>
#include <utility>

template<class Obj>
struct StaticObject {
public:
    StaticObject() noexcept = default;
    StaticObject(const StaticObject&) = delete;
    StaticObject(StaticObject&&) = delete;
    template<class... Args>
    constexpr void Init(Args&&... args) noexcept {
        ::new(&_data) Obj(::std::forward<Args>(args)...);
    }
    constexpr Obj& operator *() noexcept {
        return *reinterpret_cast<Obj*>(&_data);
    }
    constexpr const Obj& operator *() const noexcept {
        return *reinterpret_cast<const Obj*>(&_data);
    }
    constexpr Obj* operator ->() noexcept {
        return reinterpret_cast<Obj*>(&_data);
    }
    constexpr const Obj* operator ->() const noexcept {
        return reinterpret_cast<const Obj*>(&_data);
    }
private:
    alignas(alignof(Obj)) unsigned char _data[sizeof(Obj)];
};
