#ifndef INCLUDED_KEYBOARD_HPP
#define INCLUDED_KEYBOARD_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include <queue>

//-----------------------------------------------------------------------------------------------
class Keyboard
{
	static const unsigned int NUMBER_OF_KEYS = 256;
public:
	static enum Key
	{
		A = 'A',
		B = 'B',
		C = 'C',
		D = 'D',
		E = 'E',
		F = 'F',
		G = 'G',
		H = 'H',
		I = 'I',
		J = 'J',
		K = 'K',
		L = 'L',
		M = 'M',
		N = 'N',
		O = 'O',
		P = 'P',
		Q = 'Q',
		R = 'R',
		S = 'S',
		T = 'T',
		U = 'U',
		V = 'V',
		W = 'W',
		X = 'X',
		Y = 'Y',
		Z = 'Z',

		GRAVE = 0xC0, // VK_OEM_3 in Windows
		TILDE = 0xC0, // VK_OEM_3 in Windows
		NUMBER_1 = '1',
		NUMBER_2 = '2',
		NUMBER_3 = '3',
		NUMBER_4 = '4',
		NUMBER_5 = '5',
		NUMBER_6 = '6',
		NUMBER_7 = '7',
		NUMBER_8 = '8',
		NUMBER_9 = '9',
		NUMBER_0 = '0',

		BACKSPACE	= 0x08, //VK_BACK in Windows
		ENTER		= 0x0D, // VK_RETURN in Windows
		ESCAPE		= 0x1B, //VK_ESCAPE in Windows
		TAB			= 0x09, //VK_TAB in Windows

		INSERT		= 0x2D, //VK_INSERT in Windows
		DELETE_KEY	= 0x2E, //VK_DELETE in Windows
		HOME		= 0x24, //VK_HOME in Windows
		END			= 0x23, //VK_END in Windows
		PAGE_UP		= 0x21, //VK_PRIOR in Windows
		PAGE_DOWN	= 0x22, //VK_NEXT in Windows

		ARROW_UP	= 0x26, // VK_UP in Windows
		ARROW_DOWN	= 0x28,	// VK_DOWN in Windows
		ARROW_LEFT	= 0x25,	// VK_LEFT in Windows
		ARROW_RIGHT = 0x27,	// VK_RIGHT in Windows

		NUMPAD_0		= 0x60, // VK_NUMPAD0 in Windows
		NUMPAD_1		= 0x61, // VK_NUMPAD1 in Windows
		NUMPAD_2		= 0x62, // VK_NUMPAD2 in Windows
		NUMPAD_3		= 0x63, // VK_NUMPAD3 in Windows
		NUMPAD_4		= 0x64, // VK_NUMPAD4 in Windows
		NUMPAD_5		= 0x65, // VK_NUMPAD5 in Windows
		NUMPAD_6		= 0x66, // VK_NUMPAD6 in Windows
		NUMPAD_7		= 0x67, // VK_NUMPAD7 in Windows
		NUMPAD_8		= 0x68, // VK_NUMPAD8 in Windows
		NUMPAD_9		= 0x69, // VK_NUMPAD9 in Windows
		NUMPAD_PERIOD	= 0x6E, // VK_DECIMAL in Windows
		NUMPAD_PLUS		= 0x6B, // VK_ADD in Windows
		NUMPAD_MINUS	= 0x6D, // VK_SUBTRACT in Windows
		NUMPAD_TIMES	= 0x6A, // VK_MULTIPLY in Windows
		NUMPAD_DIVIDE	= 0x6F, // VK_DIVIDE in Windows
	};

private:
	bool wasKeyDown[ NUMBER_OF_KEYS ];
	bool isKeyDown[ NUMBER_OF_KEYS ];
	std::queue< unsigned char > characterInput;

public:
	Keyboard()
	{
		for( unsigned int i = 0; i < NUMBER_OF_KEYS; ++i )
		{
			isKeyDown[ i ] = false;
			wasKeyDown[ i ] = false;
		}
	}

	void AddCharacterToInputQueue( unsigned char key ) { characterInput.push( key ); }
	void ClearInputQueue();
	bool InputQueueIsEmpty() const { return characterInput.empty(); }
	unsigned char GetCharacterFromInputQueue();

	bool KeyIsPressedOrHeld( Key key ) const	{ return isKeyDown[ key ];  }
	bool KeyIsNotPressedOrHeld( Key key ) const	{ return !KeyIsPressedOrHeld( key ); }
	bool KeyIsPressed( Key key ) const			{ return ( isKeyDown[ key ] && !wasKeyDown[ key ] ); }
	bool KeyIsNotPressed( Key key ) const		{ return !KeyIsPressed( key ); }

	void SetKeyDown( Key key )			 { isKeyDown[ key ] = true;  }
	void SetKeyDown( unsigned char key ) { isKeyDown[ key ] = true;  }
	void SetKeyUp( unsigned char key );
	void SetKeyUp( Key key );

	void Update();
};







//-----------------------------------------------------------------------------------------------
inline void Keyboard::ClearInputQueue() 
{ 
	while ( !characterInput.empty() )
	{
		characterInput.pop();
	}
}

//-----------------------------------------------------------------------------------------------
inline unsigned char Keyboard::GetCharacterFromInputQueue()
{ 
	unsigned char frontChar = characterInput.front();
	characterInput.pop(); 
	return frontChar;
}

//-----------------------------------------------------------------------------------------------
inline void Keyboard::SetKeyUp( unsigned char key )
{
	isKeyDown[ key ] = false;
	wasKeyDown[ key ] = false;
}

//-----------------------------------------------------------------------------------------------
inline void Keyboard::SetKeyUp( Key key )
{
	isKeyDown[ key ] = false;
	wasKeyDown[ key ] = false;
}

//-----------------------------------------------------------------------------------------------
inline void Keyboard::Update()
{
	for( unsigned int key = 0; key < NUMBER_OF_KEYS; ++key )
	{
		if( isKeyDown[ key ] )
		{
			wasKeyDown[ key ] = true;
		}
	}
	ClearInputQueue();
}

#endif //INCLUDED_KEYBOARD_HPP
