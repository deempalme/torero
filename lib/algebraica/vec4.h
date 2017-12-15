#ifndef VECTOR4_H
#define VECTOR4_H

#include "vec2.h"
#include "vec3.h"

#include <math.h>
#include <ostream>
#include <assert.h>

/*
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
 | ------------------------------------ four-dimensional vector ---------------------------------- |
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
*/
namespace Algebraica {
  template<typename T> class vec4
  {
  public:
    vec4(T x = 0, T y = 0, T z = 0, T w = 0) :
      values_{x, y, z, w} {}

    vec4(const vec4<T> &v){ *this = v; }

    vec4(const vec3<T> &v, const T w = 0) :
      values_{v[0], v[1], v[2], w} {}

    vec4(const vec2<T> &v) :
      values_{v[0], v[1], 0, 0} {}
    /*
   * Access the constant value of each axis
   *
   */
    const T x() const { return values_[0]; }
    const T y() const { return values_[1]; }
    const T z() const { return values_[2]; }
    const T w() const { return values_[3]; }
    /*
   * Access the value of each axis
   *
   * @param {unsigned int} index is the array's index
   */
    T& operator[](const unsigned int index){
      assert(index >= 0 && index < 4);
      return values_[index];
    }
    const T operator[](const unsigned int index) const{
      assert(index >= 0 && index < 4);
      return values_[index];
    }
    /*
   * Sets all the values
   */
    vec4<T>& operator()(const T x = 0, const T y = 0, const T z = 0, const T w = 0){
      values_[0] = x;
      values_[1] = y;
      values_[2] = z;
      values_[3] = w;
      return *this;
    }
    /*
   * Add a value to the vector
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    vec4<T> operator+(const vec4<T> &v){
      return vec4<T>(values_[0] + v[0], values_[1] + v[1], values_[2] + v[2], values_[3] + v[3]);
    }
    /*
   * @param {number} s is a scalar number
   */
    vec4<T> operator+(const T &s){
      return vec4<T>(values_[0] + s, values_[1] + s, values_[2] + s, values_[3] + s);
    }
    /*
   * Substracts a value from the vector
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    vec4<T> operator-(const vec4<T> &v){
      return vec4<T>(values_[0] - v[0], values_[1] - v[1], values_[2] - v[2], values_[3] - v[3]);
    }
    /*
   * @param {number} s is a scalar number
   */
    vec4<T> operator-(const T &s){
      return vec4<T>(values_[0] - s, values_[1] - s, values_[2] - s, values_[3] - s);
    }
    /*
   * Multiplies this vector for a value
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    vec4<T> operator*(const vec4<T> &v){
      return vec4<T>(values_[0] * v[0], values_[1] * v[1], values_[2] * v[2], values_[3] * v[3]);
    }
    /*
   * @param {vec3} v is a 3 dimensional vector
   */
    vec4<T> operator*(const vec3<T> &v){
      return vec4<T>(values_[0] * v[0], values_[1] * v[1], values_[2] * v[2], values_[3]);
    }
    /*
   * @param {number} s is a scalar number
   */
    vec4<T> operator*(const T &s){
      return vec4<T>(values_[0] * s, values_[1] * s, values_[2] * s, values_[3] * s);
    }
    /*
   * Divides this vector for a value
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    vec4<T> operator/(const vec4<T> &v){
      return vec4<T>(values_[0] / v[0], values_[1] / v[1], values_[2] / v[2], values_[3] / v[3]);
    }
    /*
   * @param {number} s is a scalar number
   */
    vec4<T> operator/(const T &s){
      return vec4<T>(values_[0] / s, values_[1] / s, values_[2] / s, values_[3] / s);
    }
    /*
   * Vector assigment addition
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    vec4<T>& operator+=(const vec4<T> &v){
      values_[0] += v[0];
      values_[1] += v[1];
      values_[2] += v[2];
      values_[3] += v[3];
      return *this;
    }
    /*
   * @param {number} s is a scalar number
   */
    vec4<T>& operator+=(const T &s){
      values_[0] += s;
      values_[1] += s;
      values_[2] += s;
      values_[3] += s;
      return *this;
    }
    /*
   * Vector assigment substraction
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    vec4<T>& operator-=(const vec4<T> &v){
      values_[0] -= v[0];
      values_[1] -= v[1];
      values_[2] -= v[2];
      values_[3] -= v[3];
      return *this;
    }
    /*
   * @param {number} s is a scalar number
   */
    vec4<T>& operator-=(const T &s){
      values_[0] -= s;
      values_[1] -= s;
      values_[2] -= s;
      values_[3] -= s;
      return *this;
    }
    /*
   * Vector assigment multiplication
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    vec4<T>& operator*=(const vec4<T> &v){
      values_[0] *= v[0];
      values_[1] *= v[1];
      values_[2] *= v[2];
      values_[3] *= v[3];
      return *this;
    }
    /*
   * @param {number} s is a scalar number
   */
    vec4<T>& operator*=(const T &s){
      values_[0] *= s;
      values_[1] *= s;
      values_[2] *= s;
      values_[3] *= s;
      return *this;
    }
    /*
   * Vector assigment division
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    vec4<T>& operator/=(const vec4<T> &v){
      values_[0] /= v[0];
      values_[1] /= v[1];
      values_[2] /= v[2];
      values_[3] /= v[3];
      return *this;
    }
    /*
   * @param {number} s is a scalar number
   */
    vec4<T>& operator/=(const T &s){
      values_[0] /= s;
      values_[1] /= s;
      values_[2] /= s;
      values_[3] /= s;
      return *this;
    }
    /*
   * Vector incremment prefix
   */
    vec4<T>& operator++(){
      ++values_[0];
      ++values_[1];
      ++values_[2];
      ++values_[3];
      return *this;
    }
    /*
   * Vector incremment suffix
   */
    vec4<T> operator++(int){
      vec4<T> temp(*this);
      ++*this;
      return temp;
    }
    /*
   * Vector decrement prefix
   */
    vec4<T>& operator--(){
      --values_[0];
      --values_[1];
      --values_[2];
      --values_[3];
      return *this;
    }
    /*
   * Vector decrement suffix
   */
    vec4<T> operator--(int){
      vec4<T> temp(*this);
      --*this;
      return temp;
    }
    /*
   * Vector's printing output
   *
   * @param {ostream} os is the output stream reference from std::ostream
   */
    friend std::ostream& operator<<(std::ostream &os, const vec4<T> &v){
      return os << "vec4(" << v.x() << ", " << v.y() << ", " << v.z() << ", " << v.w() << ") ";
    }
    /*
   * Verifying if two vectors are equal
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    const bool operator==(const vec4<T> &v){
      return values_[0] == v[0] && values_[1] == v[1] && values_[2] == v[2] && values_[3] == v[2];
    }
    /*
   * Verifying if two vectors are different
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    const bool operator!=(const vec4<T> &v){
      return !(values_[0] == v[0] && values_[1] == v[1] && values_[2] == v[2] && values_[3] == v[2]);
    }
    /*
   * Vectors dimensional comparation (greater than)
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    const bool operator>(const vec4<T> &v){
      return length() > v.length();
    }
    /*
   * Vectors dimensional comparation (lesser than)
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    const bool operator<(const vec4<T> &v){
      return length() < v.length();
    }
    /*
   * Vectors dimensional comparation (greater or equal than)
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    const bool operator>=(const vec4<T> &v){
      return length() >= v.length();
    }
    /*
   * Vectors dimensional comparation (lesser or equal than)
   *
   * @param {vec4} v is a 4 dimensional vector
   */
    const bool operator<=(const vec4<T> &v){
      return length() <= v.length();
    }
    /*
   * pointer to this vector data
   */
    const T* data() const{
      return &values_[0];
    }
    T* data(){
      return &values_[0];
    }
    /*
   * vector length
   */
    const T length(){
      return sqrt(values_[0] * values_[0] + values_[1] * values_[1] +
          values_[2] * values_[2] + values_[3] * values_[3]);;
    }
    /*
   * normalizing this vector
   */
    vec4<T>& normalize(){
      T d(length());
      d = (d > 0)? 1/d : 0;
      values_[0] *= d;
      values_[1] *= d;
      values_[2] *= d;
      values_[3] *= d;
      return *this;
    }
    /*
   * return the normalized vector
   */
    const vec4<T> normalized(){
      T d(length());
      d = (d > 0)? 1/d : 0;
      return vec4<T>(values_[0] * d, values_[1] * d, values_[2] * d, values_[3] * d);
    }
    /*
   * return the sum of each vector's element
   */
    const T add(){
      return T(values_[0] + values_[1] + values_[2] + values_[3]);
    }

  private:
    T values_[4];
  };

  typedef vec4<double> vec4d;
  typedef vec4<float> vec4f;
  typedef vec4<int> vec4i;
  }

#endif // VECTOR4_H
