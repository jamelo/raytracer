#ifndef Image_HPP
#define Image_HPP

#include <cassert>
#include <cstddef>
#include <cmath>
#include <iterator>
#include <memory>
#include <random>

#include <SFML/Graphics/Image.hpp>

#include <Colour.hpp>

inline size_t lcm(size_t a, size_t b)
{
    size_t a1 = a;
    size_t b1 = b;
    size_t tmp;

    while (b1 != 0) {
        tmp = b1;
        b1 = a1 % b1;
        a1 = tmp;
    }

    return (a * b) / a1;
}

template <typename PixelType, bool is_const>
class ImageIterator;

template <typename PixelType, bool is_const>
class ImageRow
{
public:
    typedef typename std::conditional<is_const, const PixelType*, PixelType*>::type iterator;
    typedef const PixelType* const_iterator;

private:
    iterator m_rowStart;
    iterator m_rowEnd;

    ImageRow(iterator rowStart, iterator rowEnd) :
        m_rowStart(rowStart),
        m_rowEnd(rowEnd)
    { }

public:
    ImageRow() = default;

    iterator begin() { return m_rowStart; }
    iterator end() { return m_rowEnd; }
    const_iterator begin() const { return m_rowStart; }
    const_iterator end() const { return m_rowEnd; }

    friend class ImageIterator<PixelType, is_const>;
};

template <typename PixelType>
class Image;

template <typename PixelType, bool is_const = false>
class ImageIterator : public std::iterator<std::random_access_iterator_tag, ImageRow<PixelType, is_const>>
{
public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef typename std::conditional<is_const, const ImageRow<PixelType, true>, ImageRow<PixelType, false>>::type value_type;
    typedef std::ptrdiff_t difference_type;
    typedef value_type& reference;
    typedef value_type* pointer;

    typedef ImageIterator<PixelType, is_const> iterator_type;
    typedef typename std::conditional<is_const, const Image<PixelType>, Image<PixelType>>::type image_type;
    typedef typename std::conditional<is_const, const PixelType, PixelType>::type pixel_type;

private:
    image_type* m_image;
    pixel_type* m_rowStart;

    ImageIterator(image_type& image, size_t row) :
            m_image(&image)
    {
        assert(row <= image.height());
        m_rowStart = m_image->data() + row * m_image->stride();
    }

public:
    ImageIterator();

    iterator_type& operator++() { m_rowStart += m_image->stride(); return *this; }
    iterator_type operator++(int) { auto tmp = *this; operator++(); return tmp; }
    iterator_type& operator--() { m_rowStart -= m_image->stride(); return *this; }
    iterator_type operator--(int) { auto tmp = *this; operator--(); return tmp; }

    iterator_type& operator+=(long int offset) { m_rowStart += m_image->stride() * offset; return *this; }
    iterator_type& operator-=(long int offset) { m_rowStart -= m_image->stride() * offset; return *this; }
    iterator_type operator+(long int offset) const { auto tmp = *this; return (tmp += offset); }
    iterator_type operator-(long int offset) const { auto tmp = *this; return (tmp -= offset); }

    bool operator<(const iterator_type& rhs) const { return m_rowStart < rhs.m_rowStart; }
    bool operator>(const iterator_type& rhs) const { return m_rowStart > rhs.m_rowStart; }
    bool operator<=(const iterator_type& rhs) const { return m_rowStart <= rhs.m_rowStart; }
    bool operator>=(const iterator_type& rhs) const { return m_rowStart >= rhs.m_rowStart; }
    bool operator==(const iterator_type& rhs) const { return m_rowStart == rhs.m_rowStart; }
    bool operator!=(const iterator_type& rhs) const { return m_rowStart != rhs.m_rowStart; }

    value_type operator*() const {
        return value_type(m_rowStart, m_rowStart + m_image->width());
    }

    value_type operator[](size_t index) const {
        pixel_type* rowStart = m_rowStart + m_image->stride() * index;
        return value_type(rowStart, rowStart + m_image->width());
    }

    friend iterator_type operator+(long int offset, const iterator_type& it) {
        auto tmp = it; return (tmp += offset);
    }

    friend iterator_type operator-(long int offset, const iterator_type& it) {
        auto tmp = it; return (tmp -= offset);
    }

    friend class Image<PixelType>;
};

//TODO: implement colour profiles for float to rgb8 conversion


template <typename PixelType>
class Image
{
private:
    std::unique_ptr<PixelType[]> m_imageData;
    size_t m_width;
    size_t m_height;
    size_t m_stride;

    static const size_t CACHE_LINE_SIZE = 64;

public:
    typedef ImageIterator<PixelType, false> iterator;
    typedef ImageIterator<PixelType, true> const_iterator;

    Image(size_t _width, size_t _height) :
        m_width(0),
        m_height(0),
        m_stride(0)
    {
        create(_width, _height);
    }

    Image(const std::string& fileName) :
        m_width(0),
        m_height(0),
        m_stride(0)
    {
        load(fileName);
    }

    Image(Image<PixelType>&&) = default;
    Image& operator=(Image<PixelType>&&) = default;

    size_t width()  const { return m_width; }
    size_t height() const { return m_height; }
    size_t stride() const { return m_stride; }

    iterator        begin()        { return iterator(*this, 0); }
    iterator        end()          { return iterator(*this, m_height ); }
    const_iterator  begin()  const { return const_iterator(*this, 0); }
    const_iterator  end()    const { return const_iterator(*this, m_height); }
    const_iterator  cbegin() const { return begin(); }
    const_iterator  cend()   const { return end(); }

    PixelType* data() { return m_imageData.get(); }
    const PixelType* data() const { return m_imageData.get(); }

    void save(const std::string& fileName) const
    {
        sf::Image outputImage;
        outputImage.create(m_width, m_height);

        size_t rowNumber = 0;

        for (auto row : *this) {
            size_t colNumber = 0;

            for (auto& pixel : row) {
                outputImage.setPixel(colNumber, rowNumber, colour_cast<sf::Color>(pixel));
                colNumber++;
            }

            rowNumber++;
        }

        outputImage.saveToFile(fileName);
    }

private:
    void create(size_t _width, size_t _height)
    {
        assert(!m_imageData);

        size_t blockSize = lcm(CACHE_LINE_SIZE, sizeof(PixelType)) / sizeof(PixelType);
        m_stride = ((_width + blockSize - 1) / blockSize) * blockSize;
        m_imageData = std::make_unique<PixelType[]>(m_stride * _height);
        m_width = _width;
        m_height = _height;
    }

    void load(const std::string& fileName)
    {
        sf::Image inputImage;
        inputImage.loadFromFile(fileName);

        create(inputImage.getSize().x, inputImage.getSize().y);

        size_t rowNumber = 0;

        for (auto& row : *this) {
            uint8_t* inputPix = inputImage.getPixelsPtr() + 4 * m_width * rowNumber;

            for (auto& pixel : row) {
                uint8_t& red    = *inputPix;
                uint8_t& green  = *(inputPix + 1);
                uint8_t& blue   = *(inputPix + 2);
                uint8_t& alpha  = *(inputPix + 3);

                pixel = colour_cast<PixelType>(sf::Color(red, green, blue, alpha));

                inputPix += 4;
            }
        }
    }
};

#endif // Image_HPP
