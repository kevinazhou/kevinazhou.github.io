#include "stdafx.h"
#include "BoneTransform.h"


Matrix4 BoneTransform::ToMatrix() const
{
	Matrix4 rotMatrix = Matrix4::CreateFromQuaternion(mRotation);
	Matrix4 transMatrix = Matrix4::CreateTranslation(mTranslation);
	return rotMatrix * transMatrix;
}

BoneTransform BoneTransform::Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
	Quaternion newRot = Lerp(a.mRotation, b.mRotation, f);
	Vector3 newPos = Lerp(a.mTranslation, b.mTranslation, f);
	BoneTransform result;
	result.mRotation = newRot;
	result.mTranslation = newPos;
	return result;
}
