#ifndef POINT_BASE_HPP
#define POINT_BASE_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <iostream>

#include <Assert.hpp>

namespace geometry
{

    namespace detail
    {

        template <typename T, std::size_t Dimensions>
        class point_base : private std::array<T, Dimensions>
        {
            static_assert(Dimensions > 0, "Dimensions must be greater than zero.");

        public:
            using container_type = typename std::array<T, Dimensions>;

            using value_type             = typename container_type::value_type;
            using iterator               = typename container_type::const_iterator;
            using const_iterator         = typename container_type::const_iterator;
            using reverse_iterator       = typename container_type::const_reverse_iterator;
            using const_reverse_iterator = typename container_type::const_reverse_iterator;
            using size_type              = typename container_type::size_type;
            using difference_type        = typename container_type::difference_type;
            using pointer                = typename container_type::const_pointer;
            using const_pointer          = typename container_type::const_pointer;
            using reference              = typename container_type::const_reference;
            using const_reference        = typename container_type::const_reference;

            using container_type::operator[];
            using container_type::data;
            using container_type::begin;
            using container_type::end;
            using container_type::cbegin;
            using container_type::cend;
            using container_type::rbegin;
            using container_type::rend;
            using container_type::crbegin;
            using container_type::crend;

            static constexpr size_type dimensions = Dimensions;

            point_base() :
                std::array<T, Dimensions>()
            {
                Assert(!isNaN());
            }

            point_base(const std::array<T, Dimensions>& components) :
                std::array<T, Dimensions>(components)
            {
                Assert(!isNaN());
            }

            constexpr size_type size() const
            {
                return Dimensions;
            }

            friend std::ostream& operator<<(std::ostream& os, const point_base<T, Dimensions>& p)
            {
                os << "(";

                std::copy(p.begin(), p.end() - 1, std::ostream_iterator<T>(os, ", "));
                std::copy(p.end() - 1, p.end(), std::ostream_iterator<T>(os, ""));

                os << ")";

                return os;
            }

            bool isNaN() const
            {
                return std::any_of(begin(), end(), [](T x){ return std::isnan(x); });
            }
        };

    }

}

#endif
