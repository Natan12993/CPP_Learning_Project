#pragma once

#include <algorithm>
#include <numeric>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <ostream>
#include <iomanip>

template<int dimension, typename T>
class Point
{
public:

    std::array<T, dimension> values {};

    Point() {}

    template <typename ...K>
    Point(T arg1, K&&... args):
        values { arg1, std::forward<K>(args)... }
    {
        static_assert(sizeof...(args) + 1 == dimension, "Number of args does not match with the dimension");
    }

    float& x() { return values[0]; }
    float x() const { return values[0]; }

    float& y() { static_assert(dimension >= 2); return values[1]; }
    float y() const { static_assert(dimension >= 2); return values[1]; }

    float& z() { static_assert(dimension >= 3); return values[2]; }
    float z() const { static_assert(dimension >= 3); return values[2]; }

    Point& operator+=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::plus<float>{});
        return *this;
    }

    Point& operator-=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::minus<float>{});
        return *this;
    }

    Point& operator*=(const T scalar)
    {
        std::transform(values.begin(), values.end(), values.begin(), [scalar](float value){return scalar * value;});
        return *this;
    }

    Point& operator*=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::multiplies<float>{});
        return *this;
    }

    Point operator+(const Point& other) const
    {
        Point result = *this;
        result += other;
        return result;
    }

    Point operator-(const Point& other) const
    {
        Point result = *this;
        result -= other;
        return result;
    }

    Point operator*(const T scalar) const
    {
        Point result = *this;
        result *= scalar;
        return result;
    }

    Point operator*(const Point& other) const
    {
        Point result = *this;
        result *= other;
        return result;
    }

    Point operator-() const {
        Point result = *this;
        result *= (-1);
        return result;
    }

    float length() const
    {
        return std::sqrt(std::accumulate(values.begin(), values.end(),0.0,[](T count, T value){return count+(value*value);}));
    }

    float distance_to(const Point& other) const { return (*this - other).length(); }

    Point& normalize(const T target_len = 1.0f)
    {
        const float current_len = length();
        if (current_len == 0)
        {
            throw std::logic_error("cannot normalize vector of length 0");
        }
        *this *= (target_len / current_len);
        return *this;
    }

    Point& cap_length(const T max_len)
    {
        assert(max_len > 0);

        const float current_len = length();
        if (current_len > max_len)
        {
            *this *= (max_len / current_len);
        }

        return *this;
    }

    friend std::ostream& operator<<( std::ostream& os, const Point& point) {
        for(T value : point.values){
            os << std::fixed << std::setw(5) << std::setprecision(1) << value;
        }
        return os;
    };
};

using Point2D = Point<2, float>;
using Point3D = Point<3, float>;

inline void test_generic_points()
{
    Point<2, float> p1 { 4.f, 8.f };
    Point<2, float> p2 { 1.f, 1.f };
    std::cout << "p1 :" << p1 << " | p2 :" << p2 << std::endl;
    auto p3 = p1 + p2;
    p1 += p2;
    p1 *= 3.f; // ou 3.f, ou 3.0 en fonction du type de Point
    std::cout << "p1 :" << p1 << " | p2 :" << p2 << " | p3 :" << p3 << std::endl;
}

// our 3D-coordinate system will be tied to the airport: the runway is parallel to the x-axis, the z-axis
// points towards the sky, and y is perpendicular to both thus,
// {1,0,0} --> {.5,.5}   {0,1,0} --> {-.5,.5}   {0,0,1} --> {0,1}
inline Point2D project_2D(const Point3D& p)
{
    return { .5f * p.x() - .5f * p.y(), .5f * p.x() + .5f * p.y() + p.z() };
}

/*inline std::ostream& operator<<(std::ostream& os, const Point3D& point)
{
    os << "x: " << point.x()  << " y: " << point.y() << " z: " << point.z() << '\n';
    return os;

}*/

/*struct Point3D
{
    //float values[3] {};
    std::array<float, 3> values {};

    Point3D() {}
    Point3D(float x, float y, float z) : values { x, y, z } {}

    float& x() { return values[0]; }
    float x() const { return values[0]; }

    float& y() { return values[1]; }
    float y() const { return values[1]; }

    float& z() { return values[2]; }
    float z() const { return values[2]; }


    Point3D& operator+=(const Point3D& other)
    {
        //x() += other.x();
        //y() += other.y();
        //z() += other.z();
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::plus<float>{});
        return *this;
    }

    Point3D& operator-=(const Point3D& other)
    {
        //x() -= other.x();
        //y() -= other.y();
        //z() -= other.z();
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::minus<float>{});
        return *this;
    }

    Point3D& operator*=(const float scalar)
    {
        //x() *= scalar;
        //y() *= scalar;
        //z() *= scalar;
        std::transform(values.begin(), values.end(), values.begin(), [scalar](float value){return scalar * value;});
        return *this;
    }

    Point3D operator+(const Point3D& other) const
    {
        Point3D result = *this;
        result += other;
        return result;
    }

    Point3D operator-(const Point3D& other) const
    {
        Point3D result = *this;
        result -= other;
        return result;
    }

    Point3D operator*(const float scalar) const
    {
        Point3D result = *this;
        result *= scalar;
        return result;
    }

    Point3D operator-() const { return Point3D { -x(), -y(), -z() }; }

    float length() const 
    { 
        return std::sqrt(std::accumulate(values.begin(), values.end(),0.0,[](float count, float value){return count+(value*value);}));
        //return std::sqrt(x() * x() + y() * y() + z() * z());
    }

    float distance_to(const Point3D& other) const { return (*this - other).length(); }

    Point3D& normalize(const float target_len = 1.0f)
    {
        const float current_len = length();
        if (current_len == 0)
        {
            throw std::logic_error("cannot normalize vector of length 0");
        }

        *this *= (target_len / current_len);
        return *this;
    }

    Point3D& cap_length(const float max_len)
    {
        assert(max_len > 0);

        const float current_len = length();
        if (current_len > max_len)
        {
            *this *= (max_len / current_len);
        }

        return *this;
    }
};

struct Point2D
{
    float values[2] {};

    Point2D() {}
    Point2D(float x, float y) : values { x, y } {}

    float& x() { return values[0]; }
    float x() const { return values[0]; }

    float& y() { return values[1]; }
    float y() const { return values[1]; }

    Point2D& operator+=(const Point2D& other)
    {
        x() += other.x();
        y() += other.y();
        return *this;
    }

    Point2D& operator*=(const Point2D& other)
    {
        x() *= other.x();
        y() *= other.y();
        return *this;
    }

    Point2D& operator*=(const float scalar)
    {
        x() *= scalar;
        y() *= scalar;
        return *this;
    }

    Point2D operator+(const Point2D& other) const
    {
        Point2D result = *this;
        result += other;
        return result;
    }

    Point2D operator*(const Point2D& other) const
    {
        Point2D result = *this;
        result *= other;
        return result;
    }

    Point2D operator*(const float scalar) const
    {
        Point2D result = *this;
        result *= scalar;
        return result;
    }
};*/

