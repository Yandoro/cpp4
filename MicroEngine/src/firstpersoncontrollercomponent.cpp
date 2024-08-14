//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "firstpersoncontrollercomponent.h"
#include "input.h"
#include <XInput.h>
#include "entity.h"
#include "memath.h"
#include "transformcomponent.h"

namespace me
{

	FirstPersonControllerComponent::FirstPersonControllerComponent()
		: m_TotalPitch(0.0f)
		, m_MovementSpeed(8.0f)
		, m_RotationSpeed(10.0f)
	{

	}

	void FirstPersonControllerComponent::Update(float deltaTime)
	{
		Input* input = Input::GetInstance();
		auto transform = GetOwner()->GetComponent<TransformComponent>().lock();
		XINPUT_STATE state;
		DWORD dwResult;
		dwResult = XInputGetState(0, &state);

		if (!transform)
			return;

		// Handle camera movement
		{
			float moveForwardBackward = 0.0f;
			float moveLeftRight = 0.0f;
			float moveUpDown = 0.0f;

			// Keyboard and mouse movement
			if (input->IsKeyDown(VK_UP) || input->IsKeyDown('W') || (input->IsKeyDown(VK_LBUTTON) && input->IsKeyDown(VK_RBUTTON)))
				moveForwardBackward += m_MovementSpeed * deltaTime;
			if (input->IsKeyDown('S') || input->IsKeyDown(VK_DOWN))
				moveForwardBackward -= m_MovementSpeed * deltaTime;
			if (input->IsKeyDown(VK_RIGHT) || input->IsKeyDown('D'))
				moveLeftRight += m_MovementSpeed * deltaTime;
			if (input->IsKeyDown('A') || input->IsKeyDown(VK_LEFT))
				moveLeftRight -= m_MovementSpeed * deltaTime;
			if (input->IsKeyDown(VK_NEXT /*page down*/) || input->IsKeyDown('Q'))
				moveUpDown -= m_MovementSpeed * deltaTime;
			if (input->IsKeyDown(VK_PRIOR /* page up */) || input->IsKeyDown('E'))
				moveUpDown += m_MovementSpeed * deltaTime;

			// Controller movement handling (left thumbstick)
			if (state.Gamepad.sThumbLY > 10000 || state.Gamepad.sThumbLY < -10000)
				moveForwardBackward += static_cast<float>(state.Gamepad.sThumbLY) / 32768.0f * m_MovementSpeed * deltaTime;
			if (state.Gamepad.sThumbLX > 10000 || state.Gamepad.sThumbLX < -10000)
				moveLeftRight += static_cast<float>(state.Gamepad.sThumbLX) / 32768.0f * m_MovementSpeed * deltaTime;

			// Apply movement
			if (moveForwardBackward != 0.0f)
				transform->TranslateLocal(0, 0, moveForwardBackward);
			if (moveLeftRight != 0.0f)
				transform->TranslateLocal(moveLeftRight, 0, 0);
			if (moveUpDown != 0.0f)
				transform->Translate(0, moveUpDown, 0);
		}

		// Handle camera orientation
		{
			float pitch = 0.0f;
			float yaw = 0.0f;

			// Mouse input handling
			if (input->IsKeyDown(VK_LBUTTON))
			{
				int mouseDeltaX;
				int mouseDeltaY;
				input->GetMouseDelta(mouseDeltaX, mouseDeltaY);

				pitch += static_cast<float>(mouseDeltaY) * m_RotationSpeed * deltaTime;
				yaw += static_cast<float>(mouseDeltaX) * m_RotationSpeed * deltaTime;
			}

			// Controller input handling for right thumbstick
			if (state.Gamepad.sThumbRX > 10000 || state.Gamepad.sThumbRX < -10000)
			{
				yaw += static_cast<float>(state.Gamepad.sThumbRX) / 32768.0f * m_RotationSpeed * deltaTime;
			}
			if (state.Gamepad.sThumbRY > 10000 || state.Gamepad.sThumbRY < -10000)
			{
				pitch += static_cast<float>(state.Gamepad.sThumbRY) / 32768.0f * m_RotationSpeed * deltaTime;
			}

			m_TotalPitch += pitch;
			const float deg90 = 90.0f;

			if (m_TotalPitch > deg90) {
				pitch = deg90 - (m_TotalPitch - pitch);
				m_TotalPitch = deg90;
			}
			else if (m_TotalPitch < -deg90) {
				pitch = -deg90 - (m_TotalPitch - pitch);
				m_TotalPitch = -deg90;
			}

			if (yaw != 0.0f)
				transform->Rotate(0, yaw, 0);
			if (pitch != 0.0f)
				transform->RotateLocal(pitch, 0, 0);
		}
	}
}