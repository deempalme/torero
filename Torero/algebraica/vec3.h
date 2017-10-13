#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>
#include <ostream>

using namespace std;

/*
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
 | ----------------------------------- three-dimensional vector ---------------------------------- |
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
*/
template<typename T> class vec3
{
public:
  vec3(T _x = 0, T _y = 0, T _z = 0) :
    values({_x, _y, _z}), x_n(0), y_n(0), z_n(0), Length(0), sizeOf(sizeof(T) * 3),
    isNormalized(false), isMeasured(false) {}
  vec3(const vec3<T> &_clone){ this = _clone; }

  const T& x(){ return *values[0]; }
  const T& y(){ return *values[1]; }
  const T& z(){ return *values[2]; }

  // index array access
  T& operator[](const unsigned int _i){
    changed();
    switch(_i){
    case 1: return *values[1]; break;
    case 2: return *values[2]; break;
    default: return *values[0]; break;
    }
  }
  // vector addition
  const vec3<T> operator+(const vec3<T> &_b){
    changed();
    return vec3<T>(values[0] + _b.x(), values[1] + _b.y(), values[2] + _b.z());
  }
  // vector substraction
  const vec3<T> operator-(const vec3<T> &_b){
    changed();
    return vec3<T>(values[0] - _b.x(), values[1] - _b.y(), values[2] - _b.z());
  }
  // vector multiplication
  const vec3<T> operator*(const vec3<T> &_b){
    changed();
    return vec3<T>(values[0] * _b.x(), values[1] * _b.y(), values[2] * _b.z());
  }
  // vector division
  const vec3<T> operator/(const vec3<T> &_b){
    changed();
    return vec3<T>(values[0] / _b.x(), values[1] / _b.y(), values[2] / _b.z());
  }
  // vector assigment addition
  vec3<T>& operator+=(const vec3<T> &_b){
    values[0] += _b.x();
    values[1] += _b.y();
    values[2] += _b.z();
    changed();
    return *this;
  }
  // vector assigment substraction
  vec3<T>& operator-=(const vec3<T> &_b){
    values[0] -= _b.x();
    values[1] -= _b.y();
    values[2] -= _b.z();
    changed();
    return *this;
  }
  // vector assigment multiplication
  vec3<T>& operator*=(const vec3<T> &_b){
    values[0] *= _b.x();
    values[1] *= _b.y();
    values[2] *= _b.z();
    changed();
    return *this;
  }
  // vector assigment division
  vec3<T>& operator/=(const vec3<T> &_b){
    values[0] /= _b.x();
    values[1] /= _b.y();
    values[2] /= _b.z();
    changed();
    return *this;
  }
  // vector increment prefix
  vec3<T>& operator++(){
    ++values[0];
    ++values[1];
    ++values[2];
    changed();
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
    --values[0];
    --values[1];
    --values[2];
    changed();
    return *this;
  }
  // vector decrement postfix
  vec3<T> operator--(int){
    vec3<T> temp(*this);
    --*this;
    return temp;
  }
  // printing the values
  ostream& operator<<(ostream &_os){
    return _os << "vec3(" << values[0] << ", " << values[1] << ", " << values[2] << ")\n";
  }
  // verifying if two vectors are equal
  const bool operator==(const vec3<T> &_b){
    return values[0] == _b.x() && values[1] == _b.y() && values[2] == _b.z();
  }
  // verifying if two vectors are different
  const bool operator!=(const vec3<T> &_b){
    return !(values[0] == _b.x() && values[1] == _b.y() && values[2] == _b.z());
  }
  // vectors dimensional comparation (greater than)
  const bool operator>(const vec3<T> &_b){
    return length() > _b.length();
  }
  // vectors dimensional comparation (lesser than)
  const bool operator<(const vec3<T> &_b){
    return length() < _b.length();
  }
  // vectors dimensional comparation (greater or equal than)
  const bool operator>=(const vec3<T> &_b){
    return length() > _b.length() || length() == _b.length();
  }
  // vectors dimensional comparation (lesser or equal than)
  const bool operator<=(const vec3<T> &_b){
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
      Length = sqrt(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
      isMeasured = true;
    }
    return Length;
  }
  // normalize this vector
  vec3<T>& normalize(){
    if(!isNormalized){
      isNormalized = true;
      T d(length());
      if(d > 0) d = 1 / d;
      x_n = values[0] *= d;
      y_n = values[1] *= d;
      z_n = values[2] *= d;
    }
    return *this;
  }
  // normalized vector
  const vec3<T> normalized(){
    if(!isNormalized){
      isNormalized = true;
      T d(length());
      if(d > 0) d = 1 / d;
      x_n = values[0] * d;
      y_n = values[1] * d;
      z_n = values[2] * d;
    }
    return vec3<T>(x_n, y_n, z_n);
  }
  // rotate the vector through X axis
  vec3<T>& rotateX(const T _angle) {
    changed();
    values[1] = values[1] * cos(_angle) - values[2] * sin(_angle);
    values[2] = values[1] * sin(_angle) + values[2] * cos(_angle);
    return *this;
  }
  // rotate the vector through Y axis
  vec3<T>& rotateY(const T _angle) {
    changed();
    values[0] = values[2] * sin(_angle) + values[0] * cos(_angle);
    values[2] = values[2] * cos(_angle) - values[0] * sin(_angle);
    return *this;
  }
  // rotate the vector through Z axis
  vec3<T>& rotateZ(const T _angle) {
    changed();
    values[0] = values[0] * cos(_angle) - values[1] * sin(_angle);
    values[1] = values[0] * sin(_angle) + values[1] * cos(_angle);
    return *this;
  }

private:  
  void changed(){
    isNormalized = false;
    isMeasured = false;
  }

  T values[3];
  T x_n, y_n, z_n;
  T Length;
  unsigned int sizeOf;
  bool isNormalized, isMeasured;
};

typedef vec3<double> vec3d;
typedef vec3<float> vec3f;
typedef vec3<int> vec3i;

#endif // VECTOR3_H
