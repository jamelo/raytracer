#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <builders/ParamTypes.hpp>
#include <Exceptions.hpp>

namespace builders
{

    template <typename T1, typename T2>
    struct static_or : public std::integral_constant<bool, T1::value || T2::value> { };

    template <typename T, typename ... Candidates>
    struct is_one_of;

    template <typename T, typename First, typename ... Rest>
    struct is_one_of<T, First, Rest...> : public static_or<std::is_same<T, First>, is_one_of<T, Rest...>> { };

    template <typename T, typename First>
    struct is_one_of<T, First> : public std::is_same<T, First> { };

    template <typename Target, typename std::enable_if<is_one_of<Target, ParamTypes::Colour, ParamTypes::Point3, ParamTypes::Vector3>::value, int>::type = 0>
    Target paramCast(const ParamTypes::IntegerList& arg)
    {
        if (arg.size() != 3) {
            throw InvalidArrayConversionException(arg, arg.size(), ParamTypes::Index<Target>::value, 3);
        }

        return Target(arg[0], arg[1], arg[2]);
    }

    template <typename Target, typename std::enable_if<is_one_of<Target, ParamTypes::Colour, ParamTypes::Point3, ParamTypes::Vector3>::value, int>::type = 0>
    Target paramCast(const ParamTypes::FloatList& arg)
    {
        if (arg.size() != 3) {
            throw InvalidArrayConversionException(arg, arg.size(), ParamTypes::Index<Target>::value, 3);
        }

        return Target(arg[0], arg[1], arg[2]);
    }

    template <typename Target, typename std::enable_if<is_one_of<Target, ParamTypes::Point2, ParamTypes::Vector2>::value, int>::type = 0>
    Target paramCast(const ParamTypes::IntegerList& arg)
    {
        if (arg.size() != 2) {
            throw InvalidArrayConversionException(arg, arg.size(), ParamTypes::Index<Target>::value, 2);
        }

        return Target(arg[0], arg[1]);
    }

    template <typename Target, typename std::enable_if<is_one_of<Target, ParamTypes::Point2, ParamTypes::Vector2>::value, int>::type = 0>
    Target paramCast(const ParamTypes::FloatList& arg)
    {
        if (arg.size() != 2) {
            throw InvalidArrayConversionException(arg, arg.size(), ParamTypes::Index<Target>::value, 2);
        }

        return Target(arg[0], arg[1]);
    }

    template <typename Target, typename Source,
    typename std::enable_if<!is_one_of<Target, ParamTypes::Colour, ParamTypes::Point3, ParamTypes::Vector3>::value, int>::type = 0>
    Target paramCast(const Source& arg);

    template <>
    ParamTypes::Identifier paramCast<ParamTypes::Identifier>(const ParamTypes::String& arg)
    {
        return ParamTypes::Identifier(arg);
    }

    template <>
    ParamTypes::ImageSize paramCast<ParamTypes::ImageSize>(const ParamTypes::IntegerList& arg)
    {
        if (arg.size() != 2) {
            throw InvalidArrayConversionException(arg, arg.size(), ParamType::eImageSize, 2);
        }

        return ParamTypes::ImageSize(arg[0], arg[1]);
    }

    template <>
    ParamTypes::Float paramCast<ParamTypes::Float>(const ParamTypes::Integer& arg)
    {
        return static_cast<ParamTypes::Float>(arg);
    }

    template <typename Target, typename Source>
    Target paramCast(const ParamValue& arg) try
    {
        return paramCast<Target>(boost::get<Source>(arg));
    }
    catch (boost::bad_get ex)
    {
        throw InvalidConversionException(arg, ParamTypes::Index<Target>::value);
    }

}

#endif
