#ifndef ALGEBRAICA_QUATERNION_H
#define ALGEBRAICA_QUATERNION_H

#include "algebraica/vec2.h"
#include "algebraica/vec3.h"
#include "algebraica/vec4.h"

#include <cmath>
#include <ostream>
#include <assert.h>

#ifndef ALGTEM
#define ALGTEM template<typename T>
#endif

/*
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
 | ------------------------------------------ quaternion ----------------------------------------- |
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
*/
namespace algebraica {
  ALGTEM class quaternion
  {
  public:
    quaternion(T x = 0, T y = 0, T z = 0, T w = 1) : values{x, y, z, w} {}

    quaternion(const quaternion<T> &q){ *this = q; }

    quaternion(const vec4<T> &v) : values{v[0], v[1], v[2], v[3]} {}

    quaternion(const vec3<T> &v, const T w = 1) : values{v[0], v[1], v[2], w} {}

    quaternion(const vec2<T> &v, const T z = 0, const T w = 1) : values{v[0], v[1], z, w} {}

    // Access the constant value of each axis
//    const T x() const { return values_[0]; }
//    const T y() const { return values_[1]; }
//    const T z() const { return values_[2]; }
//    const T w() const { return values_[3]; }
    // Access the value of each axis
    T& operator[](const unsigned int index);
    const T& operator[](const unsigned int index) const;
    // Sets all the values
    quaternion<T>& operator()(const T x = 0, const T y = 0, const T z = 0, const T w = 1);
    // Calculates a quaternion from euler angles
    quaternion<T>& from_euler(const T pitch = 0, const T yaw = 0, const T roll = 0);
    static quaternion<T> euler_to_quaternion(const T pitch = 0, const T yaw = 0, const T roll = 0);
    // Calculates euler angles from quaternion
    vec3<T> to_euler();
    const vec3<T> to_euler() const;
    // Calculates euler angles from quaternion
    void to_euler(T *pitch, T *yaw, T *roll);
    void to_euler(T *pitch, T *yaw, T *roll) const;
    // Extracts the X, Y and Z values from the quaternion
    vec3<T> to_3D();
    const vec3<T> to_3D() const;

    T angle();
    const T angle() const;

    T pitch();
    const T pitch() const;
    T yaw();
    const T yaw() const;
    T roll();
    const T roll() const;

    quaternion<T>& rotate(const vec3<T> axis, const T angle);
    quaternion<T>& rotate(const T x, const T y, const T z, const T angle);

    static quaternion<T> from_axis_and_angle(const vec3<T> axis, const T angle);
    static quaternion<T> from_axis_and_angle(const T x, const T y, const T z, const T angle);
    // Add a quaternion
    quaternion<T> operator+(const quaternion<T> &q);
    const quaternion<T> operator+(const quaternion<T> &q) const;
    // Add a scalar number
    quaternion<T> operator+(const T &s);
    const quaternion<T> operator+(const T &s) const;
    // Substracts a quaternion
    quaternion<T> operator-(const quaternion<T> &q);
    const quaternion<T> operator-(const quaternion<T> &q) const;
    // Substracts a scalar
    quaternion<T> operator-(const T &s);
    const quaternion<T> operator-(const T &s) const;
    // Multiplies for a quaternion
    quaternion<T> operator*(const quaternion<T> &q);
    const quaternion<T> operator*(const quaternion<T> &q) const;
    // Multiplies for a vector
    vec3<T> operator*(const vec3<T> &v);
    const vec3<T> operator*(const vec3<T> &v) const;
    // Multiplies for a scalar number
    quaternion<T> operator*(const T &s);
    const quaternion<T> operator*(const T &s) const;
    // Divides for a scalar number
    quaternion<T> operator/(const T &s);
    const quaternion<T> operator/(const T &s) const;
    // Quaternion assigment addition
    quaternion<T>& operator+=(const quaternion<T> &q);
    // Scalar assigment addition
    quaternion<T>& operator+=(const T &s);
    // Quaternion assigment substraction
    quaternion<T>& operator-=(const quaternion<T> &q);
    // Scalar assigment substraction
    quaternion<T>& operator-=(const T &s);
    // Quaternion assigment multiplication
    quaternion<T>& operator*=(const quaternion<T> &q);
    // Scalar assigment multiplication
    quaternion<T>& operator*=(const T &s);
    // Scalar assigment division
    quaternion<T>& operator/=(const T &s);
    // Quaternion's printing output
    friend std::ostream& operator<<(std::ostream &os, const quaternion<T> &q){
      return os << "quaternion(" << q.x << ", " << q.y << ", "<< q.z << ", " << q.w << ") ";
    }
    // Verifying if two quaternions are equal
    bool operator==(const quaternion<T> &q);
    bool operator==(const quaternion<T> &q) const;
    // Verifying if two quaternions are different
    bool operator!=(const quaternion<T> &q);
    bool operator!=(const quaternion<T> &q) const;
    // Quaternion dimensional comparation (greater than)
    bool operator>(const quaternion<T> &q);
    bool operator>(const quaternion<T> &q) const;
    // Quaternion dimensional comparation (lesser than)
    bool operator<(const quaternion<T> &q);
    bool operator<(const quaternion<T> &q) const;
    // Quaternion dimensional comparation (greater or equal than)
    bool operator>=(const quaternion<T> &q);
    bool operator>=(const quaternion<T> &q) const;
    // Quaternion dimensional comparation (lesser or equal than)
    bool operator<=(const quaternion<T> &q);
    bool operator<=(const quaternion<T> &q) const;

    // Casting
    inline quaternion<int> to_int(){
      return quaternion<int>(static_cast<int>(x), static_cast<int>(y),
                             static_cast<int>(z), static_cast<int>(w));
    }
    inline quaternion<int> to_int() const{
      return quaternion<int>(static_cast<int>(x), static_cast<int>(y),
                             static_cast<int>(z), static_cast<int>(w));
    }
    inline quaternion<float> to_float(){
      return quaternion<float>(static_cast<float>(x), static_cast<float>(y),
                               static_cast<float>(z), static_cast<float>(w));
    }
    inline quaternion<float> to_float() const{
      return quaternion<float>(static_cast<float>(x), static_cast<float>(y),
                               static_cast<float>(z), static_cast<float>(w));
    }
    inline quaternion<double> to_double(){
      return quaternion<double>(static_cast<double>(x), static_cast<double>(y),
                                static_cast<double>(z), static_cast<double>(w));
    }
    inline quaternion<double> to_double() const{
      return quaternion<double>(static_cast<double>(x), static_cast<double>(y),
                                static_cast<double>(z), static_cast<double>(w));
    }
    // Pointer to this quaternion data
    T* data();
    const T* data() const;
    // Normalizing this quaternion
    quaternion<T>& normalize();
    // Returns the normalized quaternion
    quaternion<T> normalized();
    const quaternion<T> normalized() const;
    // Returns the quaternion length
    T length();
    const T length() const;

//  private:
    union {
      struct {
        T x;
        T y;
        T z;
        T w;
      };
      T values[4];
    };
  };

  typedef quaternion<double> quaternionD;
  typedef quaternion<float> quaternionF;
  typedef quaternion<int> quaternionI;

  extern template class quaternion<double>;
  extern template class quaternion<float>;
  extern template class quaternion<int>;
}

#endif // ALGEBRAICA_QUATERNION_H
