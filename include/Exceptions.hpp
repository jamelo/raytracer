#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <cassert>
#include <string>
#include <sstream>

#include <builders/ParamTypes.hpp>

class MissingParameterException
{
public:
    MissingParameterException(const std::string& paramName) {
        (void)paramName;
        assert(false);
    }
};

class UnexpectedTypeException
{
public:
    UnexpectedTypeException(const std::string& paramName) {
        (void)paramName;
        assert(false);
    }
};

class SomeKindOfException
{

};

class SomeKindOfNotImplementedException
{

};

class SomeKindOfNameConflictException
{

};

class BuilderException : std::exception
{
public:

    virtual const char* what() const noexcept(true) override
    {
        return m_message.c_str();
    }

    class MessageProxy
    {
    public:
        MessageProxy(BuilderException& ex) :
            m_ex(ex),
            m_msg(ex.m_message)
        {
            m_msg.seekp(0, std::ios_base::end);
        }

        MessageProxy(const MessageProxy&) = delete;
        MessageProxy(MessageProxy&&) = default;

        ~MessageProxy()
        {
            m_ex.m_message = m_msg.str();
        }

        template <typename T>
        MessageProxy& operator<<(const T& x)
        {
            m_msg << x;
            return *this;
        }

    private:
        BuilderException& m_ex;
        std::stringstream m_msg;
    };

protected:
    MessageProxy message()
    {
        return *this;
    }

    std::string m_message;
};

class InvalidArrayConversionException : public BuilderException
{
public:
    InvalidArrayConversionException(const builders::ParamValue& arg, size_t argSize, builders::ParamType targetType, size_t targetSize)
    {
        message() << "Invalid conversion from array of type " << builders::stringify(builders::getParamType(arg)) << " of size " << argSize;
        message() << " to " << builders::stringify(targetType) << " (required size is " << targetSize << ").";
    }
};

class UndefinedParameterException : public BuilderException
{
public:
    UndefinedParameterException(const std::string& paramName)
    {
        message() << "Invalid get of undefined parameter '" << paramName << "'.";
    }
};

class InvalidParameterException : public BuilderException
{
public:
    InvalidParameterException(const std::string& paramName)
    {
        message() << "Invalid parameter '" << paramName << "'.";
    }
};

class ParameterRedefinitionException : public BuilderException
{
public:
    ParameterRedefinitionException(const std::string& paramName)
    {
        message() << "Redefinition of parameter '" << paramName << "'.";
    }
};

class InvalidConversionException : public BuilderException
{
public:
    InvalidConversionException(const builders::ParamValue& arg, builders::ParamType targetType)
    {
        message() << "Invalid conversion from " << builders::stringify(builders::getParamType(arg));
        message() << " to " << builders::stringify(targetType) << ".";
    }
};

class UndeclaredIdentifierException : public BuilderException
{
public:
    UndeclaredIdentifierException(const std::string& identifier)
    {
        message() << "Undeclared identifier '" << identifier << "'.";
    }
};

class ShapeRedefinitionException : public BuilderException
{
public:
    ShapeRedefinitionException(const std::string& name)
    {
        message() << "Redefinition of shape type '" << name << "'.";
    }
};

class UnknownShapeTypeException : public BuilderException
{
public:
    UnknownShapeTypeException(const std::string& shape)
    {
        message() << "Unknown shape type '" << shape << "'.";
    }
};

#endif
