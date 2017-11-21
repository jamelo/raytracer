#ifndef SCENE_LOADER_JSON_HPP
#define SCENE_LOADER_JSON_HPP

#include <errno.h>
#include <fstream>
#include <istream>
#include <limits>
#include <map>
#include <system_error>

#include <json.hpp>

#include <builders/ParamTypes.hpp>
#include <builders/SurfaceBuilder.hpp>
#include <Exceptions.hpp>
#include <SceneLoader.hpp>

class SceneLoaderJson : public SceneLoader
{
    using json = nlohmann::json;

public:
    virtual builders::BuilderArgs load(const std::string& filename) override
    {
        using namespace builders;

        std::ifstream jsonFile(filename);

        if (!jsonFile.is_open()) {
            throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory));
        }

        const json j(jsonFile);
        auto args = *jsonToBuilderArgs(j);

        return args;
    }

private:

    std::shared_ptr<builders::BuilderArgs> jsonToBuilderArgs(const json& j)
    {
        std::map<std::string, json> jsonArgs = j;
        builders::BuilderArgs args;

        for (const auto& arg : jsonArgs) {
            args.insert(arg.first, jsonToParamValue(arg.second));
        }

        return std::make_shared<builders::BuilderArgs>(args);
    }

    builders::ParamValue jsonToParamValue(const json& j)
    {
        switch (j.type()) {
            case json::value_t::null:
                throw UnexpectedTypeException("blah");
            case json::value_t::object:
                return jsonToBuilderArgs(j);
            case json::value_t::array:
                return arrayToParamValue(j);
            case json::value_t::string:
                return j.get<json::string_t>();
            case json::value_t::boolean:
                return j.get<json::boolean_t>();
            case json::value_t::number_integer:
                return j.get<json::number_integer_t>();
            case json::value_t::number_unsigned:
                return unsignedToParamValue(j);
            case json::value_t::number_float:
                return j.get<json::number_float_t>();
            case json::value_t::discarded:
                throw UnexpectedTypeException("blah");
            default:
                assert(false);
                throw SomeKindOfException();
        }
    }

    builders::ParamValue unsignedToParamValue(json::number_unsigned_t x)
    {
        if (x > static_cast<json::number_unsigned_t>(std::numeric_limits<builders::ParamTypes::Integer>::max())) {
            throw UnexpectedTypeException("blah");
        }

        return static_cast<builders::ParamTypes::Integer>(x);
    }

    builders::ParamType commonType(const json& j)
    {
        for (const json& subExp : j) {
            switch(subExp.type()) {
                case json::value_t::number_integer:
                case json::value_t::number_unsigned:
                    break;
                case json::value_t::number_float:
                    return builders::ParamType::eFloat;
                default:
                    throw SomeKindOfException();
            }
        }

        return builders::ParamType::eInteger;
    }

    builders::ParamValue arrayToParamValue(const json& j)
    {
        using namespace builders::ParamTypes;

        if (j.size() == 0) {
            return IntegerList();
        }

        switch(commonType(j)) {
            case builders::ParamType::eInteger:
                return j.get<IntegerList>();
            case builders::ParamType::eFloat:
                return j.get<FloatList>();
            default:
                throw SomeKindOfException();
        }
    }
};

#endif
