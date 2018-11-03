// Heap
#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <utility>

template<::std::size_t Cap_, class Key_, class Cmp_ = ::std::greater<Key_>>
struct AlgoHeap {
    static constexpr auto Cap = Cap_;
    using Key = Key_;
    using Cmp = Cmp_;
    Key Dat[Cap]{};
    ::std::size_t Size = 0;
    constexpr bool IsEmpty() const noexcept {
        return !Size;
    }
    constexpr void Clear() noexcept {
        Size = 0;
    }
    constexpr const Key& Root() const noexcept {
        return Dat[0];
    }
    template<class Arg>
    inline void Push(Arg&& key) noexcept {
        Dat[Size++] = ::std::forward<Arg>(key);
        ::std::push_heap(Dat, Dat + Size, Cmp{});
    }
    inline const Key& Extract() noexcept {
        ::std::pop_heap(Dat, Dat + Size, Cmp{});
        return Dat[Size--];
    }
};

template<::std::size_t Cap_, class Id_, class Key_>
struct KeyedHeap {
    static constexpr auto Cap = Cap_;
    using Id = Id_;
    using Key = Key_;
    struct Stor {
        Id id;
        Key key;
    } Dat[Cap]{};
    ::std::size_t Idx[Cap]{};
    ::std::size_t Size = 0;
    constexpr bool IsEmpty() const noexcept {
        return !Size;
    }
    constexpr void Clear() noexcept {
        Size = 0;
    }
    constexpr const Key& RootKey() const noexcept {
        return At(1).key;
    }
    constexpr bool Has(Id id) const noexcept {
        return !!Idx[id];
    }
    constexpr Id Root() const noexcept {
        return At(1).id;
    }
    template<class Arg>
    constexpr void Push(Id id, Arg&& key) noexcept {
        DecKey(id, ::std::forward<Arg>(key));
    }
    template<class Arg>
    constexpr void DecKey(Id id, Arg&& key) noexcept {
        auto idx = Has(id) ? Idx[id] : ++Size;
        while (idx != 1) {
            auto upp = idx >> 1;
            if (At(upp).key < ::std::forward<Arg>(key))
                break;
            At(idx) = ::std::move(At(upp));
            Idx[At(idx).id] = idx;
            idx = upp;
        }
        At(idx).key = ::std::forward<Arg>(key);
        At(idx).id = id;
        Idx[id] = idx;
    }
    constexpr Id Extract() noexcept {
        auto id = At(1).id;
        Idx[id] = 0;
        auto idx = ::std::size_t{1};
        auto& dat = At(Size--);
        while ((idx << 1) <= Size) {
            auto lch = idx << 1;
            auto rch = lch + 1;
            if (!(At(lch).key < dat.key) && (rch > Size || !(At(rch).key < dat.key)))
                break;
            if (rch <= Size && At(rch).key < At(lch).key)
                lch = rch;
            At(idx) = ::std::move(At(lch));
            Idx[At(idx).id] = idx;
            idx = lch;
        }
        At(idx) = ::std::move(dat);
        Idx[At(idx).id] = idx;
        return id;
    }
    constexpr Stor& At(::std::size_t idx) noexcept {
        return Dat[idx - 1];
    }
    constexpr const Stor& At(::std::size_t idx) const noexcept {
        return Dat[idx - 1];
    }
};
