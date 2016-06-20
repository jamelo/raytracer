#ifndef Transformation_HPP
#define Transformation_HPP

#include <cmath>

#include <geometry/Point.hpp>
#include <geometry/Vector.hpp>

namespace geometry
{

    template <typename T, std::size_t Dimensions>
    class Transformation
    {
    private:
        std::array<Vector<T, Dimensions>, Dimensions> m_data;

        Transformation() { }

    public:
        /*Transformation(const std::array<Vector<T, Dimensions>, Dimensions>& _data) :
            m_data(_data)
        { }*/

        Transformation(const std::initializer_list<std::initializer_list<T>>& _data) {
            std::size_t rowNum = 0;

            for (const auto& row : _data) {
                std::size_t colNum = 0;

                for (auto cell : row) {
                    m_data[rowNum][colNum] = cell;

                    colNum++;

                    if (colNum >= m_data[rowNum].size()) {
                        break;
                    }
                }

                for (; colNum < m_data[rowNum].size(); colNum++) {
                    m_data[rowNum][colNum] = 0;
                }

                rowNum++;

                if (rowNum >= m_data.size()) {
                    break;
                }
            }

            for (; rowNum < m_data.size(); rowNum++) {
                for (std::size_t colNum = 0; colNum < m_data[rowNum].size(); colNum++) {
                    m_data[rowNum][colNum] = 0;
                }
            }
        }

        Transformation<T, Dimensions> operator*(const Transformation<T, Dimensions>& rhs) const {
            Transformation<T, Dimensions> result;
            Transformation<T, Dimensions> rhsTransposed = transpose(rhs);

            for (std::size_t row = 0; row < Dimensions; row++) {
                for (std::size_t col = 0; col < Dimensions; col++) {
                    result.m_data[row][col] = m_data[row] * rhsTransposed.m_data[col];
                }
            }

            return result;
        }

        Transformation<T, Dimensions> operator*=(T rhs) const {
            Transformation<T, Dimensions> result;

            for (std::size_t row = 0; row < Dimensions; row++) {
                result.m_data[row] *= rhs;
            }

            return result;
        }

        Transformation<T, Dimensions> operator*(T rhs) const {
            return Transformation<T, Dimensions>(*this) *= rhs;
        }

        Vector<T, Dimensions> operator*(const Vector<T, Dimensions>& rhs) const {
            Vector<T, Dimensions> result;

            for (std::size_t row = 0; row < Dimensions; row++) {
                result.m_data[row] = m_data[row] * rhs;
            }

            return result;
        }

        Point<T, Dimensions> operator*(const Point<T, Dimensions>& rhs) const {
            Vector<T, Dimensions> result;

            for (std::size_t row = 0; row < Dimensions; row++) {
                result[row] = static_cast<Vector<T, Dimensions>>(m_data[row]) * static_cast<Vector<T, Dimensions>>(rhs);
            }

            return static_cast<Point<T, Dimensions>>(result);
        }

        friend Transformation<T, Dimensions> transpose(const Transformation<T, Dimensions>& src)
        {
            Transformation<T, Dimensions> result;

            for (std::size_t row = 0; row < Dimensions; row++) {
                for (std::size_t col = 0; col < Dimensions; col++) {
                    result.m_data[row][col] = src.m_data[col][row];
                }
            }

            return result;
        }

    };

    //template <typename T, std::size_t Dimensions>
    template <typename T>
    Transformation<T, 3ul> rotation(T alpha, T beta, T gamma)
    {
        Transformation<T, 3ul> X({
            {1, 0, 0},
            {0, std::cos(alpha), -std::sin(alpha)},
            {0, std::sin(alpha), std::cos(alpha)}
        });

        Transformation<T, 3ul> Y{
            {std::cos(beta), 0, -std::sin(beta)},
            {0, 1, 0},
            {std::sin(beta), 0, std::cos(beta)}
        };

        Transformation<T, 3ul> Z{
            {std::cos(gamma), -std::sin(gamma), 0},
            {std::sin(gamma), std::cos(gamma), 0},
            {0, 0, 1}
        };

        return Z * Y * X;
    }

}

#endif // Transformation_HPP
