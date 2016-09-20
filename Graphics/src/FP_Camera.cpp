#include "FP_Camera.h"
using namespace Break::Graphics; 

FP_Camera::FP_Camera(void)
{
	mAngleX = 0;
	mAngleY = 0;
	this->Reset(0, 0, 0, 
		0, 0, -1, 
		0, 1, 0);

	SetPerspectiveProjection(45.0f,4.0f/3.0f,0.1f,100.0f);
}

FP_Camera::~FP_Camera(void)
{
}

glm::vec3 FP_Camera::GetLookDirection()
{
	return -mDirection;
}

void FP_Camera::Reset(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
{
	glm::vec3 eyePt(eyeX, eyeY, eyeZ);
	glm::vec3 centerPt(centerX, centerY, centerZ);
	glm::vec3 upVec(upX, upY, upZ);
	Reset(eyePt, centerPt, upVec);
}

void FP_Camera::Reset(const glm::vec3 &eye, const glm::vec3 &center, glm::vec3 &up)
{
	mPosition = eye;
	
	//still needs normalization
	mDirection = center - mPosition;
	//i = j x k
	mRight = glm::cross( mDirection,up );
	//j = k x i
	mUp = up;


	//normalize all
	mUp = glm::normalize(mUp);
	mRight = glm::normalize(mRight);
	mDirection = glm::normalize(mDirection);

	mViewMatrix = glm::lookAt(mPosition,center,mUp);
	//UpdateViewMatrix();
}

glm::mat4 FP_Camera::GetViewMatrix()
{
	return mViewMatrix;
}

void FP_Camera::UpdateViewMatrix()
{
	// float PI = 3.14f; 

	mDirection = glm::vec3(
		-cos(mAngleY)*sin(mAngleX), 
		 sin(mAngleY), 
		-cos(mAngleY)*cos(mAngleX));
	

	mDirection = glm::normalize(mDirection);


	mRight = glm::cross(mDirection,glm::vec3(0,1,0));

	mUp = glm::cross(mRight,mDirection);

	glm::vec3 center = mPosition + mDirection;
	mViewMatrix = glm::lookAt(mPosition,center,mUp);

	//glm::mat4 view = glm::mat4(	mRight.x,		mRight.y,		mRight.z,		-glm::dot(mPosition,mRight),
	//	                            mUp.x,			mUp.y,			mUp.z,			-glm::dot(mPosition,mUp),
	//								mDirection.x,	mDirection.y,	mDirection.z,	-glm::dot(mPosition,mDirection),
	//									0,				0,				0,				1);
	//mViewMatrix = glm::transpose(view);
}

glm::mat4 FP_Camera::GetProjectionMatrix()
{
	return mProjectionMatrix;
}

void FP_Camera::SetPerspectiveProjection(float FOV, float aspectRatio, float near, float far)
{
	mProjectionMatrix = glm::perspective(FOV,aspectRatio,near,far);
}

void FP_Camera::Slide(float stepR, float stepU, float stepD)
{
	mPosition += stepR *mRight;
	mPosition += stepU * mUp;
	//next transition needs to be tested!!.
	mPosition += stepD * mDirection;
}

void FP_Camera::Yaw(float angleDegrees)
{
	mAngleX +=angleDegrees;
}

void FP_Camera::Pitch(float angleDegrees)
{
	mAngleY +=angleDegrees;
}

void FP_Camera::Roll(float angleDegrees)
{
	//no ROLL!!!
}

void FP_Camera::Walk(float dist)
{
	mPosition += dist * mDirection;
}

void FP_Camera::Strafe(float dist)
{
	mPosition += dist *mRight;
}

void FP_Camera::Fly(float dist)
{
	mPosition += dist * mUp;
}
