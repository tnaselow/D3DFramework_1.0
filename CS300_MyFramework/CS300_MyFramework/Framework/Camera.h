#pragma once
#define GLM_FORCE_SWIZZLE 
#include "glm/glm.hpp"

class Camera
{
	public:
		Camera(glm::vec3 forward = glm::vec3(0, 0, -1), glm::vec3 position = glm::vec3(0, 0, 0))
			: m_Forward(forward), m_Up(0, 1, 0), m_Right(1, 0, 0), m_Position(0,0,0), m_Pitch(0), m_Yaw(0){}

		//void setForward(glm::vec3 forward)
		//{
		//	m_Forward = glm::normalize(forward);
		//	m_Right = glm::cross(forward, m_WorldUp);
		//	m_Right = glm::normalize(m_Right);
		//	m_Up = glm::cross(m_Right, m_Forward);
		//	m_Up = glm::normalize(m_Up);
		//}

		glm::mat4 getTransform()
		{
			return glm::inverse(glm::mat4(glm::vec4(m_Right.x, m_Right.y, m_Right.z, 0),
					glm::vec4(m_Up.x, m_Up.y, m_Up.z, 0), -glm::vec4(m_Forward.x, m_Forward.y, m_Forward.z, 0),
				glm::vec4(m_Position, 1)));

			//glm::mat4 camToWorld(glm::mat4(glm::vec4(m_Right.x, m_Right.y, m_Right.z, 0),
			//	glm::vec4(m_Up.x, m_Up.y, m_Up.z, 0),
			//	-glm::vec4(m_Forward.x, m_Forward.y, m_Forward.z, 0), glm::vec4(0, 0, 0, 1)));
			//
			//glm::mat4 camTrans(glm::vec4(1, 0, 0, 0),
			//	glm::vec4(0, 1, 0, 0),
			//	glm::vec4(0, 0, 1, 0),
			//	glm::vec4(-m_Position, 1));
			//
			//return glm::transpose(camToWorld) * camTrans;
		}

		void moveForward(float dist) { m_Position += m_Forward * dist; }
		void moveRight(float dist) { m_Position += m_Right * dist; }
		void moveUp(float dist) { m_Position += m_Up * dist; }

		void setPitch(float pitch) { m_Pitch = pitch; calculateVectors(); }
		void setYaw(float yaw) { m_Yaw = yaw; calculateVectors(); }
		float getPitch() { return m_Pitch; }
		float getYaw() { return m_Yaw; }

		void setPosition(glm::vec3 position) { m_Position = position; }

		glm::vec3 getForward() { return m_Forward; }
		glm::vec3 getUp() { return m_Up; }
		glm::vec3 getRight() { return m_Right; }

		glm::vec3 getPosition() { return m_Position; }

	private:
		void calculateVectors()
		{
			if (m_Pitch > 89.0f)
				m_Pitch = 89.0f;
			if (m_Pitch < -89.0f)
				m_Pitch = -89.0f;

			m_Forward.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			m_Forward.y = sin(glm::radians(m_Pitch));
			m_Forward.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			m_Forward = glm::normalize(m_Forward);
		
			m_Right = glm::normalize(glm::cross(m_Forward, m_WorldUp));
			m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
		}

	private:
		float m_Pitch, m_Yaw;
		glm::vec3 m_Position;
		glm::vec3 m_Forward, m_Up, m_Right;
		glm::mat4 m_CamTransform;
		glm::vec3 m_WorldUp = glm::vec3(0, 1, 0);
};
