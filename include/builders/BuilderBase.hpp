#ifndef BUILDER_BASE_HPP
#define BUILDER_BASE_HPP

#include <memory>
#include <type_traits>

#include <builders/BuilderArgs.hpp>
#include <builders/Conversions.hpp>
#include <builders/ParamTypes.hpp>

namespace builders
{

    template <typename T>
    class BuilderBase
    {
    private:
        class ParamDefinition
        {
        public:
            ParamDefinition(ParamType type, ParamNecessity necessity, const ParamValue& defaultValue) :
                m_type(type),
                m_necessity(necessity),
                m_defaultValue(defaultValue)
            {

            }

            ParamType type() const
            {
                return m_type;
            }

            ParamNecessity necessity() const
            {
                return m_necessity;
            }

            ParamValue defaultValue() const
            {
                return m_defaultValue;
            }

        private:
            ParamType m_type;
            ParamNecessity m_necessity;
            ParamValue m_defaultValue;
        };

    public:
        virtual ~BuilderBase()
        {

        }

        std::shared_ptr<T> build(const BuilderArgs& args);

    protected:
        void parameter(const std::string& name, ParamType type, Required_t necessity);

        void parameter(const std::string& name, ParamType type, Optional_t necessity, const ParamValue& defaultValue);

        void setAllowExtraArguments(bool value)
        {
            m_allowExtraArgs = value;
        }

    private:
        using ParameterMap = std::map<std::string, ParamDefinition>;

        virtual std::shared_ptr<T> construct(const BuilderArgs& args) = 0;

        virtual ParamValue customConvert(const ParamValue& arg, ParamType targetType);

        void defineParameter(const std::string& name, ParamType type, ParamNecessity necessity, const ParamValue& defaultValue);

        ParamValue convertArg(const ParamValue& arg, ParamType targetType);

        ParameterMap m_parameters;
        bool m_allowExtraArgs;
    };

    template <typename T>
    inline std::shared_ptr<T> BuilderBase<T>::build(const BuilderArgs& args)
    {
        BuilderArgs sanitizedArgs;

        for (const auto& p : m_parameters)
        {
            const std::string& paramName = p.first;
            const ParamDefinition& paramDef = p.second;

            auto argIter = args.find(paramName);

            if (argIter != args.end())
            {
                const auto& arg = argIter->second;
                sanitizedArgs.insert(paramName, convertArg(arg, paramDef.type()));
            }
            else if (paramDef.necessity() == ParamNecessity::eOptional)
            {
                sanitizedArgs.insert(paramName, paramDef.defaultValue());
            }
            else
            {
                throw MissingParameterException(paramName);
            }
        }

        // Check for arguments with names that aren't defined under the parameter list
        for (const auto& arg : args)
        {
            if (m_parameters.find(arg.first) == m_parameters.end())
            {
                if (!m_allowExtraArgs)
                {
                    throw InvalidParameterException(arg.first);
                }

                sanitizedArgs.insert(arg);
            }
        }

        return this->construct(sanitizedArgs);
    }

    template <typename T>
    inline void BuilderBase<T>::parameter(const std::string& name, ParamType type, Required_t necessity)
    {
        defineParameter(name, type, necessity, ParamValue());
    }

    template <typename T>
    inline void BuilderBase<T>::parameter(const std::string& name, ParamType type, Optional_t necessity, const ParamValue& defaultValue)
    {
        defineParameter(name, type, necessity, defaultValue);
    }

    template <typename T>
    inline void BuilderBase<T>::defineParameter(const std::string& name, ParamType type, ParamNecessity necessity, const ParamValue& defaultValue)
    {
        typename ParameterMap::iterator iter;
        bool success;
        std::tie(iter, success) = m_parameters.insert(typename ParameterMap::value_type(name, ParamDefinition(type, necessity, defaultValue)));

        if (!success)
        {
            // Parameter already defined
            throw ParameterRedefinitionException(name);
        }
    }

    template <typename T>
    inline ParamValue BuilderBase<T>::customConvert(const ParamValue& arg, ParamType targetType)
    {
        throw InvalidConversionException(arg, targetType);
    }

    template <typename T>
    inline ParamValue BuilderBase<T>::convertArg(const ParamValue& arg, ParamType targetType)
    {
        ParamType argType = getParamType(arg);

        if (argType == targetType) {
            return arg;
        }

        switch (targetType) {
            case ParamType::eFloat:
                return paramCast<ParamTypes::Float, ParamTypes::Integer>(arg);
            case ParamType::eIdentifier:
                return paramCast<ParamTypes::Identifier, ParamTypes::String>(arg);
            case ParamType::eImageSize:
                return paramCast<ParamTypes::ImageSize, ParamTypes::IntegerList>(arg);
            case ParamType::eColour:
                if (argType == ParamType::eFloatList)
                {
                    return paramCast<ParamTypes::Colour, ParamTypes::FloatList>(arg);
                }
                else if (argType == ParamType::eIntegerList)
                {
                    return paramCast<ParamTypes::Colour, ParamTypes::IntegerList>(arg);
                }

                break;
            case ParamType::ePoint2:
                if (argType == ParamType::eFloatList)
                {
                    return paramCast<ParamTypes::Point2, ParamTypes::FloatList>(arg);
                }
                else if (argType == ParamType::eIntegerList)
                {
                    return paramCast<ParamTypes::Point2, ParamTypes::IntegerList>(arg);
                }

                break;
            case ParamType::ePoint3:
                if (argType == ParamType::eFloatList)
                {
                    return paramCast<ParamTypes::Point3, ParamTypes::FloatList>(arg);
                }
                else if (argType == ParamType::eIntegerList)
                {
                    return paramCast<ParamTypes::Point3, ParamTypes::IntegerList>(arg);
                }

                break;
            case ParamType::eVector2:
                if (argType == ParamType::eFloatList)
                {
                    return paramCast<ParamTypes::Vector2, ParamTypes::FloatList>(arg);
                }
                else if (argType == ParamType::eIntegerList)
                {
                    return paramCast<ParamTypes::Vector2, ParamTypes::IntegerList>(arg);
                }

                break;
            case ParamType::eVector3:
                if (argType == ParamType::eFloatList)
                {
                    return paramCast<ParamTypes::Vector3, ParamTypes::FloatList>(arg);
                }
                else if (argType == ParamType::eIntegerList)
                {
                    return paramCast<ParamTypes::Vector3, ParamTypes::IntegerList>(arg);
                }

                break;

            case ParamType::eCamera:
            case ParamType::eShape:
            case ParamType::eShapeMap:
            case ParamType::eSurface:
            case ParamType::eSurfaceMap:
                return customConvert(arg, targetType);

            default: break;
        }

        throw InvalidConversionException(arg, targetType);
    }

}

#endif
