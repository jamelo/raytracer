#ifndef PARAM_TYPES_HPP
#define PARAM_TYPES_HPP

#include <functional>
#include <map>
#include <string>
#include <type_traits>
#include <boost/variant.hpp>

#include <Camera.hpp>
#include <geometry/Point.hpp>
#include <geometry/Vector.hpp>
#include <graphics/Colour.hpp>
#include <shapes/Shape.hpp>
#include <Surface.hpp>

namespace builders
{
    class BuilderArgs;

    enum class ParamType
    {
        eBoolean,
        eCamera,
        eColour,
        eFloat,
        eFloatList,
        eIdentifier,
        eImageSize,
        eInteger,
        eIntegerList,
        eObject,
        ePoint2,
        ePoint3,
        eShape,
        eShapeMap,
        eString,
        eSurface,
        eSurfaceMap,
        eVector2,
        eVector3
    };

    //TODO: move this somewhere better
    inline std::string stringify(builders::ParamType type)
    {
        switch (type)
        {
            case builders::ParamType::eBoolean:     return "Boolean";
            case builders::ParamType::eCamera:      return "Camera";
            case builders::ParamType::eColour:      return "Colour";
            case builders::ParamType::eFloat:       return "Float";
            case builders::ParamType::eFloatList:   return "FloatList";
            case builders::ParamType::eIdentifier:  return "Identifier";
            case builders::ParamType::eImageSize:   return "ImageSize";
            case builders::ParamType::eInteger:     return "Integer";
            case builders::ParamType::eIntegerList: return "IntegerList";
            case builders::ParamType::eObject:      return "Object";
            case builders::ParamType::ePoint2:      return "Point2";
            case builders::ParamType::ePoint3:      return "Point3";
            case builders::ParamType::eShape:       return "Shape";
            case builders::ParamType::eShapeMap:    return "ShapeMap";
            case builders::ParamType::eString:      return "String";
            case builders::ParamType::eSurface:     return "Surface";
            case builders::ParamType::eSurfaceMap:  return "SurfaceMap";
            case builders::ParamType::eVector2:     return "Vector2";
            case builders::ParamType::eVector3:     return "Vector3";
        }

        assert(false);
    }


    enum class ParamNecessity
    {
        eOptional,
        eRequired
    };

    struct Optional_t : std::integral_constant<ParamNecessity, ParamNecessity::eOptional> { };
    struct Required_t : std::integral_constant<ParamNecessity, ParamNecessity::eRequired> { };

    constexpr Optional_t OPTIONAL = Optional_t();
    constexpr Required_t REQUIRED = Required_t();

    //TODO: move this definition somewhere else
    //TODO: expand upon defintion
    class IdentifierString : public std::string
    {
    public:
        using std::string::basic_string;

        IdentifierString(const std::string& string) :
            basic_string(string)
        { }
    };

    namespace ParamTypes
    {

        using Boolean     = bool;
        using Camera      = std::shared_ptr<::Camera>;
        using Colour      = graphics::ColourRgb<float>;
        using Float       = double;
        using FloatList   = std::vector<Float>;
        using Identifier  = IdentifierString;
        using Integer     = int64_t;
        using ImageSize   = geometry::Point2t<Integer>;
        using IntegerList = std::vector<Integer>;
        using Object      = std::shared_ptr<BuilderArgs>;
        using Point2      = geometry::Point2;
        using Point3      = geometry::Point3;
        using Shape       = std::shared_ptr<shapes::Shape>;
        using ShapeMap    = std::map<Identifier, Shape>;
        using String      = std::string;
        using Surface     = std::shared_ptr<::Surface>;
        using SurfaceMap  = std::map<Identifier, Surface>;
        using Vector2     = geometry::Vector2;
        using Vector3     = geometry::Vector3;

        template <typename T> struct Index;
        template <> struct Index<Boolean>     : public std::integral_constant<ParamType, ParamType::eBoolean>     { };
        template <> struct Index<Camera>      : public std::integral_constant<ParamType, ParamType::eCamera>      { };
        template <> struct Index<Colour>      : public std::integral_constant<ParamType, ParamType::eColour>      { };
        template <> struct Index<Float>       : public std::integral_constant<ParamType, ParamType::eFloat>       { };
        template <> struct Index<FloatList>   : public std::integral_constant<ParamType, ParamType::eFloatList>   { };
        template <> struct Index<Identifier>  : public std::integral_constant<ParamType, ParamType::eIdentifier>  { };
        template <> struct Index<ImageSize>   : public std::integral_constant<ParamType, ParamType::eImageSize>   { };
        template <> struct Index<Integer>     : public std::integral_constant<ParamType, ParamType::eInteger>     { };
        template <> struct Index<IntegerList> : public std::integral_constant<ParamType, ParamType::eIntegerList> { };
        template <> struct Index<Object>      : public std::integral_constant<ParamType, ParamType::eObject>      { };
        template <> struct Index<Point2>      : public std::integral_constant<ParamType, ParamType::ePoint2>      { };
        template <> struct Index<Point3>      : public std::integral_constant<ParamType, ParamType::ePoint3>      { };
        template <> struct Index<Shape>       : public std::integral_constant<ParamType, ParamType::eShape>       { };
        template <> struct Index<ShapeMap>    : public std::integral_constant<ParamType, ParamType::eShapeMap>    { };
        template <> struct Index<String>      : public std::integral_constant<ParamType, ParamType::eString>      { };
        template <> struct Index<Surface>     : public std::integral_constant<ParamType, ParamType::eSurface>     { };
        template <> struct Index<SurfaceMap>  : public std::integral_constant<ParamType, ParamType::eSurfaceMap>  { };
        template <> struct Index<Vector2>     : public std::integral_constant<ParamType, ParamType::eVector2>     { };
        template <> struct Index<Vector3>     : public std::integral_constant<ParamType, ParamType::eVector3>     { };

    }

    using ParamValue = boost::variant<
        ParamTypes::Boolean,
        ParamTypes::Camera,
        ParamTypes::Colour,
        ParamTypes::Float,
        ParamTypes::FloatList,
        ParamTypes::Identifier,
        ParamTypes::ImageSize,
        ParamTypes::Integer,
        ParamTypes::IntegerList,
        ParamTypes::Object,
        ParamTypes::Point2,
        ParamTypes::Point3,
        ParamTypes::Shape,
        ParamTypes::ShapeMap,
        ParamTypes::String,
        ParamTypes::Surface,
        ParamTypes::SurfaceMap,
        ParamTypes::Vector2,
        ParamTypes::Vector3
    >;

    template <typename First>
    ParamType getParamTypeHelper(int typeNumber) {
        (void)typeNumber;
        assert(typeNumber == 0);
        return ParamTypes::Index<First>::value;
    }

    template <typename First, typename Second, typename ... Rest>
    ParamType getParamTypeHelper(int typeNumber) {
        assert(typeNumber >= 0);

        if (typeNumber == 0) {
            return ParamTypes::Index<First>::value;
        } else {
            return getParamTypeHelper<Second, Rest...>(typeNumber - 1);
        }
    }

    template <typename ... VariantTypes>
    ParamType getParamType(const boost::variant<VariantTypes...>& arg) {
        return getParamTypeHelper<VariantTypes...>(arg.which());
    }

}

#endif
