#ifndef Image_HPP
#define Image_HPP

#include <cassert>
#include <cstddef>
#include <cmath>
#include <iterator>
#include <random>

#include <SFML/Graphics/Image.hpp>

#include <Colour.hpp>

size_t lcm(size_t a, size_t b)
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

template <typename PixelType>
class ImageIterator;

template <typename PixelType>
class ImageRow
{
private:
    PixelType* m_rowStart;
    PixelType* m_rowEnd;

    ImageRow(PixelType* rowStart, PixelType* rowEnd) :
    m_rowStart(rowStart),
    m_rowEnd(rowEnd)
    { }

public:
    typedef PixelType* iterator;
    typedef const PixelType* const_iterator;

    ImageRow() = default;

    iterator begin() { return m_rowStart; }
    iterator end() { return m_rowEnd; }
    const_iterator begin() const { return m_rowStart; }
    const_iterator end() const { return m_rowEnd; }

    friend class ImageIterator<PixelType>;
};

template <typename PixelType>
class Image;

template <typename PixelType>
class ImageIterator : public std::iterator<std::random_access_iterator_tag, ImageRow<PixelType>>
{
private:
    Image<PixelType>* m_image;
    PixelType* m_rowStart;

    ImageIterator(Image<PixelType>* image, size_t row) :
            m_image(image)
    {
        assert(image != nullptr);
        assert(row <= image->height());

        m_rowStart = m_image->data() + row * m_image->stride();
    }

public:
    ImageIterator();

    ImageIterator<PixelType>& operator++() { m_rowStart += m_image->stride(); return *this; }
    ImageIterator<PixelType> operator++(int) { auto tmp = *this; operator++(); return tmp; }
    ImageIterator<PixelType>& operator--() { m_rowStart -= m_image->stride(); return *this; }
    ImageIterator<PixelType> operator--(int) { auto tmp = *this; operator--(); return tmp; }

    ImageIterator<PixelType>& operator+=(long int offset) { m_rowStart += m_image->stride() * offset; return *this; }
    ImageIterator<PixelType>& operator-=(long int offset) { m_rowStart -= m_image->stride() * offset; return *this; }
    ImageIterator<PixelType> operator+(long int offset) const { auto tmp = *this; return (tmp += offset); }
    ImageIterator<PixelType> operator-(long int offset) const { auto tmp = *this; return (tmp -= offset); }

    bool operator<(const ImageIterator<PixelType>& rhs) const { return m_rowStart < rhs.m_rowStart; }
    bool operator>(const ImageIterator<PixelType>& rhs) const { return m_rowStart > rhs.m_rowStart; }
    bool operator<=(const ImageIterator<PixelType>& rhs) const { return m_rowStart <= rhs.m_rowStart; }
    bool operator>=(const ImageIterator<PixelType>& rhs) const { return m_rowStart >= rhs.m_rowStart; }
    bool operator==(const ImageIterator<PixelType>& rhs) const { return m_rowStart == rhs.m_rowStart; }
    bool operator!=(const ImageIterator<PixelType>& rhs) const { return m_rowStart != rhs.m_rowStart; }

    ImageRow<PixelType> operator*() const
    {
        return ImageRow<PixelType>(m_rowStart, m_rowStart + m_image->width());
    }

    ImageRow<PixelType> operator[](size_t index) const
    {
        PixelType* rowStart = m_rowStart + m_image->stride() * index;
        return ImageRow<PixelType>(rowStart, rowStart + m_image->width());
    }

    friend ImageIterator<PixelType> operator+(long int offset, const ImageIterator<PixelType>& it)
    {
        auto tmp = it; return (tmp += offset);
    }

    friend ImageIterator<PixelType> operator-(long int offset, const ImageIterator<PixelType>& it)
    {
        auto tmp = it; return (tmp -= offset);
    }

    friend class Image<PixelType>;
};

//TODO: implement colour profiles for float to rgb8 conversion


template <typename PixelType>
class Image
{
private:
    PixelType* m_imageData;
    size_t m_width;
    size_t m_height;
    size_t m_stride;

    static const size_t CACHE_LINE_SIZE = 64;

public:
    typedef ImageIterator<PixelType> iterator;
    typedef ImageIterator<const PixelType> const_iterator;

    Image(size_t _width, size_t _height) :
            m_imageData(0),
            m_width(0),
            m_height(0),
            m_stride(0)
    {
        create(_width, _height);
    }

    Image(const std::string& fileName) :
    m_imageData(0),
    m_width(0),
    m_height(0),
    m_stride(0)
    {
        load(fileName);
    }

    ~Image()
    {
        delete m_imageData;
    }

    size_t width() const { return m_width; }
    size_t height() const { return m_height; }
    size_t stride() const { return m_stride; }

    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, m_height ); }
    const_iterator begin() const { return const_iterator(this, 0); }
    const_iterator end() const { return const_iterator(this, m_height); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    PixelType* data() { return m_imageData; }
    const PixelType* data() const { return m_imageData; }

    void save(const std::string& fileName)
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
        assert(m_imageData == nullptr);

        size_t blockSize = lcm(CACHE_LINE_SIZE, sizeof(PixelType)) / sizeof(PixelType);
        m_stride = ((_width + blockSize - 1) / blockSize) * blockSize;
        m_imageData = new PixelType[m_stride * _height];
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
