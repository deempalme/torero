#include "algebraica/vec3.h"
#include "algebraica/quaternion.h"

namespace algebraica {
  ALGTEM T& vec3<T>::operator[](const unsigned int index){
    assert(index >= 0 && index < 3);
    return values_[index];
  }

  ALGTEM const T& vec3<T>::operator[](const unsigned int index) const{
    assert(index >= 0 && index < 3);
    return values_[index];
  }

  ALGTEM vec3<T> vec3<T>::operator+(const vec3<T> &v){
    return vec3<T>(x + v.x, y + v.y, z + v.z);
  }

  ALGTEM const vec3<T> vec3<T>::operator+(const vec3<T> &v) const{
    return vec3<T>(x + v.x, y + v.y, z + v.z);
  }

  ALGTEM vec3<T> vec3<T>::operator-(const vec3<T> &v){
    return vec3<T>(x - v.x, y - v.y, z - v.z);
  }

  ALGTEM const vec3<T> vec3<T>::operator-(const vec3<T> &v) const{
    return vec3<T>(x - v.x, y - v.y, z - v.z);
  }

  ALGTEM vec3<T> vec3<T>::operator*(const vec3<T> &v){
    return vec3<T>(x * v.x, y * v.y, z * v.z);
  }

  ALGTEM const vec3<T> vec3<T>::operator*(const vec3<T> &v) const{
    return vec3<T>(x * v.x, y * v.y, z * v.z);
  }

  ALGTEM vec3<T> vec3<T>::operator*(const T s){
    return vec3<T>(x * s, y * s, z * s);
  }

  ALGTEM const vec3<T> vec3<T>::operator*(const T s) const{
    return vec3<T>(x * s, y * s, z * s);
  }

  ALGTEM vec3<T> vec3<T>::operator/(const vec3<T> &v){
    return vec3<T>(x / v.x, y / v.y, z / v.z);
  }

  ALGTEM const vec3<T> vec3<T>::operator/(const vec3<T> &v) const{
    return vec3<T>(x / v.x, y / v.y, z / v.z);
  }

  ALGTEM vec3<T> vec3<T>::operator/(const T s){
    return vec3<T>(x / s, y / s, z / s);
  }

  ALGTEM const vec3<T> vec3<T>::operator/(const T s) const{
    return vec3<T>(x / s, y / s, z / s);
  }

  ALGTEM vec3<T>& vec3<T>::operator+=(const vec3<T> &v){
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  ALGTEM vec3<T>& vec3<T>::operator-=(const vec3<T> &v){
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  ALGTEM vec3<T>& vec3<T>::operator*=(const vec3<T> &v){
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }

  ALGTEM vec3<T>& vec3<T>::operator*=(const T s){
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }

  ALGTEM vec3<T>& vec3<T>::operator/=(const vec3<T> &v){
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
  }

  ALGTEM vec3<T>& vec3<T>::operator/=(const T s){
    x /= s;
    y /= s;
    z /= s;
    return *this;
  }

  ALGTEM vec3<T>& vec3<T>::operator++(){
    ++x;
    ++y;
    ++z;
    return *this;
  }

  ALGTEM vec3<T> vec3<T>::operator++(int){
    vec3<T> temp(*this);
    ++*this;
    return temp;
  }

  ALGTEM vec3<T>& vec3<T>::operator--(){
    --x;
    --y;
    --z;
    return *this;
  }

  ALGTEM vec3<T> vec3<T>::operator--(int){
    vec3<T> temp(*this);
    --*this;
    return temp;
  }

  ALGTEM vec3<T>& vec3<T>::operator ()(const T x, const T y, const T z){
    values_[0] = x;
    values_[1] = y;
    values_[2] = z;
    return *this;
  }

  ALGTEM bool vec3<T>::operator==(const vec3<T> &v){
    return x == v.x && y == v.y && z == v.z;
  }

  ALGTEM bool vec3<T>::operator==(const vec3<T> &v) const{
    return x == v.x && y == v.y && z == v.z;
  }

  ALGTEM bool vec3<T>::operator!=(const vec3<T> &v){
    return !(x == v.x && y == v.y && z == v.z);
  }

  ALGTEM bool vec3<T>::operator!=(const vec3<T> &v) const{
    return !(x == v.x && y == v.y && z == v.z);
  }

  ALGTEM bool vec3<T>::operator>(const vec3<T> &v){
    return length() > v.length();
  }

  ALGTEM bool vec3<T>::operator>(const vec3<T> &v) const{
    return length() > v.length();
  }

  ALGTEM bool vec3<T>::operator<(const vec3<T> &v){
    return length() < v.length();
  }

  ALGTEM bool vec3<T>::operator<(const vec3<T> &v) const{
    return length() < v.length();
  }

  ALGTEM bool vec3<T>::operator>=(const vec3<T> &v){
    return length() >= v.length();
  }

  ALGTEM bool vec3<T>::operator>=(const vec3<T> &v) const{
    return length() >= v.length();
  }

  ALGTEM bool vec3<T>::operator<=(const vec3<T> &v){
    return length() <= v.length();
  }

  ALGTEM bool vec3<T>::operator<=(const vec3<T> &v) const{
    return length() <= v.length();
  }

  ALGTEM T* vec3<T>::data(){
    return &values_[0];
  }

  ALGTEM const T* vec3<T>::data() const{
    return &values_[0];
  }

  ALGTEM T vec3<T>::length(){
    return std::sqrt(x * x + y * y + z * z);
  }

  ALGTEM const T vec3<T>::length() const{
    return std::sqrt(x * x + y * y + z * z);
  }

  ALGTEM T vec3<T>::distance(const vec3<T> &first, const vec3<T> &second){
    return vec3<T>(first - second).length();
  }

  ALGTEM vec3<T>& vec3<T>::normalize(){
    T d(length());
    d = (d > 0)? 1/d : 0;
    x *= d;
    y *= d;
    z *= d;
    return *this;
  }

  ALGTEM vec3<T> vec3<T>::normalize(vec3<T> v){
    T d{v.length()};
    d = (d > 0)? 1/d : 0;
    return vec3<T>(d * v.x, d * v.y, d * v.z);
  }

  ALGTEM vec3<T> vec3<T>::normalized(){
    T d(length());
    d = (d > 0)? 1/d : 0;
    return vec3<T>(x * d, y * d, z * d);
  }

  ALGTEM const vec3<T> vec3<T>::normalized() const{
    T d(length());
    d = (d > 0)? 1/d : 0;
    return vec3<T>(x * d, y * d, z * d);
  }

  ALGTEM vec3<T> vec3<T>::cross(const vec3<T> &u, const vec3<T> &v){
    return vec3<T>(u.y * v.z - v.y * u.z,
                   u.z * v.x - v.z * u.x,
                   u.x * v.y - v.x * u.y);
  }

  ALGTEM T vec3<T>::dot(const vec3<T> &u, const vec3<T> &v){
    vec3<T> tmp(u * v);
    return tmp.x + tmp.y + tmp.z;
  }

  ALGTEM vec3<T>& vec3<T>::rotateX(const T _angle) {
    y = y * std::cos(_angle) - z * std::sin(_angle);
    z = y * std::sin(_angle) + z * std::cos(_angle);
    return *this;
  }

  ALGTEM vec3<T>& vec3<T>::rotateY(const T _angle) {
    x = z * std::sin(_angle) + x * std::cos(_angle);
    z = z * std::cos(_angle) - x * std::sin(_angle);
    return *this;
  }

  ALGTEM vec3<T>& vec3<T>::rotateZ(const T _angle) {
    x = x * std::cos(_angle) - y * std::sin(_angle);
    y = x * std::sin(_angle) + y * std::cos(_angle);
    return *this;
  }

  ALGTEM vec3<T> &vec3<T>::rotate(const quaternion<T> quat){
    const vec3<T> Q{quat.to_3D()};
    const vec3<T> A{cross(*this, Q) + (*this) * quat[3]};
    *this += cross(A, Q) * T(2);
    return *this;
  }

  template class vec3<double>;
  template class vec3<float>;
  template class vec3<int>;
}
