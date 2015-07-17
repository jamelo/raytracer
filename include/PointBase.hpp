#ifndef PointBase_HPP
#define PointBase_HPP

#include <array>
#include <iterator>
#include <iostream>

namespace geometry
{

    namespace detail
    {

        template <typename T, std::size_t Dimensions>
        class point_base : private std::array<T, Dimensions>
        {
            static_assert(Dimensions > 0, "Dimensions must be greater than zero.");

        public:
            typedef typename std::array<T, Dimensions> container_type;
            typedef typename container_type::value_type value_type;
            typedef typename container_type::const_iterator iterator;
            typedef typename container_type::const_iterator const_iterator;
            typedef typename container_type::const_reverse_iterator reverse_iterator;
            typedef typename container_type::const_reverse_iterator const_reverse_iterator;
            typedef typename container_type::size_type size_type;
            typedef typename container_type::difference_type difference_type;
            typedef typename container_type::const_pointer pointer;
            typedef typename container_type::const_pointer const_pointer;
            typedef typename container_type::const_reference reference;
            typedef typename container_type::const_reference const_reference;

            static constexpr size_type dimensions = Dimensions;

            point_base() :
                std::array<T, Dimensions>() {
            }

            point_base(const std::array<T, Dimensions>& _components) :
                std::array<T, Dimensions>(_components) {
            }

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

            constexpr size_type size() const {
                return Dimensions;
            }

            friend std::ostream& operator<<(std::ostream& os, const point_base<T, Dimensions>& p) {
                os << "(";

                std::copy(p.begin(), p.end() - 1, std::ostream_iterator<T>(os, ", "));
                std::copy(p.end() - 1, p.end(), std::ostream_iterator<T>(os, ""));
                
                os << ")";

                return os;
            }
        };

    }

}

#endif // PointBase_HPP
