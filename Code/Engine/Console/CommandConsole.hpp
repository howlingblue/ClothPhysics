#ifndef INCLUDED_COMMAND_CONSOLE_HPP
#define INCLUDED_COMMAND_CONSOLE_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include <functional>
#include <string>
#include <vector>
#include "../Font/BitmapFont.hpp"
#include "../Math/FloatVector2.hpp"
#include "../Color.hpp"

//-----------------------------------------------------------------------------------------------
STATIC class CommandConsole
{
	#pragma region Nested Class Declarations and Definitions
public:
	//-----------------------------------------------------------------------------------------------
	struct CommandArguments;

private:
	//-----------------------------------------------------------------------------------------------
	typedef std::function< void( const CommandArguments& ) > ConsoleCommand;
	struct PaneColorData;

	//-----------------------------------------------------------------------------------------------
	struct TextLine
	{
		std::string text;
		Color	textColor;
		Color	backgroundColor;
		Color	shadowColor;

		TextLine( const std::string& line, const Color& lineColor, const Color& backColor = Color( 0.f, 0.f, 0.f, 0.f ), 
			const Color& dropShadowColor = Color( 0.f, 0.f, 0.f, 0.f ) )
			: text( line )
			, textColor( lineColor )
			, backgroundColor( backColor )
			, shadowColor( dropShadowColor )
		{ }
	};
	
	//-----------------------------------------------------------------------------------------------
	struct Log
	{
		Log( const FloatVector2& logLowerLeftCorner, const FloatVector2& logUpperRightCorner )
			: lowerLeftCorner( logLowerLeftCorner )
			, upperRightCorner( logUpperRightCorner )
		{ }

		void Clear() { previousOutput.clear(); }
		void StoreLine( TextLine line ) { previousOutput.push_back( line ); }

		//Data Members
		FloatVector2 lowerLeftCorner, upperRightCorner;
		std::vector< TextLine > previousOutput;
	};

	//-----------------------------------------------------------------------------------------------
	struct Prompt
	{
		Prompt( const FloatVector2& promptLowerLeftCorner, const FloatVector2& promptUpperRightCorner )
			: lowerLeftCorner( promptLowerLeftCorner )
			, upperRightCorner( promptUpperRightCorner )
			, cursorIsShowing( true )
			, cursorLocation( 0 )
			, cursorVertexBufferID( 0 )
			, secondsSinceCursorBlinkChange( 0.f )
		{ }
		
		FloatVector2 lowerLeftCorner, upperRightCorner;
		std::string currentInput;

		static const float CURSOR_BLINK_RATE_SECONDS;
		bool cursorIsShowing;
		unsigned int cursorLocation;
		unsigned int cursorVertexBufferID;
		float secondsSinceCursorBlinkChange;
	};
	#pragma endregion

public:
	static void CreateConsole( const FloatVector2& logLowerLeftCorner, const FloatVector2& logUpperRightCorner, 
								const FloatVector2 promptLowerLeftCorner, const FloatVector2& promptUpperRightCorner );

	static CommandConsole* GetConsole();

	static void RegisterConsoleCommand( std::string commandString, ConsoleCommand command );

	void AddCharacterToPrompt( unsigned char character );
	void ClearLog( ) { m_log.Clear(); }
	void ClearPrompt();
	void ExecuteCommandInPrompt();
	void MovePromptCursorLeft();
	void MovePromptCursorRight();
	void MovePromptCursorToStart()	{ m_prompt.cursorLocation = 0; }
	void MovePromptCursorToEnd()	{ m_prompt.cursorLocation = m_prompt.currentInput.length(); }
	bool PromptIsEmpty() const { return m_prompt.currentInput.empty(); }
	void RemoveLetterLeftOfCursorPosition();
	void RemoveLetterRightOfCursorPosition();

	void WriteTextToLog( const std::string& text, const Color& textColor, const Color& backgroundColor = Color( 0.f, 0.f, 0.f, 0.f ), 
							const Color& textShadowColor = Color( 0.f, 0.f, 0.f, 0.f ) );

	void Render() const;
	void Update( float deltaSeconds );

private:
	static const Color BACKGROUND_PANE_COLOR;
	static const Color BORDER_COLOR;
	static const float BORDER_THICKNESS;
	static const float TEXT_LINE_HEIGHT;

	static CommandConsole* s_console;
	static std::map< std::string, ConsoleCommand > s_commandRegistry;

	CommandConsole( const FloatVector2& logLowerLeftCorner, const FloatVector2& logUpperRightCorner, 
					const FloatVector2 promptLowerLeftCorner, const FloatVector2& promptUpperRightCorner );
	void Initialize();

	void GenerateConsolePaneVBO();
	void GenerateTextCursorVBO();
	void RenderBackgroundPanes() const;
	void RenderBorders() const;
	void RenderConsoleText() const;
	void RenderTextCursor() const;
	void SplitCommandIntoCommandAndArguments( const std::string& commandLine, std::string& out_command, CommandArguments& out_arguments ) const;

	BitmapFont m_font;
	Log m_log;
	Prompt m_prompt;

	unsigned int m_borderIndexBufferID;
	unsigned int m_paneIndexBufferID;
	unsigned int m_vertexBufferID;
};








//-----------------------------------------------------------------------------------------------
STATIC inline void CommandConsole::CreateConsole( const FloatVector2& logLowerLeftCorner, const FloatVector2& logUpperRightCorner, 
													const FloatVector2 promptLowerLeftCorner, const FloatVector2& promptUpperRightCorner )
{
	s_console = new CommandConsole( logLowerLeftCorner, logUpperRightCorner, promptLowerLeftCorner, promptUpperRightCorner );
	s_console->Initialize();
}

//-----------------------------------------------------------------------------------------------
STATIC inline CommandConsole* CommandConsole::GetConsole()
{
	if( s_console == nullptr )
		exit( -35 );
	return s_console;
}

//-----------------------------------------------------------------------------------------------
inline CommandConsole::CommandConsole( const FloatVector2& logLowerLeftCorner, const FloatVector2& logUpperRightCorner, 
										const FloatVector2 promptLowerLeftCorner, const FloatVector2& promptUpperRightCorner )
	: m_log( logLowerLeftCorner, logUpperRightCorner )
	, m_prompt( promptLowerLeftCorner, promptUpperRightCorner )
	, m_borderIndexBufferID( 0 )
	, m_paneIndexBufferID( 0 )
	, m_vertexBufferID( 0 )
	, m_font()
{ }

//-----------------------------------------------------------------------------------------------
inline void CommandConsole::Initialize()
{
	GenerateConsolePaneVBO();
	GenerateTextCursorVBO();

	static const std::string fontDefinitionLocation = "Data/Font/MainFont_EN.FontDef.xml";
	static const std::string fontImageLocation = "Data/Font/MainFont_EN_00.png";
	m_font = BitmapFont( fontDefinitionLocation, &fontImageLocation, 1 );
}


//-----------------------------------------------------------------------------------------------
STATIC inline void CommandConsole::RegisterConsoleCommand( std::string commandString, ConsoleCommand command ) 
{ 
	//FIX? Should a command be able to overwrite others with no consequence or flag?
	s_commandRegistry[ commandString ] = command; 
}

//-----------------------------------------------------------------------------------------------
inline void CommandConsole::AddCharacterToPrompt( unsigned char character ) 
{
	m_prompt.currentInput.insert( m_prompt.currentInput.begin() + m_prompt.cursorLocation, character );
	++m_prompt.cursorLocation; //Move the cursor after the entered character
}

//-----------------------------------------------------------------------------------------------
inline void CommandConsole::ClearPrompt() 
{ 
	m_prompt.currentInput.clear(); 
	m_prompt.cursorLocation = 0;
}

//-----------------------------------------------------------------------------------------------
inline void CommandConsole::MovePromptCursorLeft()
{
	if( m_prompt.cursorLocation > 0 )
		--m_prompt.cursorLocation;
}

//-----------------------------------------------------------------------------------------------
inline void CommandConsole::MovePromptCursorRight()
{
	if( m_prompt.cursorLocation < m_prompt.currentInput.length() )
		++m_prompt.cursorLocation;
}

//-----------------------------------------------------------------------------------------------
inline void CommandConsole::RemoveLetterLeftOfCursorPosition()
{
	//Can't delete left of the start or crash
	if( m_prompt.cursorLocation == 0 )
		return;

	//cursor location reflects the character to the right, so character to delete is minus one
	m_prompt.currentInput.erase( m_prompt.currentInput.begin() + m_prompt.cursorLocation - 1 );

	--m_prompt.cursorLocation; //Move the cursor location left to follow the void left by deletion
}

//-----------------------------------------------------------------------------------------------
inline void CommandConsole::RemoveLetterRightOfCursorPosition()
{
	//Can't delete right of the end or crash
	if( m_prompt.cursorLocation == m_prompt.currentInput.length() )
		return;

	m_prompt.currentInput.erase( m_prompt.currentInput.begin() + m_prompt.cursorLocation );
}

//-----------------------------------------------------------------------------------------------
inline void CommandConsole::WriteTextToLog( const std::string& text, const Color& textColor,
											const Color& backgroundColor, const Color& textShadowColor )
{
	m_log.StoreLine( TextLine( text, textColor, backgroundColor, textShadowColor ) );
}

#endif //INCLUDED_COMMAND_CONSOLE_HPP
