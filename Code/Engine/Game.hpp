#ifndef INCLUDED_GAME_HPP
#define INCLUDED_GAME_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "Input/Keyboard.hpp"
#include "Input/Mouse.hpp"
#include "Input/Xbox.hpp"
#include "Console/CommandConsole.hpp"

//-----------------------------------------------------------------------------------------------
class Game
{
protected:
	bool&		 m_quitVariable;
	unsigned int m_screenWidth;
	unsigned int m_screenHeight;
	double		 m_aspectRatio;
	double		 m_horizontalFOVDegrees;

	virtual void RenderGame() const = 0;
	virtual void RenderUI() const = 0;

	virtual void GameUpdate( float deltaSeconds ) = 0;
	virtual void InputUpdate(  float deltaSeconds, Keyboard& keyInput, const Mouse& mouseInput, const Xbox::Controller& xboxInput ) = 0;


private:
	CommandConsole*	m_console;
	bool			m_consoleVisible;

	//We have no need of a pithy assignment or copy operator!
	Game( const Game& other );
	Game& operator=( const Game& other );

	void UpdateConsoleInput( float deltaSeconds, Keyboard& keyInput, const Mouse& mouseInput );

public:
	Game( bool& quitVariable, unsigned int screenWidth, unsigned int screenHeight, float horizontalFOVDegrees );

	virtual void Initialize();

	void Quit() { m_quitVariable = true; }
	void Render() const;
	void Update( float deltaSeconds, Keyboard& keyInput, const Mouse& mouseInput, const Xbox::Controller& xboxInput );
};

#endif //INCLUDED_GAME_HPP
