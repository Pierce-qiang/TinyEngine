#include "./Camera.h"

#include <iostream>

#include "../macros.h"
#include "../platform/window/InputManager.h"
#include "../platform/window/WindowManager.h"
#include "glm/gtc/matrix_transform.hpp"
namespace TEngine
{
	Camera::Camera(const glm::vec3& pos, const glm::vec3& up, float yaw, float pitch) :
		mPos(pos), mUp(up), mYaw(yaw), mPitch(pitch)
	{
		mWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		mMovementSpeed = CAMERA_SPEED;
		mRotationSensitivity = CAMERA_ROTATION_SENSITIVITY;
		mFOV = CAMERA_FOV;
		mCanRotate = false;

		UpdateCameraVectors();
	}

	Camera::~Camera()
	{
	}

	glm::mat4 Camera::GetViewMatrix()
	{
		return glm::lookAt(mPos, mPos + mFront, mUp);
	}

	glm::mat4 Camera::GetProjectionMatrix()
	{
		return glm::perspective(glm::radians(mFOV), (float)WindowManager::Instance()->GetWidth() / (float)WindowManager::Instance()->GetHeight(), NEAR_PLANE, FAR_PLANE);
	}

	void Camera::ProcessInput(float deltaTime)
	{
		// can camera rotate
		if (InputManager::Instance()->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
			mCanRotate = true;
		else
			mCanRotate = false;

		// movement speed
		if (InputManager::Instance()->IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
			mMovementSpeed = CAMERA_SPEED * 4.0f;
		else
			mMovementSpeed = CAMERA_SPEED;

		// camera movement
		glm::vec3 direction = glm::vec3(0.0f);
		if (InputManager::Instance()->IsKeyPressed(GLFW_KEY_W))
			direction += mFront;
		if (InputManager::Instance()->IsKeyPressed(GLFW_KEY_S))
			direction -= mFront;
		if (InputManager::Instance()->IsKeyPressed(GLFW_KEY_A))
			direction -= mRight;
		if (InputManager::Instance()->IsKeyPressed(GLFW_KEY_D))
			direction += mRight;
		if (InputManager::Instance()->IsKeyPressed(GLFW_KEY_Q))
			direction += mWorldUp;
		if (InputManager::Instance()->IsKeyPressed(GLFW_KEY_E))
			direction -= mWorldUp;
		ProcessCameraMovement(direction, deltaTime);

		// camera FOV
		float scrollDelta = glm::clamp(float(InputManager::Instance()->GetScrollYDelta() * 4.0), -4.0f, 4.0f);
		ProcessCameraScroll(scrollDelta);

		// camera rotation
		if (mCanRotate)
		{
			float mouseXDelta = (float)(InputManager::Instance()->GetMouseXDelta()) * mRotationSensitivity;
			//note the mouse y data start from up, so it should be inverted
			float mouseYDelta = (float)(-InputManager::Instance()->GetMouseYDelta()) * mRotationSensitivity;
			ProcessCameraRotation(mouseXDelta, mouseYDelta);
			//std::cout << "ready to rotate:"<< mFront[0]<<mFront[1]<<mFront[2] <<"\n";
		}
	}

	void Camera::ProcessCameraMovement(glm::vec3& direction, float deltaTime)
	{
		mPos += direction * deltaTime * mMovementSpeed;
	}

	void Camera::ProcessCameraRotation(float xoffset, float yoffset)
	{
		mYaw += xoffset;
		mPitch += yoffset;
		//avoid front align with worldup,also avoid strange camera
		mPitch = glm::clamp(mPitch, -89.0f, 89.0f);

		UpdateCameraVectors();
	}

	void Camera::ProcessCameraScroll(float yoffset)
	{
		mFOV -= yoffset;
		mFOV = glm::clamp(mFOV, 10.0f, CAMERA_FOV);
	}

	void Camera::UpdateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		front.y = sin(glm::radians(mPitch));
		front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		mFront = glm::normalize(front);

		// be careful that m_Front is negative z-axis
		mRight = glm::normalize(glm::cross(mFront, mWorldUp));
		mUp = glm::normalize(glm::cross(mRight, mFront));
	}
}