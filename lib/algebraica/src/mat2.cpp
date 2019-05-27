
#include "algebraica/mat2.h"

namespace algebraica{
  ALGTEM mat2<T>& mat2<T>::to_identity(){
    m_[0][0] = 1, m_[0][1] = 0;
    m_[1][0] = 0, m_[1][1] = 1;
  }

  ALGTEM const T mat2<T>::determinant(){
    return m_[0][0] * m_[1][1] - m_[0][1] * m_[1][0];
  }

  ALGTEM mat2<T>& mat2<T>::invert(){
    const T inverter{1 / determinant()};
    const mat2<T> temporal = *this;
    m_[0][0] =  inverter * temporal[1][1], m_[0][1] = -inverter * temporal[0][1];
    m_[1][0] = -inverter * temporal[1][0], m_[1][1] =  inverter * temporal[0][0];

    return *this;
  }

  ALGTEM vec2<T>& mat2<T>::operator[](const unsigned int index){
    assert(index >= 0 && index < 2);
    return m_[index];
  }

  ALGTEM mat2<T>& mat2<T>::operator()(const T m00, const T m01, const T m10, const T m11){
    m_[0][0] = m00, m_[0][1] = m01;
    m_[1][0] = m10, m_[1][1] = m11;
  }

  ALGTEM const vec2<T>& mat2<T>::operator[](const unsigned int index) const{
    assert(index >= 0 && index < 2);
    return m_[index];
  }

  ALGTEM mat2<T>& mat2<T>::operator/=(const T s){
    m_[0] /= s;
    m_[1] /= s;

    return *this;
  }

  ALGTEM T* mat2<T>::data(){
    return &data_[0];
  }

  ALGTEM const T* mat2<T>::data() const{
    return &data_[0];
  }

  template class mat2<double>;
  template class mat2<float>;
  template class mat2<int>;
}
