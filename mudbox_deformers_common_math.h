/**
 * @file   mudbox_deformers_common_math.h
 * @brief  Basic math utilities.
 */

#ifndef MUDBOX_DEFORMERS_COMMON_MATH_H
#define MUDBOX_DEFORMERS_COMMON_MATH_H

#define PI 3.14159265359f
#define DEG_TO_RAD 0.01745329251994f
#define RAD_TO_DEG 57.2957795130f
#define TAU 6.28318530717958647692f

#include <math.h>
#include <float.h>


/**
 * This function checks if the two given floating-point numbers are equal within a
 * given tolerance.
 *
 * @param a 		The first number.
 * @param b 		The second number.
 * @param epsilon 	The tolerance.
 *
 * @return 		``true`` if the numbers are equal, ``false`` otherwise.
 */
inline bool areFloatsEqual(float a, float b, float epsilon)
{
	if (fabsf(a - b) < epsilon) {
		return true;
	} else {
		return false;
	}
}

/// @overload
inline bool areFloatsEqual(float a, float b)
{
	return areFloatsEqual(a, b, FLT_EPSILON);
}

/// @overload
inline bool isEqual(float a, float b)
{
	return areFloatsEqual(a, b, FLT_EPSILON);
}

/// @overload
inline bool isEqual(double a, double b, double epsilon)
{
	if (fabs(a - b) < epsilon) {
		return true;
	} else {
		return false;
	}
}

/// @overload
inline bool isEqual(double a, double b)
{
	return isEqual(a, b, DBL_EPSILON);
}


/**
 * This function will return a random value between ``0`` and ``1``.
 *
 * @return		A normalized random value.
 */
inline float getNormalizedRandomValue()
{
	return float(rand()) / float(RAND_MAX);
}


/**
 * Linearly interpolate between ``a`` and ``b`` by factor ``t``.
 *
 * @param a 	The first value.
 * @param b 	The second value.
 * @param t 	The interpolation factor.
 *
 * @return 	The interpolated value.
 */
inline float lerp(float a, float b, float t)
{
	float result = ((1.0f - t) * a) + (t * b);

	return result;
}

/// @overload
inline double lerp(double a, double b, double t)
{
	double result = ((1.0 - t) * a) + (t * b);

	return result;
}

/// @overload
inline float lerp(int a, int b, float t)
{
	float result = ((1.0f - t) * (float)a) + (t * (float)b);

	return result;
}

/// @overload
inline float lerp(unsigned int a, unsigned int b, float t)
{
	float result = ((1.0f - t) * (float)a) + (t * (float)b);

	return result;
}


/**
 * Clamps the given ``val`` to between the given ``min`` and ``max`` limits.
 *
 * @param val 	The value to clamp.
 * @param min 	The lower bound.
 * @param max 	The upper bound.
 *
 * @return 	The clamped value.
 */
inline int clamp(int val, int min, int max)
{
	int result = val;

	if (val < min) {
		result = min;
	} else if (val > max) {
		result = max;
	}

	return result;
}

/// @overload
inline unsigned int clamp(unsigned int val, unsigned int min, unsigned int max)
{
	unsigned int result = val;

	if (val < min) {
		result = min;
	} else if (val > max) {
		result = max;
	}

	return result;
}

/// @overload
inline float clamp(float val, float min, float max)
{
	float result = val;

	if (val < min) {
		result = min;
	} else if (val > max) {
		result = max;
	}

	return result;
}

/// @overload
inline double clamp(double val, double min, double max)
{
	double result = val;

	if (val < min) {
		result = min;
	} else if (val > max) {
		result = max;
	}

	return result;
}


/**
 * Calculates the linear parameter that produces the interpolant ``val`` within the
 * range ``[min, max]``.
 *
 * @param val		The interpolant.
 * @param min		The minimum value of the range.
 * @param max		The maximum value of the range.
 *
 * @return			The linear parameter.
 */
inline float unlerp(float val, float min, float max)
{
	return (val - min) / (max - min);
}

inline double unlerp(double val, double min, double max)
{
	return (val - min) / (max - min);
}


#ifdef __cplusplus
/**
 * Linearly interpolate between ``a`` and ``b`` by factor ``t``. The type provided
 * must support the ``*`` and assignment operands.
 * NOTE: This is precise (guarantees result is ``v1`` when t = 1).
 *
 * @param a 	The first value.
 * @param b 	The second value.
 * @param t 	The interpolation factor.
 *
 * @return 	The interpolated value.
 */
template<typename T>
inline T lerp(T a, T b, float factor)
{
	T result = static_cast<T>(((1.0f - factor) * a) + (factor * b));

	return result;
}


/**
 * Linearly interpolate between ``a`` and ``b`` by factor ``t``. The type provided
 * must support the ``*`` and assignment operands.
 * NOTE: ``qlerp`` stands for quicklerp; this is imprecise and does not guarantee
 * result is ``v1`` when ``t`` is ``1``,
 * due to floating-point error.
 * May be used when the hardware has a FMA instruction.
 *
 * @param a 	The first value.
 * @param b 	The second value.
 * @param t 	The interpolation factor.
 *
 * @return 	The interpolated value.
 */
template<typename T>
inline T qlerp(T a, T b, float factor)
{
	T result = a + (factor * (b - a));

	return result;
}


/**
 * Clamps the given ``val`` to between the given ``min`` and ``max`` limits.
 * The type provided must support the ``<``, ``>`` and assignment operands.
 *
 * @param val 	The value to clamp.
 * @param min 	The lower bound.
 * @param max 	The upper bound.
 *
 * @return 	The clamped value.
 */
template<typename T>
inline T clamp(T val, T min, T max)
{
	T result = val;

	if (val < min) {
		result = min;
	} else if (val > max) {
		result = max;
	}

	return result;
}
#endif // __cplusplus


/**
 * Finds the number of digits for the given number.
 *
 * @param num 	The number to check.
 *
 * @return 	The number of digits.
 */
inline int findNumberOfDigits(int num)
{
	int n = 0;
	while (num) {
		num /= 10;
		++n;
	}

	return n;
}


/**
 * Calculates the inner product (dot product) of the given vectors.
 *
 * @param v1 	The first vector.
 * @param v2 	The second vector.
 *
 * @return 	The inner product.
 */
float innerProduct(const mudbox::Vector &v1, const mudbox::Vector &v2);


/**
 * Calculates the cross product of the given vectors.
 *
 * @param v1 	The first vector.
 * @param v2 	The second vector.
 *
 * @return 	The cross product.
 */
mudbox::Vector crossProduct(const mudbox::Vector &v1, const mudbox::Vector &v2);


/**
 * Rotates the given vector by a specified amount.
 *
 * @param v			The vector to rotate.
 * @param axis			The axis to rotate about.
 * @param angle		The angle to rotate by, in **radians**.
 *
 * @return				The rotated vector.
 */
mudbox::Vector rotateBy(const mudbox::Vector &v, const mudbox::Vector &axis, float angle);


#endif /* MUDBOX_DEFORMERS_COMMON_MATH_H */
