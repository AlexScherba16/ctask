#ifndef MISC_H
#define MISC_H

#include "utils/types/types.h"
#include <functional>

namespace ctask::utils::misc
{
    using namespace ctask::utils::types;

    /**
     * @class Defer
     * @brief Utility class for executing a function upon destruction.
     *
     * The Defer class ensures that a given function is executed when
     * an instance of the class goes out of scope. Thanks to Golang for
     * naming.
     */

    class Defer
    {
    public:
        Defer() = delete;
        Defer(const Defer&) = delete;
        Defer(Defer&&) = delete;
        Defer& operator=(const Defer&) = delete;
        Defer& operator=(Defer&&) = delete;

        Defer(std::function<void()> func) :
            func_(std::move(func))
        {
        }

        ~Defer()
        {
            if (func_)
            {
                try
                {
                    func_();
                }
                catch (const std::exception& e)
                {
                    // log something
                }
                catch (...)
                {
                    // log something really terrible :)
                }
            }
        }

    private:
        std::function<void()> func_{};
    };

    /**
     * @brief Converts a string representation of an HTTP method to an HttpMethod enum.
     *
     * This function maps standard HTTP method strings to their respective enum values.
     * If the provided string is not recognized, it defaults to UNKNOWN_METHOD.
     *
     * @param method A string view representing the HTTP method.
     * @return HttpMethod The corresponding HttpMethod enum value.
     */
    static HttpMethod httpMethodFromString(std::string_view method)
    {
        static const std::unordered_map<std::string_view, HttpMethod> methodMap = {
            {"GET", HttpMethod::GET_METHOD},
            {"POST", HttpMethod::POST_METHOD}
        };
        const auto it{methodMap.find(method)};
        return (it != methodMap.end()) ? it->second : HttpMethod::UNKNOWN_METHOD;
    }
}

#endif //MISC_H
