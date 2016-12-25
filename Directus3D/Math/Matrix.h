/*
Copyright(c) 2016 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES ==========
#include "MathHelper.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"
//=====================

namespace Directus
{
	namespace Math
	{
		class DllExport Matrix
		{
		public:
			Matrix()
			{
				m00 = 1; m01 = 0; m02 = 0; m03 = 0;
				m10 = 0; m11 = 1; m12 = 0; m13 = 0;
				m20 = 0; m21 = 0; m22 = 1; m23 = 0;
				m30 = 0; m31 = 0; m32 = 0; m33 = 1;
			}

			Matrix(
				float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33)
			{
				this->m00 = m00; this->m01 = m01; this->m02 = m02; this->m03 = m03;
				this->m10 = m10; this->m11 = m11; this->m12 = m12; this->m13 = m13;
				this->m20 = m20; this->m21 = m21; this->m22 = m22; this->m23 = m23;
				this->m30 = m30; this->m31 = m31; this->m32 = m32; this->m33 = m33;
			}

			Matrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
			{
				Matrix mRotation = CreateRotation(rotation);

				m00 = scale.x * mRotation.m00; m01 = scale.x * mRotation.m01; m02 = scale.x * mRotation.m02; m03 = 0;
				m10 = scale.y * mRotation.m10; m11 = scale.y * mRotation.m11; m12 = scale.y * mRotation.m12; m13 = 0;
				m20 = scale.z * mRotation.m20; m21 = scale.z * mRotation.m21; m22 = scale.z * mRotation.m22; m23 = 0;
				m30 = translation.x; m31 = translation.y; m32 = translation.z; m33 = 1;
			}

			~Matrix() {}

			//= TRANSLATION ==================================================================================
			Vector3 GetTranslation() { return Vector3(m30, m31, m32); }

			static Matrix CreateTranslation(const Vector3& position)
			{
				return Matrix(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					position.x, position.y, position.z, 1
				);
			}
			//================================================================================================

			//= ROTATION =====================================================================================
			static Matrix CreateRotation(const Quaternion& rotation)
			{
				float a = 1.0f - 2.0f * rotation.y * rotation.y - 2.0f * rotation.z * rotation.z;
				float b = 2.0f * rotation.x * rotation.y + 2.0f * rotation.w * rotation.z;
				float c = 2.0f * rotation.x * rotation.z - 2.0f * rotation.w * rotation.y;
				float d = 2.0f * rotation.x * rotation.y - 2.0f * rotation.w * rotation.z;
				float e = 1.0f - 2.0f * rotation.x * rotation.x - 2.0f * rotation.z * rotation.z;
				float f = 2.0f * rotation.y * rotation.z + 2.0f * rotation.w * rotation.x;
				float g = 2.0f * rotation.x * rotation.z + 2.0f * rotation.w * rotation.y;
				float h = 2.0f * rotation.y * rotation.z - 2.0f * rotation.w * rotation.x;
				float i = 1.0f - 2.0f * rotation.x * rotation.x - 2.0f *rotation.y * rotation.y;

				return Matrix(
					a, d, g, 0.0f,
					b, e, h, 0.0f,
					c, f, i, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				);
			}

			Quaternion GetRotation()
			{
				Matrix matrix = *this;
				Quaternion q;
				float t = matrix.m00 + matrix.m11 + matrix.m22;

				if (t > 0.0f)
				{
					float invS = 0.5f / sqrtf(1.0f + t);

					q.x = (matrix.m21 - matrix.m12) * invS;
					q.y = (matrix.m02 - matrix.m20) * invS;
					q.z = (matrix.m10 - matrix.m01) * invS;
					q.w = 0.25f / invS;
				}
				else
				{
					if (matrix.m00 > matrix.m11 && matrix.m00 > matrix.m22)
					{
						float invS = 0.5f / sqrtf(1.0f + matrix.m00 - matrix.m11 - matrix.m22);

						q.x = 0.25f / invS;
						q.y = (matrix.m01 + matrix.m10) * invS;
						q.z = (matrix.m20 + matrix.m02) * invS;
						q.w = (matrix.m21 - matrix.m12) * invS;
					}
					else if (matrix.m11 > matrix.m22)
					{
						float invS = 0.5f / sqrtf(1.0f + matrix.m11 - matrix.m00 - matrix.m22);

						q.x = (matrix.m01 + matrix.m10) * invS;
						q.y = 0.25f / invS;
						q.z = (matrix.m12 + matrix.m21) * invS;
						q.w = (matrix.m02 - matrix.m20) * invS;
					}
					else
					{
						float invS = 0.5f / sqrtf(1.0f + matrix.m22 - matrix.m00 - matrix.m11);

						q.x = (matrix.m02 + matrix.m20) * invS;
						q.y = (matrix.m12 + matrix.m21) * invS;
						q.z = 0.25f / invS;
						q.w = (matrix.m10 - matrix.m01) * invS;
					}
				}

				return q;
			}
			//================================================================================================

			//= SCALE ========================================================================================
			Vector3 GetScale()
			{
				return Vector3(
					Vector3(m00, m01, m02).Length(),
					Vector3(m10, m11, m12).Length(),
					Vector3(m20, m21, m22).Length()
				);
			}

			static Matrix CreateScale(float scale) { return CreateScale(scale, scale, scale); }
			static Matrix CreateScale(const Vector3& scale) { return CreateScale(scale.x, scale.y, scale.z); }
			static Matrix CreateScale(float scaleX, float scaleY, float ScaleZ)
			{
				return Matrix(
					scaleX, 0, 0, 0,
					0, scaleY, 0, 0,
					0, 0, ScaleZ, 0,
					0, 0, 0, 1
				);
			}
			//================================================================================================		

			//= MISC ===========================================================================================================================
			static Matrix CreateLookAtLH(const Vector3& cameraPosition, const Vector3& target, const Vector3& up)
			{
				Vector3 zAxis = Vector3::Normalize(target - cameraPosition);
				Vector3 xAxis = Vector3::Normalize(Vector3::Cross(up, zAxis));
				Vector3 yAxis = Vector3::Cross(zAxis, xAxis);

				return Matrix(
					xAxis.x, yAxis.x, zAxis.x, 0,
					xAxis.y, yAxis.y, zAxis.y, 0,
					xAxis.z, yAxis.z, zAxis.z, 0,
					-Vector3::Dot(xAxis, cameraPosition), -Vector3::Dot(yAxis, cameraPosition), -Vector3::Dot(zAxis, cameraPosition), 1.0f
				);
			}

			static Matrix CreateOrthographicLH(float width, float height, float zNearPlane, float zFarPlane)
			{
				return Matrix(
					2 / width, 0, 0, 0,
					0, 2 / height, 0, 0,
					0, 0, 1 / (zFarPlane - zNearPlane), 0,
					0, 0, zNearPlane / (zNearPlane - zFarPlane), 1
				);
			}

			static Matrix CreateOrthoOffCenterLH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
			{
				return Matrix(
					2 / (right - left), 0, 0, 0,
					0, 2 / (top - bottom), 0, 0,
					0, 0, 1 / (zFarPlane - zNearPlane), 0,
					(left + right) / (left - right), (top + bottom) / (bottom - top), zNearPlane / (zNearPlane - zFarPlane), 1
				);
			}

			static Matrix CreatePerspectiveFieldOfViewLH(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
			{
				float yScale = Cot(fieldOfView / 2);
				float xScale = yScale / aspectRatio;
				float zn = nearPlaneDistance;
				float zf = farPlaneDistance;

				return Matrix(
					xScale, 0, 0, 0,
					0, yScale, 0, 0,
					0, 0, zf / (zf - zn), 1,
					0, 0, -zn * zf / (zf - zn), 0
				);
			}
			//=================================================================================================================================

			//= TRANSPOSE ====================================================================================
			Matrix Transposed() const { return Transpose(*this); }
			static Matrix Transpose(const Matrix& matrix)
			{
				return Matrix(
					matrix.m00, matrix.m10, matrix.m20, matrix.m30,
					matrix.m01, matrix.m11, matrix.m21, matrix.m31,
					matrix.m02, matrix.m12, matrix.m22, matrix.m32,
					matrix.m03, matrix.m13, matrix.m23, matrix.m33
				);
			}
			//================================================================================================

			//= INVERT =======================================================================================
			Matrix Inverted() const { return Invert(*this); }
			static Matrix Invert(const Matrix& matrix)
			{
				float v0 = matrix.m20 * matrix.m31 - matrix.m21 * matrix.m30;
				float v1 = matrix.m20 * matrix.m32 - matrix.m22 * matrix.m30;
				float v2 = matrix.m20 * matrix.m33 - matrix.m23 *matrix.m30;
				float v3 = matrix.m21 * matrix.m32 - matrix.m22 * matrix.m31;
				float v4 = matrix.m21 * matrix.m33 - matrix.m23 * matrix.m31;
				float v5 = matrix.m22 * matrix.m33 - matrix.m23 * matrix.m32;

				float i00 = (v5 * matrix.m11 - v4 * matrix.m12 + v3 * matrix.m13);
				float i10 = -(v5 * matrix.m10 - v2 * matrix.m12 + v1 * matrix.m13);
				float i20 = (v4 * matrix.m10 - v2 * matrix.m11 + v0 * matrix.m13);
				float i30 = -(v3 * matrix.m10 - v1 * matrix.m11 + v0 * matrix.m12);

				float invDet = 1.0f / (i00 * matrix.m00 + i10 * matrix.m01 + i20 * matrix.m02 + i30 * matrix.m03);

				i00 *= invDet;
				i10 *= invDet;
				i20 *= invDet;
				i30 *= invDet;

				float i01 = -(v5 * matrix.m01 - v4 * matrix.m02 + v3 * matrix.m03) * invDet;
				float i11 = (v5 * matrix.m00 - v2 * matrix.m02 + v1 * matrix.m03) * invDet;
				float i21 = -(v4 * matrix.m00 - v2 * matrix.m01 + v0 * matrix.m03) * invDet;
				float i31 = (v3 * matrix.m00 - v1 * matrix.m01 + v0 * matrix.m02) * invDet;

				v0 = matrix.m10 * matrix.m31 - matrix.m11 * matrix.m30;
				v1 = matrix.m10 * matrix.m32 - matrix.m12 * matrix.m30;
				v2 = matrix.m10 * matrix.m33 - matrix.m13 * matrix.m30;
				v3 = matrix.m11 * matrix.m32 - matrix.m12 * matrix.m31;
				v4 = matrix.m11 * matrix.m33 - matrix.m13 * matrix.m31;
				v5 = matrix.m12 * matrix.m33 - matrix.m13 * matrix.m32;

				float i02 = (v5 * matrix.m01 - v4 * matrix.m02 + v3 * matrix.m03) * invDet;
				float i12 = -(v5 * matrix.m00 - v2 * matrix.m02 + v1 * matrix.m03) * invDet;
				float i22 = (v4 * matrix.m00 - v2 * matrix.m01 + v0 * matrix.m03) * invDet;
				float i32 = -(v3 * matrix.m00 - v1 * matrix.m01 + v0 * matrix.m02) * invDet;

				v0 = matrix.m21 * matrix.m10 - matrix.m20 * matrix.m11;
				v1 = matrix.m22 * matrix.m10 - matrix.m20 * matrix.m12;
				v2 = matrix.m23 * matrix.m10 - matrix.m20 * matrix.m13;
				v3 = matrix.m22 * matrix.m11 - matrix.m21 * matrix.m12;
				v4 = matrix.m23 * matrix.m11 - matrix.m21 * matrix.m13;
				v5 = matrix.m23 * matrix.m12 - matrix.m22 * matrix.m13;

				float i03 = -(v5 * matrix.m01 - v4 * matrix.m02 + v3 * matrix.m03) * invDet;
				float i13 = (v5 * matrix.m00 - v2 * matrix.m02 + v1 * matrix.m03) * invDet;
				float i23 = -(v4 * matrix.m00 - v2 * matrix.m01 + v0 * matrix.m03) * invDet;
				float i33 = (v3 * matrix.m00 - v1 * matrix.m01 + v0 * matrix.m02) * invDet;

				return Matrix(
					i00, i01, i02, i03,
					i10, i11, i12, i13,
					i20, i21, i22, i23,
					i30, i31, i32, i33);
			}
			//================================================================================================

			void Decompose(Vector3& scale, Quaternion& rotation, Vector3& translation)
			{
				translation = GetTranslation();
				scale = GetScale();

				// let's calculate the rotation now
				if ((scale.x == 0.0f) || (scale.y == 0.0f) || (scale.z == 0.0f))
				{
					rotation = Quaternion(0, 0, 0, 1);
					return;
				}

				Matrix normalized;
				normalized.m00 = m00 / scale.x;
				normalized.m01 = m01 / scale.x;
				normalized.m02 = m02 / scale.x;
				normalized.m10 = m10 / scale.y;
				normalized.m11 = m11 / scale.y;
				normalized.m12 = m12 / scale.y;
				normalized.m20 = m20 / scale.z;
				normalized.m21 = m21 / scale.z;
				normalized.m22 = m22 / scale.z;

				rotation = normalized.GetRotation();
			}

			//= MULTIPLICATION ================================================================================================================
			Matrix operator*(const Matrix& rhs) const
			{
				return Matrix(
					m00 * rhs.m00 + m01 * rhs.m10 + m02 * rhs.m20 + m03 * rhs.m30,
					m00 * rhs.m01 + m01 * rhs.m11 + m02 * rhs.m21 + m03 * rhs.m31,
					m00 * rhs.m02 + m01 * rhs.m12 + m02 * rhs.m22 + m03 * rhs.m32,
					m00 * rhs.m03 + m01 * rhs.m13 + m02 * rhs.m23 + m03 * rhs.m33,
					m10 * rhs.m00 + m11 * rhs.m10 + m12 * rhs.m20 + m13 * rhs.m30,
					m10 * rhs.m01 + m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31,
					m10 * rhs.m02 + m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32,
					m10 * rhs.m03 + m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33,
					m20 * rhs.m00 + m21 * rhs.m10 + m22 * rhs.m20 + m23 * rhs.m30,
					m20 * rhs.m01 + m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31,
					m20 * rhs.m02 + m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32,
					m20 * rhs.m03 + m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33,
					m30 * rhs.m00 + m31 * rhs.m10 + m32 * rhs.m20 + m33 * rhs.m30,
					m30 * rhs.m01 + m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31,
					m30 * rhs.m02 + m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32,
					m30 * rhs.m03 + m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33
				);
			}

			Vector3 Matrix::operator *(const Vector3& rhs) const
			{
				Vector4 vWorking;

				vWorking.x = (rhs.x * m00) + (rhs.y * m10) + (rhs.z * m20) + m30;
				vWorking.y = (rhs.x * m01) + (rhs.y * m11) + (rhs.z * m21) + m31;
				vWorking.z = (rhs.x * m02) + (rhs.y * m12) + (rhs.z * m22) + m32;
				vWorking.w = 1 / ((rhs.x * m03) + (rhs.y * m13) + (rhs.z * m23) + m33);

				return Vector3(vWorking.x * vWorking.w, vWorking.y * vWorking.w, vWorking.z * vWorking.w);
			}
			//=================================================================================================================================

			//= COMPARISON ====================================================================================================================
			bool operator==(const Matrix& rhs) const
			{
				if (m00 != rhs.m00)
					return false;

				if (m01 != rhs.m01)
					return false;

				if (m01 != rhs.m01)
					return false;

				if (m01 != rhs.m01)
					return false;

				if (m10 != rhs.m00)
					return false;

				if (m11 != rhs.m11)
					return false;

				if (m11 != rhs.m11)
					return false;

				if (m11 != rhs.m11)
					return false;

				if (m20 != rhs.m20)
					return false;

				if (m21 != rhs.m21)
					return false;

				if (m21 != rhs.m21)
					return false;

				if (m21 != rhs.m21)
					return false;

				if (m30 != rhs.m30)
					return false;

				if (m31 != rhs.m31)
					return false;

				if (m31 != rhs.m31)
					return false;

				if (m31 != rhs.m31)
					return false;

				return true;
			}

			bool operator!=(const Matrix& b) const { return !(*this == b); }
			//===========================================================================================================================

			std::string ToString() const;

			// Column-major memory representation 
			float m00, m10, m20, m30;
			float m01, m11, m21, m31;
			float m02, m12, m22, m32;
			float m03, m13, m23, m33;
			// Note: HLSL expects column-major by default

			static const Matrix Identity;
		};

		// Reverse order operators
		inline DllExport Vector3 operator*(const Vector3& lhs, const Matrix& rhs) { return rhs * lhs; }
	}
}
