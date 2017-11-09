#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>
#include <ostream>
#include "assert.h"

/*
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
 | ----------------------------------- three-dimensional vector ---------------------------------- |
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
*/
namespace Algebraica {
  template<typename T> class vec3
  {
  public:
    vec3(T x = 0, T y = 0, T z = 0) :
      values_{x, y, z} {}
    vec3(const vec3<T> &v){ *this = v; }

    const T x() const { return values_[0]; }
    const T y() const { return values_[1]; }
    const T z() const { return values_[2]; }

    /*
   * index array access
   */
    T& operator[](const unsigned int index){
      assert(index >= 0 && index < 3);
      return values_[index];
    }
    /*
   * constat index array access
   */
    const T operator[](const unsigned int index) const{
      assert(index >= 0 && index < 3);
      return values_[index];
    }
    // vector addition
    vec3<T> operator+(const vec3<T> &v){
      return vec3<T>(values_[0] + v.x(), values_[1] + v.y(), values_[2] + v.z());
    }
    // vector substraction
    vec3<T> operator-(const vec3<T> &v){
      return vec3<T>(values_[0] - v.x(), values_[1] - v.y(), values_[2] - v.z());
    }
    // vector multiplication
    vec3<T> operator*(const vec3<T> &v){
      return vec3<T>(values_[0] * v.x(), values_[1] * v.y(), values_[2] * v.z());
    }
    vec3<T> operator*(const T s){
      return vec3<T>(values_[0] * s, values_[1] * s, values_[2] * s);
    }
    // vector division
    vec3<T> operator/(const vec3<T> &v){
      return vec3<T>(values_[0] / v.x(), values_[1] / v.y(), values_[2] / v.z());
    }
    vec3<T> operator/(const T s){
      return vec3<T>(values_[0] / s, values_[1] / s, values_[2] / s);
    }
    // vector assigment addition
    vec3<T>& operator+=(const vec3<T> &v){
      values_[0] += v.x();
      values_[1] += v.y();
      values_[2] += v.z();
      return *this;
    }
    // vector assigment substraction
    vec3<T>& operator-=(const vec3<T> &v){
      values_[0] -= v.x();
      values_[1] -= v.y();
      values_[2] -= v.z();
      return *this;
    }
    // vector assigment multiplication
    vec3<T>& operator*=(const vec3<T> &v){
      values_[0] *= v.x();
      values_[1] *= v.y();
      values_[2] *= v.z();
      return *this;
    }
    // vector assigment multiplication
    vec3<T>& operator*=(const T s){
      values_[0] *= s;
      values_[1] *= s;
      values_[2] *= s;
      return *this;
    }
    // vector assigment division
    vec3<T>& operator/=(const vec3<T> &v){
      values_[0] /= v.x();
      values_[1] /= v.y();
      values_[2] /= v.z();
      return *this;
    }
    // vector assigment division
    vec3<T>& operator/=(const T s){
      values_[0] /= s;
      values_[1] /= s;
      values_[2] /= s;
      return *this;
    }
    // vector increment prefix
    vec3<T>& operator++(){
      ++values_[0];
      ++values_[1];
      ++values_[2];
      return *this;
    }
    // vector increment postfix
    vec3<T> operator++(int){
      vec3<T> temp(*this);
      ++*this;
      return temp;
    }
    // vector decrement prefix
    vec3<T>& operator--(){
      --values_[0];
      --values_[1];
      --values_[2];
      return *this;
    }
    // vector decrement postfix
    vec3<T> operator--(int){
      vec3<T> temp(*this);
      --*this;
      return temp;
    }
    vec3<T>& operator ()(const T x = 0, const T y = 0, const T z = 0){
      values_[0] = x;
      values_[1] = y;
      values_[2] = z;
      return *this;
    }
    // printing the values
    friend std::ostream& operator<<(std::ostream &os, const vec3<T> &v){
      return os << "vec3(" << v.x() << ", " << v.y() << ", " << v.z() << ") ";
    }
    // verifying if two vectors are equal
    const bool operator==(const vec3<T> &v){
      return values_[0] == v.x() && values_[1] == v.y() && values_[2] == v.z();
    }
    // verifying if two vectors are different
    const bool operator!=(const vec3<T> &v){
      return !(values_[0] == v.x() && values_[1] == v.y() && values_[2] == v.z());
    }
    // vectors dimensional comparation (greater than)
    const bool operator>(const vec3<T> &v){
      return length() > v.length();
    }
    // vectors dimensional comparation (lesser than)
    const bool operator<(const vec3<T> &v){
      return length() < v.length();
    }
    // vectors dimensional comparation (greater or equal than)
    const bool operator>=(const vec3<T> &v){
      return length() >= v.length();
    }
    // vectors dimensional comparation (lesser or equal than)
    const bool operator<=(const vec3<T> &v){
      return length() <= v.length();
    }
    // pointer to vector data
    T* data(){
      return &values_[0];
    }
    // constant pointer to vector data
    const T* data() const{
      return &values_[0];
    }
    // vector length
    const T length(){
      return sqrt(values_[0] * values_[0] + values_[1] * values_[1] + values_[2] * values_[2]);
    }
    // normalize this vector
    vec3<T>& normalize(){
      T d(length());
      d = (d > 0)? 1/d : 0;
      values_[0] *= d;
      values_[1] *= d;
      values_[2] *= d;
      return *this;
    }
    static vec3<T> normalize(vec3<T> v){
      T d{v.length()};
      d = (d > 0)? 1/d : 0;
      return vec3<T>(d * v.x(), d * v.y(), d * v.z());
    }
    // normalized vector
    const vec3<T> normalized(){
      T d(length());
      d = (d > 0)? 1/d : 0;
      return vec3<T>(values_[0] * d, values_[1] * d, values_[2] * d);
    }
    // cross product between two vectors
    static vec3<T> cross(const vec3<T> &u, const vec3<T> &v){
      return vec3<T>(u.y() * v.z() - v.y() * u.z(),
                     u.z() * v.x() - v.z() * u.x(),
                     u.x() * v.y() - v.x() * u.y());
    }
    // compute the dot product of two vectors
    static T dot(vec3<T> &u, const vec3<T> &v){
      vec3<T> tmp(u * v);
      return tmp.x() + tmp.y() + tmp.z();
    }
    // rotate the vector through X axis
    vec3<T>& rotateX(const T _angle) {
      values_[1] = values_[1] * cos(_angle) - values_[2] * sin(_angle);
      values_[2] = values_[1] * sin(_angle) + values_[2] * cos(_angle);
      return *this;
    }
    // rotate the vector through Y axis
    vec3<T>& rotateY(const T _angle) {
      values_[0] = values_[2] * sin(_angle) + values_[0] * cos(_angle);
      values_[2] = values_[2] * cos(_angle) - values_[0] * sin(_angle);
      return *this;
    }
    // rotate the vector through Z axis
    vec3<T>& rotateZ(const T _angle) {
      values_[0] = values_[0] * cos(_angle) - values_[1] * sin(_angle);
      values_[1] = values_[0] * sin(_angle) + values_[1] * cos(_angle);
      return *this;
    }

  private:
    T values_[3];
  };

  typedef vec3<double> vec3d;
  typedef vec3<float> vec3f;
  typedef vec3<int> vec3i;

  }

#endif // VECTOR3_H
