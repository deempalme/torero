#ifndef VECTOR4_H
#define VECTOR4_H

#include <math.h>
#include <ostream>

using namespace std;

/*
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
 | ------------------------------------ four-dimensional vector ---------------------------------- |
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
*/
template<typename T> class vec4
{
public:
  vec4(T _x = 0, T _y = 0, T _z = 0, T _w = 0) :
    values({_x, _y, _z, _w}), x_n(0), y_n(0), z_n(0), w_n(0), Length(0), sizeOf(sizeof(T) * 4),
    isNormalized(false), isMeasured(false) {}
  vec4(const vec4<T> &_clone){ this = _clone; }

  const T& x(){ return *values[0]; }
  const T& y(){ return *values[1]; }
  const T& z(){ return *values[2]; }
  const T& w(){ return *values[3]; }

  // index array access
  T& operator[](const unsigned int _i){
    changed();
    switch(_i){
    case 1: return *values[1]; break;
    case 2: return *values[2]; break;
    case 3: return *values[3]; break;
    default: return *values[0]; break;
    }
  }
  // vector addition
  const vec4<T> operator+(const vec4<T> &_b){
    changed();
    return vec4<T>(values[0] + _b.x(), values[1] + _b.y(), values[2] + _b.z(), values[3] + _b.w());
  }
  // vector substraction
  const vec4<T> operator-(const vec4<T> &_b){
    changed();
    return vec4<T>(values[0] - _b.x(), values[1] - _b.y(), values[2] - _b.z(), values[3] - _b.w());
  }
  // vector multiplication
  const vec4<T> operator*(const vec4<T> &_b){
    changed();
    return vec4<T>(values[0] * _b.x(), values[1] * _b.y(), values[2] * _b.z(), values[3] * _b.w());
  }
  // vector division
  const vec4<T> operator/(const vec4<T> &_b){
    changed();
    return vec4<T>(values[0] / _b.x(), values[1] / _b.y(), values[2] / _b.z(), values[3] / _b.w());
  }
  // vector assigment addition
  vec4<T>& operator+=(const vec4<T> &_b){
    values[0] += _b.x();
    values[1] += _b.y();
    values[2] += _b.z();
    values[3] += _b.w();
    changed();
    return *this;
  }
  // vector assigment substraction
  vec4<T>& operator-=(const vec4<T> &_b){
    values[0] -= _b.x();
    values[1] -= _b.y();
    values[2] -= _b.z();
    values[3] -= _b.w();
    changed();
    return *this;
  }
  // vector assigment multiplication
  vec4<T>& operator*=(const vec4<T> &_b){
    values[0] *= _b.x();
    values[1] *= _b.y();
    values[2] *= _b.z();
    values[3] *= _b.w();
    changed();
    return *this;
  }
  // vector assigment division
  vec4<T>& operator/=(const vec4<T> &_b){
    values[0] /= _b.x();
    values[1] /= _b.y();
    values[2] /= _b.z();
    values[3] /= _b.w();
    changed();
    return *this;
  }
  // vector increment prefix
  vec4<T>& operator++(){
    ++values[0];
    ++values[1];
    ++values[2];
    ++values[3];
    changed();
    return *this;
  }
  // vector increment postfix
  vec4<T> operator++(int){
    vec4<T> temp(*this);
    ++*this;
    return temp;
  }
  // vector decrement prefix
  vec4<T>& operator--(){
    --values[0];
    --values[1];
    --values[2];
    --values[3];
    changed();
    return *this;
  }
  // vector decrement postfix
  vec4<T> operator--(int){
    vec4<T> temp(*this);
    --*this;
    return temp;
  }
  // printing the values
  ostream& operator<<(ostream &_os){
    return _os << "vec4(" << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << ")\n";
  }
  // verifying if two vectors are equal
  const bool operator==(const vec4<T> &_b){
    return values[0] == _b.x() && values[1] == _b.y() && values[2] == _b.z() && values[3] == _b.z();
  }
  // verifying if two vectors are different
  const bool operator!=(const vec4<T> &_b){
    return !(values[0] == _b.x() && values[1] == _b.y() && values[2] == _b.z() && values[3] == _b.z());
  }
  // vectors dimensional comparation (greater than)
  const bool operator>(const vec4<T> &_b){
    return length() > _b.length();
  }
  // vectors dimensional comparation (lesser than)
  const bool operator<(const vec4<T> &_b){
    return length() < _b.length();
  }
  // vectors dimensional comparation (greater or equal than)
  const bool operator>=(const vec4<T> &_b){
    return length() > _b.length() || length() == _b.length();
  }
  // vectors dimensional comparation (lesser or equal than)
  const bool operator<=(const vec4<T> &_b){
    return length() < _b.length() || length() == _b.length();
  }
  // constant pointer to vector data
  const T* data() const{
    return &values;
  }
  // data size in bytes of this vector
  const unsigned int size_of(){
    return sizeOf;
  }
  // vector length
  const T length(){
    if(!isMeasured){
      Length = sqrt(values[0]*values[0] + values[1]*values[1] + values[2]*values[2] + values[3]*values[3]);
      isMeasured = true;
    }
    return Length;
  }
  // normalized vector
  const vec4<T> normalize(){
    if(!isNormalized){
      isNormalized = true;
      T d(length());
      if(d > 0) d = 1 / d;
      x_n = values[0] * d;
      y_n = values[1] * d;
      z_n = values[2] * d;
      w_n = values[3] * d;
    }
    return vec4<T>(x_n, y_n, z_n, w_n);
  }

private:  
  void changed(){
    isNormalized = false;
    isMeasured = false;
  }

  T values[4];
  T x_n, y_n, z_n, w_n;
  T Length;
  unsigned int sizeOf;
  bool isNormalized, isMeasured;
};

typedef vec4<double> vec4d;
typedef vec4<float> vec4f;
typedef vec4<int> vec4i;

#endif // VECTOR4_H
