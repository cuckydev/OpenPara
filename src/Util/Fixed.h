/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Fixed.h -
	Fixed point class
*/

#pragma once

#include "Boot/OpenPara.h"

namespace OpenPara
{
	// Fixed point class template
	template <typename T, T FRAC>
	class Fixed
	{
		private:
			T x;

		public:
			using type = T;
			static constexpr T frac = FRAC;
			static constexpr T base = (T(1) << FRAC);

			// Constructors
			constexpr Fixed() : x(0) {}
			constexpr Fixed(T _x) : x(_x << FRAC) {}
			constexpr Fixed(float _x) : x(T(_x * base)) {}
			constexpr Fixed(double _x) : x(T(_x * base)) {}
			constexpr Fixed(long double _x) : x(T(_x * base)) {}

			template<typename T2, T2 FRAC2>
			constexpr Fixed(Fixed<T2, FRAC2> const &rhs) : x(*((T2*)&rhs))
			{ 
				if (FRAC2 > FRAC > 0)
					x >>= FRAC2 - FRAC;
				if (FRAC > FRAC2 > 0)
					x <<= FRAC - FRAC2;
			}

			// Functions
			Fixed<T, FRAC> floor() const
			{ Fixed<T, FRAC> result; result.x = x & ~(base - 1); return result; }
			Fixed<T, FRAC> ceil() const
			{ Fixed<T, FRAC> result; result.x = (x + (base - 1)) & ~(base - 1); return result; }
			Fixed<T, FRAC> round() const
			{ Fixed<T, FRAC> result; result.x = (x + (base >> 1)) & ~(base - 1); return result; }

			Fixed<T, FRAC> mod1() const
			{ Fixed<T, FRAC> result; result.x = x & (base - 1); return result; }

			static constexpr Fixed<T, FRAC> from_raw(T x)
			{ Fixed<T, FRAC> result; result.x = x; return result; }
			T to_raw()
			{ return x; }

			// Operators
			Fixed<T, FRAC> operator+(const Fixed<T, FRAC> &_x) const
			{ Fixed<T, FRAC> result; result.x = this->x + _x.x; return result; }
			void operator+=(const Fixed<T, FRAC> &_x)
			{ *this = *this + _x; }
			
			Fixed<T, FRAC> operator-(const Fixed<T, FRAC> &_x) const
			{ Fixed<T, FRAC> result; result.x = this->x - _x.x; return result; }
			void operator-=(const Fixed<T, FRAC> &_x)
			{ *this = *this - _x; }
			
			Fixed<T, FRAC> operator+() const
			{ return *this; }
			Fixed<T, FRAC> operator-() const
			{ Fixed<T, FRAC> result; result.x = -this->x; return result; }

			Fixed<T, FRAC> operator*(const Fixed<T, FRAC> &_x) const
			{ Fixed<T, FRAC> result; result.x = (this->x * _x.x) >> FRAC; return result; }
			Fixed<T, FRAC> operator*=(const Fixed<T, FRAC> &_x)
			{ *this = *this * _x; }

			Fixed<T, FRAC> operator*(T _x) const
			{ Fixed<T, FRAC> result; result.x = this->x * _x; return result; }
			Fixed<T, FRAC> operator*=(T _x)
			{ *this = *this * _x; }

			Fixed<T, FRAC> operator/(const Fixed<T, FRAC> &_x) const
			{ Fixed<T, FRAC> result; result.x = (this->x << FRAC) / _x.x; return result; }
			Fixed<T, FRAC> operator/=(const Fixed<T, FRAC> &_x)
			{ *this = *this * _x; }

			Fixed<T, FRAC> operator/(T _x) const
			{ Fixed<T, FRAC> result; result.x = this->x / _x; return result; }
			Fixed<T, FRAC> operator/=(T &_x)
			{ *this = *this * _x; }

			bool operator<(const Fixed<T, FRAC> &_x) const
			{ return x < _x.x; }
			bool operator>(const Fixed<T, FRAC> &_x) const
			{ return x > _x.x; }
			bool operator<=(const Fixed<T, FRAC> &_x) const
			{ return x <= _x.x; }
			bool operator>=(const Fixed<T, FRAC> &_x) const
			{ return x >= _x.x; }
			bool operator==(const Fixed<T, FRAC> &_x) const
			{ return x == _x.x; }
			bool operator!=(const Fixed<T, FRAC> &_x) const
			{ return x != _x.x; }

			operator T() const
			{ return x >> FRAC; }
	};

	// Common fixed point types
	using Fixed_4_12 = Fixed<int16_t, 12>;
	using Fixed_20_12 = Fixed<int32_t, 12>;
}
