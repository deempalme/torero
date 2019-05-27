
#include "algebraica/quaternion.h"

namespace algebraica {
  ALGTEM T& quaternion<T>::operator[](const unsigned int index){
    assert(index >= 0 && index < 4);
    return values[index];
  }

  ALGTEM const T& quaternion<T>::operator[](const unsigned int index) const{
    assert(index >= 0 && index < 4);
    return values[index];
  }

  ALGTEM quaternion<T>& quaternion<T>::operator()(const T x, const T y, const T z, const T w){
    values[0] = x;
    values[1] = y;
    values[2] = z;
    values[3] = w;
    return *this;
  }

  ALGTEM quaternion<T>& quaternion<T>::from_euler(const T pitch, const T yaw, const T roll){
    const T half{static_cast<T>(0.5)};
    const T cy {static_cast<T>(std::cos(yaw * half))};
    const T sy {static_cast<T>(std::sin(yaw * half))};
    const T cr {static_cast<T>(std::cos(roll * half))};
    const T sr {static_cast<T>(std::sin(roll * half))};
    const T cp {static_cast<T>(std::cos(pitch * half))};
    const T sp {static_cast<T>(std::sin(pitch * half))};

    x = cy * sr * cp - sy * cr * sp;
    y = cy * cr * sp + sy * sr * cp;
    z = sy * cr * cp - cy * sr * sp;
    w = cy * cr * cp + sy * sr * sp;

    return *this;
  }

  ALGTEM quaternion<T> quaternion<T>::euler_to_quaternion(const T pitch, const T yaw, const T roll){
    quaternion<T> quat;
    const T half{static_cast<T>(0.5)};
    const T cy {static_cast<T>(std::cos(yaw * half))};
    const T sy {static_cast<T>(std::sin(yaw * half))};
    const T cr {static_cast<T>(std::cos(roll * half))};
    const T sr {static_cast<T>(std::sin(roll * half))};
    const T cp {static_cast<T>(std::cos(pitch * half))};
    const T sp {static_cast<T>(std::sin(pitch * half))};

    quat.x = cy * sr * cp - sy * cr * sp;
    quat.y = cy * cr * sp + sy * sr * cp;
    quat.z = sy * cr * cp - cy * sr * sp;
    quat.w = cy * cr * cp + sy * sr * sp;

    return quat;
  }

  ALGTEM vec3<T> quaternion<T>::to_euler(){
    vec3<double> euler;
    const double sinr{2.0 * static_cast<double>(w * x + y * z)};
    const double cosr{1.0 - 2.0 * static_cast<double>(x * x + y * y)};
    euler[2] = std::atan2(sinr, cosr);

    const double sinp{2.0 * static_cast<double>(w * y - z * x)};
    if (std::fabs(sinp) >= 1.0)
      euler[0] = std::copysign(M_PI / 2.0, sinp);
    else
      euler[0] = std::asin(sinp);

    const double siny{2.0 * static_cast<double>(w * z + x * y)};
    const double cosy{1.0 - 2.0 * static_cast<double>(y * y + z * z)};
    euler[1] = std::atan2(siny, cosy);

    return static_cast<T>(euler);
  }

  ALGTEM const vec3<T> quaternion<T>::to_euler() const{
    vec3<double> euler;
    const double sinr{2.0 * static_cast<double>(w * x + y * z)};
    const double cosr{1.0 - 2.0 * static_cast<double>(x * x + y * y)};
    euler[2] = std::atan2(sinr, cosr);

    const double sinp{2.0 * static_cast<double>(w * y - z * x)};
    if (std::fabs(sinp) >= 1.0)
      euler[0] = std::copysign(M_PI / 2.0, sinp);
    else
      euler[0] = std::asin(sinp);

    const double siny{2.0 * static_cast<double>(w * z + x * y)};
    const double cosy{1.0 - 2.0 * static_cast<double>(y * y + z * z)};
    euler[1] = std::atan2(siny, cosy);

    return static_cast<T>(euler);
  }

  ALGTEM void quaternion<T>::to_euler(T *pitch, T *yaw, T *roll){
    const double sinr{2.0 * static_cast<double>(w * x + y * z)};
    const double cosr{1.0 - 2.0 * static_cast<double>(x * x + y * y)};
    *roll = static_cast<T>(std::atan2(sinr, cosr));

    const double sinp{2.0 * static_cast<double>(w * y - z * x)};
    if (std::fabs(sinp) >= 1.0)
      *pitch = static_cast<T>(std::copysign(M_PI / 2.0, sinp));
    else
      *pitch = static_cast<T>(std::asin(sinp));

    const double siny{2.0 * static_cast<double>(w * z + x * y)};
    const double cosy{1.0 - 2.0 * static_cast<double>(y * y + z * z)};
    *yaw = static_cast<T>(std::atan2(siny, cosy));
  }

  ALGTEM void quaternion<T>::to_euler(T *pitch, T *yaw, T *roll) const{
    const double sinr{2.0 * static_cast<double>(w * x + y * z)};
    const double cosr{1.0 - 2.0 * static_cast<double>(x * x + y * y)};
    *roll = static_cast<T>(std::atan2(sinr, cosr));

    const double sinp{2.0 * static_cast<double>(w * y - z * x)};
    if (std::fabs(sinp) >= 1.0)
      *pitch = static_cast<T>(std::copysign(M_PI / 2.0, sinp));
    else
      *pitch = static_cast<T>(std::asin(sinp));

    const double siny{2.0 * static_cast<double>(w * z + x * y)};
    const double cosy{1.0 - 2.0 * static_cast<double>(y * y + z * z)};
    *yaw = static_cast<T>(std::atan2(siny, cosy));
  }

  ALGTEM vec3<T> quaternion<T>::to_3D(){
    return vec3<T>(x, y, z);
  }

  ALGTEM const vec3<T> quaternion<T>::to_3D() const{
    return vec3<T>(x, y, z);
  }

  ALGTEM T quaternion<T>::angle(){
    return T(std::acos(w)) * T(2);
  }

  ALGTEM const T quaternion<T>::angle() const{
    return T(std::acos(w)) * T(2);
  }

  ALGTEM T quaternion<T>::pitch(){
    const double sinp(2.0 * static_cast<double>(w * y - z * x));
//    if(std::fabs(sinp) >= 1.0)
//      return T(std::copysign(M_PI_2, sinp));
//    else
      return T(std::asin(sinp));
  }

  ALGTEM const T quaternion<T>::pitch() const{
//    const T sinp(2.0 * (w * y - z * x));
//    if(std::fabs(sinp) >= 1.0)
//      return T(std::copysign(M_PI_2, sinp));
//    else
//      return T(std::asin(sinp));
    return T(std::atan2(2.0 * static_cast<double>(x * w + y * z),
                        1.0 - 2.0 * static_cast<double>(x * x + y * y)));
  }

  ALGTEM T quaternion<T>::yaw(){
    return T(std::atan2(2.0 * static_cast<double>(w * z + x * y),
                        1.0 - 2.0 * static_cast<double>(y * y + z * z)));
  }

  ALGTEM const T quaternion<T>::yaw() const{
    return T(std::atan2(2.0 * static_cast<double>(w * z + x * y),
                        1.0 - 2.0 * static_cast<double>(y * y + z * z)));
  }

  ALGTEM T quaternion<T>::roll(){
    return T(std::atan2(2.0 * static_cast<double>(w * x + y * z),
                        1.0 - 2.0 * static_cast<double>(x * x + y * y)));
  }

  ALGTEM const T quaternion<T>::roll() const{
    return T(std::atan2(2.0 * static_cast<double>(w * x + y * z),
                        1.0 - 2.0 * static_cast<double>(x * x + y * y)));
  }

  ALGTEM quaternion<T>& quaternion<T>::rotate(const vec3<T> axis, const T angle){
    const double a{static_cast<double>(angle)/2.0};
    const double s{std::sin(a)};

    const quaternion<T> rot(static_cast<T>(static_cast<double>(axis[0]) * s),
                            static_cast<T>(static_cast<double>(axis[1]) * s),
                            static_cast<T>(static_cast<double>(axis[2]) * s),
                            static_cast<T>(std::cos(a)));
    return *this *= rot;
  }

  ALGTEM quaternion<T>& quaternion<T>::rotate(const T x, const T y, const T z, const T angle){
    const double a{static_cast<double>(angle)/2.0};
    const double s{std::sin(a)};

    const quaternion<T> rot(static_cast<T>(static_cast<double>(x) * s),
                            static_cast<T>(static_cast<double>(y) * s),
                            static_cast<T>(static_cast<double>(z) * s),
                            static_cast<T>(std::cos(a)));
    return *this *= rot;
  }

  ALGTEM quaternion<T> quaternion<T>::from_axis_and_angle(const vec3<T> axis, const T angle){
    const double a{static_cast<double>(angle)/2};
    const double s{std::sin(a)};

    return quaternion<T>(static_cast<T>(static_cast<double>(axis[0]) * s),
                         static_cast<T>(static_cast<double>(axis[1]) * s),
                         static_cast<T>(static_cast<double>(axis[2]) * s),
                         static_cast<T>(std::cos(a)));
  }

  ALGTEM quaternion<T> quaternion<T>::from_axis_and_angle(const T x, const T y, const T z,
                                                          const T angle){
    const double a{static_cast<double>(angle)/2.0};
    const double s{std::sin(a)};

    return quaternion<T>(static_cast<T>(static_cast<double>(x) * s),
                         static_cast<T>(static_cast<double>(y) * s),
                         static_cast<T>(static_cast<double>(z) * s),
                         static_cast<T>(std::cos(a)));
  }

  ALGTEM quaternion<T> quaternion<T>::operator/(const T &s){
    return quaternion<T>(x / s, y / s, z / s, w / s);
  }

  ALGTEM const quaternion<T> quaternion<T>::operator/(const T &s) const{
    return quaternion<T>(x / s, y / s, z / s, w / s);
  }

  ALGTEM quaternion<T> quaternion<T>::operator*(const T &s){
    return quaternion<T>(x * s, y * s, z * s, w * s);
  }

  ALGTEM const quaternion<T> quaternion<T>::operator*(const T &s) const{
    return quaternion<T>(x * s, y * s, z * s, w * s);
  }

  ALGTEM quaternion<T> quaternion<T>::operator*(const quaternion<T> &q){
    return quaternion<T>( x * q.w + y * q.z - z * q.y + w * q.x,
                         -x * q.z + y * q.w + z * q.x + w * q.y,
                          x * q.y - y * q.x + z * q.w + w * q.z,
                         -x * q.x - y * q.y - z * q.z + w * q.w);
  }

  ALGTEM const quaternion<T> quaternion<T>::operator*(const quaternion<T> &q) const{
    return quaternion<T>( x * q.w + y * q.z - z * q.y + w * q.x,
                         -x * q.z + y * q.w + z * q.x + w * q.y,
                          x * q.y - y * q.x + z * q.w + w * q.z,
                         -x * q.x - y * q.y - z * q.z + w * q.w);
  }

  ALGTEM vec3<T> quaternion<T>::operator*(const vec3<T> &v){
    const vec3<T> Q{x, y, z};
    const vec3<T> A{vec3<T>::cross(v, Q) + v * w};

    return v + vec3<T>::cross(A, Q) * T(2);
  }

  ALGTEM const vec3<T> quaternion<T>::operator*(const vec3<T> &v) const{
    const vec3<T> Q{x, y, z};
    const vec3<T> A{vec3<T>::cross(v, Q) + v * w};

    return v + vec3<T>::cross(A, Q) * T(2);
  }

  ALGTEM quaternion<T> quaternion<T>::operator-(const T &s){
    return quaternion<T>(x - s, y - s, z - s, w - s);
  }

  ALGTEM const quaternion<T> quaternion<T>::operator-(const T &s) const{
    return quaternion<T>(x - s, y - s, z - s, w - s);
  }

  ALGTEM quaternion<T> quaternion<T>::operator-(const quaternion<T> &q){
    return quaternion<T>(x - q.x, y - q.y, z - q.z, w - q.w);
  }

  ALGTEM const quaternion<T> quaternion<T>::operator-(const quaternion<T> &q) const{
    return quaternion<T>(x - q.x, y - q.y, z - q.z, w - q.w);
  }

  ALGTEM quaternion<T> quaternion<T>::operator+(const T &s){
    return quaternion<T>(x + s, y + s, z + s, w + s);
  }

  ALGTEM const quaternion<T> quaternion<T>::operator+(const T &s) const{
    return quaternion<T>(x + s, y + s, z + s, w + s);
  }

  ALGTEM quaternion<T> quaternion<T>::operator+(const quaternion<T> &q){
    return quaternion<T>(x + q.x, y + q.y, z + q.z, w + q.w);
  }

  ALGTEM const quaternion<T> quaternion<T>::operator+(const quaternion<T> &q) const{
    return quaternion<T>(x + q.x, y + q.y, z + q.z, w + q.w);
  }

  ALGTEM quaternion<T> &quaternion<T>::operator/=(const T &s){
    x /= s;
    y /= s;
    z /= s;
    w /= s;
    return *this;
  }

  ALGTEM quaternion<T> &quaternion<T>::operator*=(const quaternion<T> &q){
    quaternion<T> value(*this);
    x =  value.x * q.w + value.y * q.z - value.z * q.y + value.w * q.x;
    y = -value.x * q.z + value.y * q.w + value.z * q.x + value.w * q.y;
    z =  value.x * q.y - value.y * q.x + value.z * q.w + value.w * q.z;
    w = -value.x * q.x - value.y * q.y - value.z * q.z + value.w * q.w;
    return *this;
  }

  ALGTEM quaternion<T> &quaternion<T>::operator*=(const T &s){
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
  }

  ALGTEM quaternion<T> &quaternion<T>::operator-=(const quaternion<T> &q){
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
    return *this;
  }

  ALGTEM quaternion<T> &quaternion<T>::operator-=(const T &s){
    x -= s;
    y -= s;
    z -= s;
    w -= s;
    return *this;
  }

  ALGTEM quaternion<T> &quaternion<T>::operator+=(const quaternion<T> &q){
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
    return *this;
  }

  ALGTEM quaternion<T> &quaternion<T>::operator+=(const T &s){
    x += s;
    y += s;
    z += s;
    w += s;
    return *this;
  }

  ALGTEM bool quaternion<T>::operator==(const quaternion<T> &q){
    return x == q.x && y == q.y && z == q.z && w == q.w;
  }

  ALGTEM bool quaternion<T>::operator==(const quaternion<T> &q) const{
    return x == q.x && y == q.y && z == q.z && w == q.w;
  }

  ALGTEM bool quaternion<T>::operator!=(const quaternion<T> &q){
    return !(x == q.x && y == q.y && z == q.z && w == q.w);
  }

  ALGTEM bool quaternion<T>::operator!=(const quaternion<T> &q) const{
    return !(x == q.x && y == q.y && z == q.z && w == q.w);
  }

  ALGTEM bool quaternion<T>::operator>(const quaternion<T> &q){
    return length() > q.length();
  }

  ALGTEM bool quaternion<T>::operator>(const quaternion<T> &q) const{
    return length() > q.length();
  }

  ALGTEM bool quaternion<T>::operator<(const quaternion<T> &q){
    return length() < q.length();
  }

  ALGTEM bool quaternion<T>::operator<(const quaternion<T> &q) const{
    return length() < q.length();
  }

  ALGTEM bool quaternion<T>::operator>=(const quaternion<T> &q){
    return length() >= q.length();
  }

  ALGTEM bool quaternion<T>::operator>=(const quaternion<T> &q) const{
    return length() >= q.length();
  }

  ALGTEM bool quaternion<T>::operator<=(const quaternion<T> &q){
    return length() <= q.length();
  }

  ALGTEM bool quaternion<T>::operator<=(const quaternion<T> &q) const{
    return length() <= q.length();
  }

  ALGTEM T *quaternion<T>::data(){
    return &x;
  }

  ALGTEM const T *quaternion<T>::data() const{
    return &x;
  }

  ALGTEM quaternion<T> &quaternion<T>::normalize(){
    T d(length());
    d = (d > 0)? 1/d : 0;
    x *= d;
    y *= d;
    z *= d;
    w *= d;
    return *this;
  }

  ALGTEM quaternion<T> quaternion<T>::normalized(){
    T d(length());
    d = (d > 0)? 1/d : 0;
    return quaternion<T>(x * d, y * d, z * d, w * d);
  }

  ALGTEM const quaternion<T> quaternion<T>::normalized() const{
    T d(length());
    d = (d > 0)? 1/d : 0;
    return quaternion<T>(x * d, y * d, z * d, w * d);
  }

  ALGTEM T quaternion<T>::length(){
    return static_cast<T>(std::sqrt(x * x + y * y + z * z + w * w));
  }

  ALGTEM const T quaternion<T>::length() const{
    return static_cast<T>(std::sqrt(x * x + y * y + z * z + w * w));
  }

  template class quaternion<double>;
  template class quaternion<float>;
  template class quaternion<int>;
}
