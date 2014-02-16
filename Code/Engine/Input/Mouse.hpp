#ifndef INCLUDED_MOUSE_HPP
#define INCLUDED_MOUSE_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "../Math/Vector2.hpp"

//-----------------------------------------------------------------------------------------------
class Mouse
{
	static const unsigned int NUMBER_OF_BUTTONS = 7;
public:
	static enum Button
	{
		LEFT_BUTTON = 1,
		RIGHT_BUTTON = 2,
		MIDDLE_BUTTON = 4,
		EXTRA_BUTTON_1 = 5,
		EXTRA_BUTTON_2 = 6
	};

private:
	//Data Members
	bool m_wasButtonDown[ NUMBER_OF_BUTTONS ];
	bool m_isButtonDown[ NUMBER_OF_BUTTONS ];
	Vector2< float > m_mouseMovement;
	float currentWheelDelta;

public:
	Mouse()
		: m_mouseMovement( 0.f, 0.f )
		, currentWheelDelta( 0.f )
	{
		for( unsigned int button = 0; button < NUMBER_OF_BUTTONS; ++button )
		{
			m_wasButtonDown[ button ] = false;
			m_isButtonDown[ button ] = false;
		}
	}

	bool IsButtonPressed ( Button button ) const { return IsButtonPressedOrHeld( button ) && !m_wasButtonDown[ button ]; }
	bool IsButtonPressedOrHeld ( Button button ) const { return m_isButtonDown[ button ]; }
	void SetButtonDown( Button button ) { m_isButtonDown[ button ] = true; }
	void SetButtonUp  ( Button button ) { m_isButtonDown[ button ] = false; }
	void SetButtonDown( unsigned char button ) { m_isButtonDown[ button ] = true; }
	void SetButtonUp  ( unsigned char button ) { m_isButtonDown[ button ] = false; }

	bool IsWheelScrolled() const      { return currentWheelDelta != 0.f; }
	bool IsWheelScrolledUp() const    { return currentWheelDelta > 0.f; }
	bool IsWheelScrolledDown() const  { return currentWheelDelta < 0.f; }
	void SetWheelDelta( float delta ) { currentWheelDelta = delta; }

	const Vector2< float >& GetMouseMovement() const { return m_mouseMovement; }
	void SetMouseMovement( float x, float y );

	void Update();
};


inline void Mouse::SetMouseMovement( float x, float y )
{
	m_mouseMovement.x = x;
	m_mouseMovement.y = y;
}

inline void Mouse::Update()
{
	for( unsigned int button = 0; button < NUMBER_OF_BUTTONS; ++button )
	{
		if( m_isButtonDown[ button ] )
			m_wasButtonDown[ button ] = true;
		else
			m_wasButtonDown[ button ] = false;
	}
	
	currentWheelDelta = 0.f;
}

#endif //INCLUDED_MOUSE_HPP
