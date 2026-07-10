#ifndef FIXEDCAPACITYARRAY_HPP
#define FIXEDCAPACITYARRAY_HPP
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

/// A stack-allocated, append-only container with a compile-time capacity --
/// no heap allocation, unlike std::vector.
///
/// C++17 has no language feature for constraining a template parameter, so
/// the requirement that T can live in a value-initialized std::array and be
/// reassigned by push() is enforced below with static_assert + <type_traits>.
/// In C++20 this would instead be a `requires` clause on the template head,
/// e.g.:
///   template<typename T, std::size_t Capacity>
///   requires std::default_initializable<T> && std::copyable<T>
///   class FixedCapacityArray { ... };
/// which rejects a bad T right at the template declaration with a short,
/// readable diagnostic, instead of failing deep inside push() or the
/// std::array member the first time the template gets instantiated.
template<typename T, std::size_t Capacity>
class FixedCapacityArray final
{
    static_assert(std::is_default_constructible<T>::value,
                  "FixedCapacityArray<T, Capacity> requires T to be default-constructible "
                  "(the backing std::array value-initializes all Capacity slots up front)");
    static_assert(std::is_copy_assignable<T>::value,
                  "FixedCapacityArray<T, Capacity> requires T to be copy-assignable "
                  "(push() assigns the new value into storage)");

  public:
    static constexpr std::size_t kCapacity{Capacity};

    /// Returns false (and leaves the container unchanged) instead of writing
    /// past Capacity or relying on std::array::at() to throw -- many embedded
    /// toolchains build with -fno-exceptions, where an uncaught
    /// std::out_of_range calls std::terminate rather than being catchable.
    /// [[nodiscard]] means a caller that ignores the result gets a compiler
    /// warning instead of silently losing data.
    ///
    /// Takes v by const reference, not by value: T (SensorValue) is
    /// trivially copyable, so std::move on it degrades to a plain memberwise
    /// copy -- there's no user-defined move constructor to actually elide
    /// anything. That means a by-value-then-move signature would do TWO
    /// copies (caller's temporary -> parameter v -> values[count]), while
    /// const& does exactly ONE (caller's temporary -> values[count]
    /// directly). By-value-plus-move only pays off for types with real
    /// (non-trivial) move semantics, like std::string/std::vector.
    [[nodiscard]] bool push(T const& v)
    {
        if (count >= Capacity)
        {
            return false;
        }
        /// operator[], not at(): count < Capacity is already proven by the
        /// guard above, so this index is always in range -- at() would add
        /// an exception-throwing path (see the [[nodiscard]] note above) for
        /// a bound that can never actually be violated here.
        values[count] = v;
        ++count;
        return true;
    }

    auto at(std::size_t i) const -> T const& { return values.at(i); }
    std::size_t size() const noexcept { return count; }

  private:
    std::array<T, Capacity> values{};
    std::size_t count{0U};
};

#endif