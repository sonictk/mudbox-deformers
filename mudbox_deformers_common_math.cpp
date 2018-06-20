#include "mudbox_deformers_common_math.h"
#include <Mudbox/mudbox.h>
#include <cmath>

using std::cosf;
using std::sinf;

using mudbox::Vector;


float innerProduct(const Vector &v1, const Vector &v2)
{
	float result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);

	return result;
}


Vector crossProduct(const Vector &v1, const Vector &v2)
{
	Vector result;

	result.x = (v1.y * v2.z) - (v1.z * v2.y);
	result.y = (v1.z * v2.x) - (v1.x * v2.z);
	result.z = (v1.x * v2.y) - (v1.y * v2.x);

	return result;
}


Vector rotateBy(const Vector &v, const Vector &axis, float angle)
{
	float a = angle;

	Vector result =  (v * cosf(a)) +
		(innerProduct(v, axis) * axis * (1.0f - cosf(a))) +
		(crossProduct(axis, v) * sinf(a));

	return result;
}
