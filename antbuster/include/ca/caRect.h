#ifndef CURVEDANI_Rect_H
#define CURVEDANI_Rect_H

#include "caPoint2d.h"

namespace cAni
{
template< class T >
struct _Rect
{
    _Rect();
    _Rect(T _left,T _right, T _top, T _bottom);
    _Rect(const _Rect &t);
    T left, right, top, bottom;
    _Rect & operator &= (const _Rect &clip); // intersect rect
    _Rect operator & (const _Rect &clip) const; // intersect rect
    _Rect & operator += (const _Point2<T> &point); // offset rect by point
    _Rect operator + (const _Point2<T> &point) const; // offset rect by point
    bool operator & (const _Point2<T> &point) const; // check point in rect
    bool operator == (const _Rect &o) const;
    T GetWidth() const;
    T GetHeight() const;
    bool Visible();
};
template< class T >
inline _Rect<T>::_Rect():left(0),right(0),top(0),bottom(0)
{
}
template< class T >
inline _Rect<T>::_Rect(T _left,T _right, T _top, T _bottom):
    left(_left),right(_right),top(_top),bottom(_bottom)
{
}
template< class T >
inline _Rect<T>::_Rect(const _Rect<T> &t):left(t.left),right(t.right),top(t.top),bottom(t.bottom)
{
}
template< class T >
inline _Rect<T> & _Rect<T>::operator &= (const _Rect<T> &clip)
{
    if (left<clip.left) left = clip.left;
    if (top<clip.top) top = clip.top;
    if (right>clip.right) right = clip.right;
    if (bottom>clip.bottom) bottom = clip.bottom;
    return *this;
}
template< class T >
inline _Rect<T> _Rect<T>::operator & (const _Rect<T> &clip) const
{
    _Rect<T> t(*this);
    t &= clip;
    return t;
}
template< class T >
inline _Rect<T> & _Rect<T>::operator += (const _Point2<T> &point)
{
    left += point.x;
    right += point.x;
    top += point.y;
    bottom += point.y;
    return *this;
}
template< class T >
inline _Rect<T> _Rect<T>::operator + (const _Point2<T> &point) const
{
    _Rect<T> t(*this);
    t += point;
    return t;
}
template< class T >
inline bool _Rect<T>::operator & (const _Point2<T> &point) const
{
    return point.x >= left && point.x < right && point.y >= top && point.y < bottom;
}
template< class T >
inline bool _Rect<T>::Visible()
{
    return left<right && top<bottom;
}
template< class T >
inline bool _Rect<T>::operator == (const _Rect<T> &o) const
{
    return left == o.left && right == o.right && top == o.top && bottom == o.bottom;
}
template< class T >
inline T _Rect<T>::GetWidth() const
{
    return right - left;
}
template< class T >
inline T _Rect<T>::GetHeight() const
{
    return bottom - top;
}

typedef _Rect<short> Rect;
typedef _Rect<float> Rectf;

};

#endif//CURVEDANI_Rect_H