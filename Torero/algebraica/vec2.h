#ifndef VECTOR2_H
#define VECTOR2_H

#include <math.h>
#include <ostream>

using namespace std;

/*
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
 | ------------------------------------ two-dimensional vector ----------------------------------- |
 +–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––+
*/
template<typename T> class vec2
{
public:
  vec2(T _x = 0, T _y = 0) :
    values({_x, _y}), x_n(0), y_n(0), Length(0), sizeOf(sizeof(T) * 2),
    isNormalized(false), isMeasured(false) {}
  vec2(const vec2<T> &_clone){ this = _clone; }

  const T& x(){ return *values[0]; }
  const T& y(){ return *values[1]; }

  // index array access
  T& operator[](const unsigned int _i){
    changed();
    switch(_i){
    case 1: return *values[1]; break;
    default: return *values[0]; break;
    }
  }
  // vector addition
  const vec2<T> operator+(const vec2<T> &_b){
    changed();
    return vec2<T>(values[0] + _b.x(), values[1] + _b.y());
  }
  // vector substraction
  const vec2<T> operator-(const vec2<T> &_b){
    changed();
    return vec2<T>(values[0] - _b.x(), values[1] - _b.y());
  }
  // vector multiplication
  const vec2<T> operator*(const vec2<T> &_b){
    changed();
    return vec2<T>(values[0] * _b.x(), values[1] * _b.y());
  }
  // vector division
  const vec2<T> operator/(const vec2<T> &_b){
    changed();
    return vec2<T>(values[0] / _b.x(), values[1] / _b.y());
  }
  // vector assigment addition
  vec2<T>& operator+=(const vec2<T> &_b){
    values[0] += _b.x();
    values[1] += _b.y();
    changed();
    return *this;
  }
  // vector assigment substraction
  vec2<T>& operator-=(const vec2<T> &_b){
    values[0] -= _b.x();
    values[1] -= _b.y();
    changed();
    return *this;
  }
  // vector assigment multiplication
  vec2<T>& operator*=(const vec2<T> &_b){
    values[0] *= _b.x();
    values[1] *= _b.y();
    changed();
    return *this;
  }
  // vector assigment division
  vec2<T>& operator/=(const vec2<T> &_b){
    values[0] /= _b.x();
    values[1] /= _b.y();
    changed();
    return *this;
  }
  // vector increment prefix
  vec2<T>& operator++(){
    ++values[0];
    ++values[1];
    changed();
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
    --values[0];
    --values[1];
    changed();
    return *this;
  }
  // vector decrement postfix
  vec2<T> operator--(int){
    vec2<T> temp(*this);
    --*this;
    return temp;
  }
  // printing the values
  ostream& operator<<(ostream &_os){
    return _os << "vec2(" << values[0] << ", " << values[1] << ")\n";
  }
  // verifying if two vectors are equal
  const bool operator==(const vec2<T> &_b){
    return values[0] == _b.x() && values[1] == _b.y();
  }
  // verifying if two vectors are different
  const bool operator!=(const vec2<T> &_b){
    return !(values[0] == _b.x() && values[1] == _b.y());
  }
  // vectors dimensional comparation (greater than)
  const bool operator>(const vec2<T> &_b){
    return length() > _b.length();
  }
  // vectors dimensional comparation (lesser than)
  const bool operator<(const vec2<T> &_b){
    return length() < _b.length();
  }
  // vectors dimensional comparation (greater or equal than)
  const bool operator>=(const vec2<T> &_b){
    return length() > _b.length() || length() == _b.length();
  }
  // vectors dimensional comparation (lesser or equal than)
  const bool operator<=(const vec2<T> &_b){
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
      Length = sqrt(values[0] * values[0] + values[1] * values[1]);
      isMeasured = true;
    }
    return Length;
  }
  // normalized vector
  const vec2<T> normalize(){
    if(!isNormalized){
      isNormalized = true;
      T d(length());
      if(d > 0) d = 1 / d;
      x_n = values[0] * d;
      y_n = values[1] * d;
    }
    return vec2<T>(x_n, y_n);
  }
  // rotate the vector through X axis
  vec2<T>& rotate(const T _angle) {
    changed();
    values[0] = values[0] * cos(_angle);
    values[1] = values[1] * sin(_angle);
    return *this;
  }

private:  
  void changed(){
    isNormalized = false;
    isMeasured = false;
  }

  T values[2];
  T x_n, y_n;
  T Length;
  unsigned int sizeOf;
  bool isNormalized, isMeasured;
};

typedef vec2<double> vec2d;
typedef vec2<float> vec2f;
typedef vec2<int> vec2i;

#endif // VECTOR2_H
