#ifndef Vector3_HPP
#define Vector3_HPP

#include <smmintrin.h>

namespace geometry
{

    template <typename T, size_t Dimensions>
    class Vector;

    template <>
    class Vector<float, 3ul>
    {
    private:
        float m_components[4] __attribute__((aligned(16)));

        Vector(__m128 _vec) {
            //__m128 mask = _mm_castsi128_ps(_mm_set_epi32(-1, -1, -1, 0));
            //__m128 temp1 = _mm_and_ps(_vec, mask);
            _mm_store_ps(m_components, _vec);
        }

    public:
        Vector() :
            m_components({0.0f, 0.0f, 0.0f, 0.0f}) {
        }

        Vector(const std::array<float, 3ul>& _components) :
            m_components({_components[0], _components[1], _components[2], 0.0f}) {
        }

        float operator[](size_t i) const {
            return m_components[i];
        }

        const float* begin() const {
            return m_components;
        }

        const float* end() const {
            return m_components + 3;
        }

        Vector<float, 3ul> operator+() const {
            return *this;
        }

        Vector<float, 3ul> operator-() const {
            __m128 vec1 = _mm_load_ps(m_components);
            return Vector<float, 3ul>(_mm_sub_ps(_mm_set1_ps(0.0), vec1));
        }

        Vector<float, 3ul>& operator+=(const Vector<float, 3ul>& rhs) {
            __m128 vec1 = _mm_load_ps(m_components);
            __m128 vec2 = _mm_load_ps(rhs.m_components);
            __m128 result = _mm_add_ps(vec1, vec2);
            _mm_store_ps(m_components, result);

            return *this;
        }

        Vector<float, 3ul>& operator-=(const Vector<float, 3ul>& rhs) {
            __m128 vec1 = _mm_load_ps(m_components);
            __m128 vec2 = _mm_load_ps(rhs.m_components);
            __m128 result = _mm_sub_ps(vec1, vec2);
            _mm_store_ps(m_components, result);

            return *this;
        }

        Vector<float, 3ul>& operator*=(float rhs) {
            __m128 vec1 = _mm_load_ps(m_components);
            __m128 result = _mm_mul_ps(vec1, _mm_set1_ps(rhs));
            _mm_store_ps(m_components, result);

            return *this;
        }

        Vector<float, 3ul>& operator/=(float rhs) {
            __m128 vec1 = _mm_load_ps(m_components);
            __m128 result = _mm_div_ps(vec1, _mm_set1_ps(rhs));
            _mm_store_ps(m_components, result);

            return *this;
        }

        Vector<float, 3ul> operator+(const Vector<float, 3ul>& rhs) const {
            __m128 vec1 = _mm_load_ps(m_components);
            __m128 vec2 = _mm_load_ps(rhs.m_components);
            return Vector<float, 3ul>(_mm_add_ps(vec1, vec2));
        }

        Vector<float, 3ul> operator-(const Vector<float, 3ul>& rhs) const {
            __m128 vec1 = _mm_load_ps(m_components);
            __m128 vec2 = _mm_load_ps(rhs.m_components);
            return Vector<float, 3ul>(_mm_sub_ps(vec1, vec2));
        }

        Vector<float, 3ul> operator*(float rhs) const {
            __m128 vec1 = _mm_load_ps(m_components);
            return Vector<float, 3ul>(_mm_mul_ps(vec1, _mm_set1_ps(rhs)));
        }

        Vector<float, 3ul> operator/(float rhs) const {
            __m128 vec1 = _mm_load_ps(m_components);
            return Vector<float, 3ul>(_mm_div_ps(vec1, _mm_set1_ps(rhs)));
        }

        float operator*(const Vector<float, 3ul>& rhs) const {
            float result;

            __m128 vec1 = _mm_load_ps(m_components);
            __m128 vec2 = _mm_load_ps(rhs.m_components);
            __m128 product = _mm_mul_ps(vec1, vec2);

            //__m128 mask = _mm_castsi128_ps(_mm_set_epi32(-1, -1, -1, 0));
            //__m128 temp1 = _mm_and_ps(product, mask);
            __m128 temp2 = _mm_hadd_ps(product, product);
            __m128 sum = _mm_hadd_ps(temp2, temp2);
            _mm_store_ss(&result, sum);
            return result;
            //return m_components[0] * rhs[0] + m_components[1] * rhs[1] + m_components[2] * rhs[2];
        }

        bool operator==(const Vector<float, 3ul>& rhs) {
            //__m128 vec1 = _mm_load_ps(m_components);
            //__m128 vec2 = _mm_load_ps(rhs.m_components);
            //return _mm_test_all_ones(_mm_castps_si128(_mm_cmpeq_ps(vec1, vec2)));
            return (m_components[0] == rhs[0]) && (m_components[1] == rhs[1]) && (m_components[2] == rhs[2]);
        }

        bool operator!=(const Vector<float, 3ul>& rhs) {
            __m128 vec1 = _mm_load_ps(m_components);
            __m128 vec2 = _mm_load_ps(rhs.m_components);
            return !operator==(rhs);
        }

        friend Vector<float, 3ul> operator*(float lhs, const Vector<float, 3ul>& rhs) {
            return rhs * lhs;
        }
    };

}

#endif // Vector3_HPP
