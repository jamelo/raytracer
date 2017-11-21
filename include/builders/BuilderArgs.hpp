#ifndef BUILDER_ARGS_HPP
#define BUILDER_ARGS_HPP

#include <map>
#include <string>

#include <Exceptions.hpp>
#include <builders/ParamTypes.hpp>

namespace builders
{

    class BuilderArgs : private std::map<std::string, ParamValue>
    {
    public:
        using map_t = std::map<std::string, ParamValue>;
        using const_iterator = map_t::const_iterator;
        using iterator = map_t::iterator;
        using value_type = map_t::value_type;
        using size_type = map_t::size_type;

        using map_t::begin;
        using map_t::end;
        using map_t::cbegin;
        using map_t::cend;
        using map_t::insert;
        using map_t::size;
        using map_t::find;

        template <typename T>
        const T& get(const std::string& argName) const
        {
            auto iter = find(argName);

            if (iter == end())
            {
                throw UndefinedParameterException(argName);
            }

            const ParamValue& value = iter->second;
            return boost::get<T>(value);
        }

        template <typename T>
        void insert(const std::string& parameterName, const T& value)
        {
            insert(std::make_pair(parameterName, ParamValue(value)));
        }

    private:
        std::map<std::string, ParamValue> m_arguments;
    };


}

#endif
