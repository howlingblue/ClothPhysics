#define WIN32_LEAN_AND_MEAN
#define UNICODE 0
#include <windows.h>
#include "../resource.h"
#include <cassert>
#include <crtdbg.h>
#include <map>
#include "../Engine/Console/CommandConsole.hpp"
#include "../Engine/Graphics/Renderer.hpp"
#include "../Engine/Graphics/Texture.hpp"
#include "../Engine/Input/Keyboard.hpp"
#include "../Engine/Input/Mouse.hpp"
#include "../Engine/Input/Xbox.hpp"
#include "../Engine/Sound/Mixer.hpp"
#include "../Engine/Time.hpp"
#include "../Game/Sandbox.hpp"

//-----------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);

//-----------------------------------------------------------------------------------------------
bool g_isQuitting = false;
HWND g_hWnd = nullptr;
HDC g_displayDeviceContext = nullptr;
HGLRC g_openGLRenderingContext = nullptr;

const char*		 APP_NAME = "Shader and Particle Testing Ground";

//Game
Game*			 g_gameInstance;
bool			 gameActive = true;

//Graphics
const int		 WINDOW_OFFSET_VERT = 50;
const int		 WINDOW_OFFSET_HORZ = 50;
const int		 SCREEN_WIDTH = 1600;
const int		 SCREEN_HEIGHT = 900;

//Input
Keyboard		 g_keyboard;
Mouse			 g_mouse;
Xbox::Controller g_controller;

static const double LOCKED_FRAME_RATE_SECONDS = 1.0 / 60.0;

void SetRendererSettings( Renderer* renderer )
{
	renderer->EnableFeature( Renderer::COLOR_BLENDING );
	renderer->SetAlphaBlendingFunction( Renderer::SOURCE_ALPHA, Renderer::ONE_MINUS_SOURCE_ALPHA );
	renderer->EnableFeature( Renderer::DEPTH_TESTING );
	renderer->EnableFeature( Renderer::FACE_CULLING );

	renderer->SetColorBufferClearValue( 0.f, 0.f, 0.f, 1.f );
	renderer->SetDepthBufferClearValue( 1.f );
}

//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	static const float ONE_OVER_WHEEL_DELTA = 1.f / WHEEL_DELTA;
	unsigned char asKey = (unsigned char) wParam;
	float normalizedMouseWheelDelta;
	switch( wmMessageCode )
	{
		case WM_CLOSE:
		case WM_DESTROY:
		case WM_QUIT:
			g_isQuitting = true;
			return 0;

		case WM_ACTIVATE:
			if( wParam == WA_INACTIVE )
			{
				ShowCursor( true );
				gameActive = false;
			}
			else
			{
				ShowCursor( false );
				gameActive = true;
			}
			return 0;

		case WM_CHAR:
			switch( asKey )
			{
			case '`': //(Absorbed because it's the command console key)
			case '~': //(Absorbed because it's the command console key)
			case 0x0A: // Line Feed
			case VK_BACK:
			case VK_ESCAPE:
			case VK_RETURN:
			case VK_TAB:
				return 0; //Do nothing with the special characters (handle these with regular input)

			default:
				g_keyboard.AddCharacterToInputQueue( asKey );
				return 0;
			}

		case WM_KEYDOWN:
			switch( asKey )
			{
			case VK_ESCAPE:
			default:
				g_keyboard.SetKeyDown( asKey );
				return 0;
			}

			break;

		case WM_KEYUP:
			switch( asKey )
			{
			case VK_LBUTTON:
			case VK_RBUTTON:
			case VK_MBUTTON:
			case VK_XBUTTON1:
			case VK_XBUTTON2:
				g_mouse.SetButtonUp( asKey );
				return 0;
			default:
				g_keyboard.SetKeyUp( asKey );
				return 0;
			}
			break;
		case WM_LBUTTONDOWN:
			g_mouse.SetButtonDown( VK_LBUTTON );
			break;
		case WM_LBUTTONUP:
			g_mouse.SetButtonUp( VK_LBUTTON );
			break;
		case WM_RBUTTONDOWN:
			g_mouse.SetButtonDown( VK_RBUTTON );
			break;
		case WM_RBUTTONUP:
			g_mouse.SetButtonUp( VK_RBUTTON );
			break;
		case WM_MOUSEWHEEL:
			normalizedMouseWheelDelta = static_cast< float >( GET_WHEEL_DELTA_WPARAM( wParam ) ) * ONE_OVER_WHEEL_DELTA;
			g_mouse.SetWheelDelta( normalizedMouseWheelDelta );
			break;
	}

	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}

void HandleMouseInput()
{
	static const int SCREEN_CENTER_X = static_cast< int >( SCREEN_WIDTH * 0.5f );
	static const int SCREEN_CENTER_Y = static_cast< int >( SCREEN_HEIGHT * 0.5f );

	POINT currentMouseLocation;
	if( !GetCursorPos( &currentMouseLocation ) )
		return;

	g_mouse.SetMouseMovement( static_cast< float >( currentMouseLocation.x - SCREEN_CENTER_X ),
							  static_cast< float >( currentMouseLocation.y - SCREEN_CENTER_Y ) );

	SetCursorPos( SCREEN_CENTER_X, SCREEN_CENTER_Y );
}

//-----------------------------------------------------------------------------------------------
void CreateOpenGLWindow( HINSTANCE applicationInstanceHandle )
{
	// Define a window class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Assign a win32 message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = LoadIcon( applicationInstanceHandle, MAKEINTRESOURCE( IDI_ICON1 ) );
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );

	RECT windowRect = { WINDOW_OFFSET_HORZ, WINDOW_OFFSET_VERT, WINDOW_OFFSET_HORZ + SCREEN_WIDTH, WINDOW_OFFSET_VERT + SCREEN_HEIGHT };
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, APP_NAME, -1, windowTitle, sizeof(windowTitle)/sizeof(windowTitle[0]) );
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL );

	ShowWindow( g_hWnd, SW_SHOW );
	SetForegroundWindow( g_hWnd );
	SetFocus( g_hWnd );

	g_displayDeviceContext = GetDC( g_hWnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
	pixelFormatDescriptor.nSize			= sizeof( pixelFormatDescriptor );
	pixelFormatDescriptor.nVersion		= 1;
	pixelFormatDescriptor.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType	= PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits	= 24;
	pixelFormatDescriptor.cDepthBits	= 24;
	pixelFormatDescriptor.cAccumBits	= 0;
	pixelFormatDescriptor.cStencilBits	= 8;

	int pixelFormatCode = ChoosePixelFormat( g_displayDeviceContext, &pixelFormatDescriptor );
	SetPixelFormat( g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
	g_openGLRenderingContext = wglCreateContext( g_displayDeviceContext );
	wglMakeCurrent( g_displayDeviceContext, g_openGLRenderingContext );
}

//-----------------------------------------------------------------------------------------------
void RunMessagePump()
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage );
	}
}

//-----------------------------------------------------------------------------------------------
void Update( double timeSpentLastFrameSeconds )
{
	if( !gameActive )
		return;
	float deltaSeconds = static_cast< float >( timeSpentLastFrameSeconds );

	HandleMouseInput();

	g_gameInstance->Update( deltaSeconds, g_keyboard, g_mouse, g_controller );
	g_keyboard.Update();
	g_mouse.Update();
	g_controller.Update( deltaSeconds );
	Mixer::GetMixer()->Update();
}

//-----------------------------------------------------------------------------------------------
void Render()
{
	Renderer* renderer = Renderer::GetRenderer();

	renderer->ClearColorBuffer();
	renderer->ClearDepthBuffer();

	g_gameInstance->Render();

	SwapBuffers( g_displayDeviceContext );
}

//-----------------------------------------------------------------------------------------------
double WaitUntilNextFrameThenGiveFrameTime()
{
	static double targetTime = 0.0;
	double timeNow = GetCurrentTimeSeconds();

	while( timeNow < targetTime )
	{
		timeNow = GetCurrentTimeSeconds();
	}
	targetTime = timeNow + LOCKED_FRAME_RATE_SECONDS;

	return LOCKED_FRAME_RATE_SECONDS;
}

//-----------------------------------------------------------------------------------------------
void RunFrame()
{
	static double timeSpentLastFrameSeconds = 0.0;
	RunMessagePump();
	Update( LOCKED_FRAME_RATE_SECONDS );
	Render();
	timeSpentLastFrameSeconds = WaitUntilNextFrameThenGiveFrameTime();
}

void QuitGame( const CommandConsole::CommandArguments& )
{
	g_gameInstance->Quit();
}

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );
	InitializeTimer();

	g_gameInstance = new Sandbox( g_isQuitting, SCREEN_WIDTH, SCREEN_HEIGHT, 70.f );

	CreateOpenGLWindow( applicationInstanceHandle );
	Renderer::CreateRenderer();
	SetRendererSettings( Renderer::GetRenderer() );
	Mixer::CreateMixer();

	SetCursorPos( static_cast< int >( SCREEN_WIDTH * 0.5f ), static_cast< int >( SCREEN_HEIGHT * 0.5f ) );

	g_gameInstance->Initialize();
	CommandConsole::RegisterConsoleCommand( "quit", QuitGame );

	while( !g_isQuitting )	
	{
		RunFrame();
	}
	Texture::CleanUpTextureRepository();
	delete g_gameInstance;
	

#if defined( _WIN32 ) && defined( _DEBUG )
	assert( _CrtCheckMemory() );
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
