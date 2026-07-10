#ifndef SENSORVALUE_HPP
#define SENSORVALUE_HPP
#include <string_view>

/// type/unit are always short, static-storage-duration string literals
/// ("Temperature", "Celsius", ...) supplied by the decoders -- std::string_view
/// (C++17) references them with no allocation and no copying, unlike
/// std::string. Before C++17 this project used a small hand-rolled
/// StaticStringView doing the same job (a pointer + strcmp-based equality),
/// since std::string_view didn't exist yet; now that the project targets
/// C++17, the standard type replaces it directly, including a more efficient
/// operator== (size check before any byte comparison, rather than an
/// unconditional strcmp).

/// Distinct single-member wrapper types around std::string_view so that
/// SensorValue's constructor can't have its type_/unit_ arguments swapped by
/// mistake -- SensorType and SensorUnit are different types, so passing one
/// where the other is expected is a compile error instead of a silent
/// mislabeled reading (e.g. a temperature reading whose "type" ends up being
/// "Celsius" and whose "unit" ends up being "Temperature").
struct SensorType final
{
    std::string_view value;
};

struct SensorUnit final
{
    std::string_view value;
};

class SensorValue final
{
  public:
    SensorValue() = default;
    SensorValue(double val_, SensorType type_, SensorUnit unit_)
        : value{val_}, type{type_.value}, unit{unit_.value}
    {}

    double getValue() const noexcept { return value; }
    std::string_view getType() const noexcept { return type; }
    std::string_view getUnit() const noexcept { return unit; }

  private:
    double value{0.0};
    std::string_view type{};
    std::string_view unit{};
};

#endif