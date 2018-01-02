#ifndef VECTOR2_H
#define VECTOR2_H

#include <math.h>
#include <ostream>

/*
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
 | ------------------------------------ two-dimensional vector ----------------------------------- |
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
*/
namespace Algebraica {
  template<typename T> class vec2
  {
  public:
    vec2(T _x = 0, T _y = 0) :
      values_{_x, _y} {}
    vec2(const vec2<T> &v){ *this = v; }

    const T x() const { return values_[0]; }
    const T y() const { return values_[1]; }

    // index array access
    T& operator[](const unsigned int index){
      return values_[index];
    }
    // vector addition
    const vec2<T> operator+(const vec2<T> &v){
      return vec2<T>(values_[0] + v[0], values_[1] + v[1]);
    }
    // vector substraction
    vec2<T> operator-(vec2<T> &v){
      return vec2<T>(values_[0] - v[0], values_[1] - v[1]);
    }
    // vector multiplication
    const vec2<T> operator*(const vec2<T> &v){
      return vec2<T>(values_[0] * v[0], values_[1] * v[1]);
    }
    // vector division
    const vec2<T> operator/(const vec2<T> &v){
      return vec2<T>(values_[0] / v[0], values_[1] / v[1]);
    }
    // vector assigment addition
    vec2<T>& operator+=(const vec2<T> &v){
      values_[0] += v[0];
      values_[1] += v[1];
      return *this;
    }
    // vector assigment substraction
    vec2<T>& operator-=(const vec2<T> &v){
      values_[0] -= v[0];
      values_[1] -= v[1];
      return *this;
    }
    // vector assigment multiplication
    vec2<T>& operator*=(const vec2<T> &v){
      values_[0] *= v[0];
      values_[1] *= v[1];
      return *this;
    }
    // vector assigment division
    vec2<T>& operator/=(const vec2<T> &v){
      values_[0] /= v[0];
      values_[1] /= v[1];
      return *this;
    }
    // vector increment prefix
    vec2<T>& operator++(){
      ++values_[0];
      ++values_[1];
      return *this;
    }
    // vector increment postfix
    vec2<T> operator++(int){
      vec2<T> temp(*this);
      ++*this;
      return temp;
    }
    // vector decrement prefix
    vec2<T>& operator--(){
      --values_[0];
      --values_[1];
      return *this;
    }
    // operator overload for ()
    vec2<T>& operator ()(const T x = 0, const T y = 0){
      values_[0] = x;
      values_[1] = y;
      return *this;
    }
    // vector decrement postfix
    vec2<T> operator--(int){
      vec2<T> temp(*this);
      --*this;
      return temp;
    }
    // printing the values
    friend std::ostream& operator<<(std::ostream &os, const vec2<T> &v){
      return os << "vec2(" << v.x() << ", " << v.y() << ") ";
    }
    // verifying if two vectors are equal
    const bool operator==(const vec2<T> &v){
      return values_[0] == v[0] && values_[1] == v[1];
    }
    // verifying if two vectors are different
    const bool operator!=(const vec2<T> &v){
      return !(values_[0] == v[0] && values_[1] == v[1]);
    }
    // vectors dimensional comparation (greater than)
    const bool operator>(const vec2<T> &v){
      return length() > v.length();
    }
    // vectors dimensional comparation (lesser than)
    const bool operator<(const vec2<T> &v){
      return length() < v.length();
    }
    // vectors dimensional comparation (greater or equal than)
    const bool operator>=(const vec2<T> &v){
      return length() >= v.length();
    }
    // vectors dimensional comparation (lesser or equal than)
    const bool operator<=(const vec2<T> &v){
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
    T length(){
      return std::sqrt(values_[0] * values_[0] + values_[1] * values_[1]);
    }
    // normalize this vector
    vec2<T>& normalize(){
      T d(length());
      if(d > 0) d = 1 / d;
      values_[0] *= d;
      values_[1] *= d;
      return *this;
    }
    // normalized vector
    vec2<T> normalized(){
      T d(length());
      if(d > 0) d = 1 / d;
      return vec2<T>(values_[0] * d, values_[1] * d);
    }
    // rotate the vector through X axis
    vec2<T>& rotate(const T _angle) {
      values_[0] = values_[0] * std::cos(_angle);
      values_[1] = values_[1] * std::sin(_angle);
      return *this;
    }

  private:
    T values_[2];
  };

  typedef vec2<double> vec2d;
  typedef vec2<float> vec2f;
  typedef vec2<int> vec2i;

}

#endif // VECTOR2_H
