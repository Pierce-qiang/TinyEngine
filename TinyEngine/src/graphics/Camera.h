#pragma once
#include "glm/glm.hpp"

namespace TEngine {
	class Camera
	{
	public:
		Camera(const glm::vec3& pos = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
		~Camera();

		glm::mat4 GetViewMatrix();
		glm::mat4 GetProjectionMatrix();

		void ProcessInput(float deltaTime);
		void ProcessCameraMovement(glm::vec3& direction, float deltaTime);
		void ProcessCameraRotation(float xoffset, float yoffset);
		void ProcessCameraScroll(float yoffset);

		// inline utility functions
		inline void SetPosition(const glm::vec3& pos) { mPos = pos; };
		inline void SetYaw(const float& yaw) { mYaw = yaw; };
		inline void SetPitch(const float& pitch) { mPitch = pitch; };

		inline float GetYaw() const { return mYaw; }
		inline float GetPitch() const { return mPitch; }
		inline float GetFOV() const { return mFOV; }
		inline const glm::vec3& GetPosition() const { return mPos; }
		inline const glm::vec3& GetFront() const { return mFront; }
		inline const glm::vec3& GetUp() const { return mUp; }

	public:
		void UpdateCameraVectors();

	private:
		glm::vec3 mPos, mFront, mUp, mRight, mWorldUp;

		// Euler Angles
		float mYaw;
		float mPitch;

		float mRotationSensitivity;
		float mMovementSpeed;
		float mFOV;

		bool mCanRotate;
	};

}

