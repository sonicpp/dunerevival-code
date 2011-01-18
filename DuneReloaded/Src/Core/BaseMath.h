
#ifndef __CORE_MATH_H__
#define __CORE_MATH_H__

namespace Core
{

template <typename Type>
static inline Type Abs(Type _V)
{
	return _V >= 0 ? _V : -_V;
}

template <typename Type>
static inline Type Min(Type _V1,Type _V2)
{
	return _V1 > _V2 ? _V2 : _V1;
}

template <typename Type>
static inline Type Max(Type _V1,Type _V2)
{
	return _V1 < _V2 ? _V2 : _V1;
}

template <typename Type>
static inline Type Clamp(Type _Value,Type _Min,Type _Max)
{
	return _Value > _Max ? _Max : _Value < _Min ? _Min : _Value;
}

template <typename Type>
static inline Type Lerp(float _fValue,Type _Min,Type _Max)
{
	return static_cast<Type>(_Min + _fValue * (_Max - _Min));
}

template <typename Type>
static inline float Ratio(Type _Value,Type _Min,Type _Max)
{
	return Clamp<float>(float(_Value - _Min) / float(_Max - _Min), 0.0f, 1.0f);
}

}

#endif

