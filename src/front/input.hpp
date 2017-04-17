#pragma once
#include <SDL2/SDL.h>
#include "base.hpp"

namespace front {

	namespace input{

		// blocks needed to define input event

		using EventType = uint16_t;
		using Key = SDL_Keycode; 
		using Mod = uint32_t;
				
		struct Keyboard;
		struct Mouse;
		
		
		
		
		EventType const
			EventNone = SDL_FIRSTEVENT,
		
			// application
			EventQuit = SDL_QUIT,
			
			// window
			EventWindowEvent = SDL_WINDOWEVENT,
			EventSysWMEvent = SDL_SYSWMEVENT,
			
			// keyboard
			EventKeyDown = SDL_KEYDOWN,
			EventKeyUp = SDL_KEYUP,
			EventTextEditing = SDL_TEXTEDITING,
			EventTextInput = SDL_TEXTINPUT,
			
			// mouse
			EventMouseMotion = SDL_MOUSEMOTION,
			EventMouseButtonDown = SDL_MOUSEBUTTONDOWN,
			EventMouseButtonUp = SDL_MOUSEBUTTONUP,
			EventMouseWheel = SDL_MOUSEWHEEL,
			
			// joystick
			EventJoyAxisMotion = SDL_JOYAXISMOTION,
			EventJoyBallMotion = SDL_JOYBALLMOTION,
			EventJoyHatMotion = SDL_JOYHATMOTION,
			EventJoyButtonDown = SDL_JOYBUTTONDOWN,
			EventJoyButtonUp = SDL_JOYBUTTONUP,
			EventJoyDeviceAdded = SDL_JOYDEVICEADDED,
			EventJoyDeviceRemoved = SDL_JOYDEVICEREMOVED,
			
			// game controller
			EventControllerAxisMotion = SDL_CONTROLLERAXISMOTION,
			EventControllerButtonDown = SDL_CONTROLLERBUTTONDOWN,
			EventControllerButtonUp = SDL_CONTROLLERBUTTONUP,
			EventControllerDeviceAdded = SDL_CONTROLLERDEVICEADDED,
			EventControllerDeviceRemoved = SDL_CONTROLLERDEVICEREMOVED,
			EventControllerDeviceMapped = SDL_CONTROLLERDEVICEREMAPPED,
			
			// touch 
			EventFingerDown = SDL_FINGERDOWN,
			EventFingerUp = SDL_FINGERUP,
			EventFingerMotion = SDL_FINGERMOTION,
			
			// gesture
			EventDollarGesture = SDL_DOLLARGESTURE,
			EventDollarRecord = SDL_DOLLARRECORD,
			EventMultiGesture = SDL_MULTIGESTURE,

			// clipboard
			EventClipboardUpdate = SDL_CLIPBOARDUPDATE,

			// file drop
			EventDropFile = SDL_DROPFILE,

			// audio hotplug
			EventAudioDeviceAdded = SDL_AUDIODEVICEADDED,
			EventAudioDeviceRemoved = SDL_AUDIODEVICEREMOVED,

			// render
		//	EventRenderTargetsReset = SDL_RENDER_TARGETS_RESET.
			EventRenderDeviceReset = SDL_RENDER_DEVICE_RESET, 

			// user
			EventPress = SDL_USEREVENT + 0,
			EventRelease = SDL_USEREVENT + 1,
			EventMotion = SDL_USEREVENT + 2,
			EventChar = SDL_USEREVENT + 3,
			
			EventUser = SDL_USEREVENT + 4;
			
		//	EventEnd = SDL_USEREVENT + SDL_NUMEVENTS;
			

		
		
	  
			
		using WindowEventType = SDL_WindowEventID;

		WindowEventType const		
			WindowEventShown = SDL_WINDOWEVENT_SHOWN,
			WindowEventHidden = SDL_WINDOWEVENT_HIDDEN,
			WindowEventExposed = SDL_WINDOWEVENT_EXPOSED,
			WindowEventMoved = SDL_WINDOWEVENT_MOVED,
			WindowEventResized = SDL_WINDOWEVENT_RESIZED,
			WindowEventSizeChanged = SDL_WINDOWEVENT_SIZE_CHANGED,
			WindowEventMinimized = SDL_WINDOWEVENT_MINIMIZED,
			WindowEventMaximized = SDL_WINDOWEVENT_MAXIMIZED,
			WindowEventRestored = SDL_WINDOWEVENT_RESTORED,
			WindowEventEnter = SDL_WINDOWEVENT_ENTER,
			WindowEventLeave = SDL_WINDOWEVENT_LEAVE,
			WindowEventFocusGained = SDL_WINDOWEVENT_FOCUS_GAINED,
			WindowEventFocusLost = SDL_WINDOWEVENT_FOCUS_LOST,
			WindowEventClose = SDL_WINDOWEVENT_CLOSE
		;
			
			
			

		


		Key const
			KeyNone = SDLK_UNKNOWN,
			KeyLMB = SDL_BUTTON_LEFT,
			KeyMMB = SDL_BUTTON_MIDDLE,
			KeyRMB = SDL_BUTTON_RIGHT,			
			KeyNum0 = SDLK_0,
			KeyNum1 = SDLK_1,
			KeyNum2 = SDLK_2,
			KeyNum3 = SDLK_3,
			KeyNum4 = SDLK_4,
			KeyNum5 = SDLK_5,
			KeyNum6 = SDLK_6,
			KeyNum7 = SDLK_7,
			KeyNum8 = SDLK_8,
			KeyNum9 = SDLK_9,
			KeyA = SDLK_a,
			KeyQuote = SDLK_QUOTE,
			KeyB = SDLK_b,
			KeyBackslash = SDLK_BACKSLASH,
			KeyBackspace = SDLK_BACKSPACE,
			KeyC = SDLK_c,
			KeyCapsLock = SDLK_CAPSLOCK,
			KeyComma = SDLK_COMMA,
			KeyD = SDLK_d,
			KeyDelete = SDLK_DELETE,
			KeyDown = SDLK_DOWN,
			KeyE = SDLK_e,
			KeyEnd = SDLK_END,
			KeyEquals = SDLK_EQUALS,
			KeyEsc = SDLK_ESCAPE,
			KeyF = SDLK_f,
			KeyF1 = SDLK_F1,
			KeyF10 = SDLK_F10,
			KeyF11 = SDLK_F11,
			KeyF12 = SDLK_F12,
			KeyF13 = SDLK_F13,
			KeyF14 = SDLK_F14,
			KeyF15 = SDLK_F15,
			KeyF16 = SDLK_F16,
			KeyF17 = SDLK_F17,
			KeyF18 = SDLK_F18,
			KeyF19 = SDLK_F19,
			KeyF2 = SDLK_F2,
			KeyF20 = SDLK_F20,
			KeyF21 = SDLK_F21,
			KeyF22 = SDLK_F22,
			KeyF23 = SDLK_F23,
			KeyF24 = SDLK_F24,
			KeyF3 = SDLK_F3,
			KeyF4 = SDLK_F4,
			KeyF5 = SDLK_F5,
			KeyF6 = SDLK_F6,
			KeyF7 = SDLK_F7,
			KeyF8 = SDLK_F8,
			KeyF9 = SDLK_F9,
			KeyFIND = SDLK_FIND,
			KeyG = SDLK_g,
			KeyBackquote = SDLK_BACKQUOTE,
			KeyH = SDLK_h,
			KeyHome = SDLK_HOME,
			KeyI = SDLK_i,
			KeyInsert = SDLK_INSERT,
			KeyJ = SDLK_j,
			KeyK = SDLK_k,
			KeyNumpad0 = SDLK_KP_0,
			KeyNumpad00 = SDLK_KP_00,
			KeyNumpad000 = SDLK_KP_000,
			KeyNumpad1 = SDLK_KP_1,
			KeyNumpad2 = SDLK_KP_2,
			KeyNumpad3 = SDLK_KP_3,
			KeyNumpad4 = SDLK_KP_4,
			KeyNumpad5 = SDLK_KP_5,
			KeyNumpad6 = SDLK_KP_6,
			KeyNumpad7 = SDLK_KP_7,
			KeyNumpad8 = SDLK_KP_8,
			KeyNumpad9 = SDLK_KP_9,
			KeyNumpadA = SDLK_KP_A,
			KeyNumpadAmpersand = SDLK_KP_AMPERSAND,
			KeyNumpadAt = SDLK_KP_AT,
			KeyNumpadBackspace = SDLK_KP_BACKSPACE,
			KeyNumpadBinary = SDLK_KP_BINARY,
			KeyNumpadColon = SDLK_KP_COLON,
			KeyNumpadComma = SDLK_KP_COMMA,
			KeyNumpadDecimal = SDLK_KP_DECIMAL,
			KeyNumpadDivide = SDLK_KP_DIVIDE,
			KeyNumpadEnter = SDLK_KP_ENTER,
			KeyNumpadMinus = SDLK_KP_MINUS,
			KeyNumpadMultiply = SDLK_KP_MULTIPLY,
			KeyNumpadPeriod = SDLK_KP_PERIOD,
			KeyNumpadPlus = SDLK_KP_PLUS,
			KeyNumpadVerticalBar = SDLK_KP_VERTICALBAR,
			KeyNumpadXor = SDLK_KP_XOR,
			KeyL = SDLK_l,
			KeyLAlt = SDLK_LALT,
			KeyLCtrl = SDLK_LCTRL,
			KeyLeft = SDLK_LEFT,
			KeyLEFTBRACKET = SDLK_LEFTBRACKET,
			KeyLGUI = SDLK_LGUI,
			KeyLShift = SDLK_LSHIFT,
			KeyM = SDLK_m,
			KeyMenu = SDLK_MENU,
			KeyMinus = SDLK_MINUS,
			KeyMode = SDLK_MODE,
			KeyMute = SDLK_MUTE,
			KeyN = SDLK_n,
			KeyNumLockClear = SDLK_NUMLOCKCLEAR,
			KeyO = SDLK_o,
			KeyOper = SDLK_OPER,
			KeyOut = SDLK_OUT,
			KeyP = SDLK_p,
			KeyPageDown = SDLK_PAGEDOWN,
			KeyPageUp = SDLK_PAGEUP,
			KeyPaste = SDLK_PASTE,
			KeyPause = SDLK_PAUSE,
			KeyPeriod = SDLK_PERIOD,
			KeyPower = SDLK_POWER,
			KeyPrintScreen = SDLK_PRINTSCREEN,
			KeyPrior = SDLK_PRIOR,
			KeyQ = SDLK_q,
			KeyR = SDLK_r,
			KeyRAlt = SDLK_RALT,
			KeyRCtrl = SDLK_RCTRL,
			KeyEnter = SDLK_RETURN,
			KeyRETURN2 = SDLK_RETURN2,
			KeyRGui = SDLK_RGUI,
			KeyRight = SDLK_RIGHT,
			KeyRightBracket = SDLK_RIGHTBRACKET,
			KeyRShift = SDLK_RSHIFT,
			KeyS = SDLK_s,
			KeyScrollLock = SDLK_SCROLLLOCK,
			KeySelect = SDLK_SELECT,
			KeySemiColon = SDLK_SEMICOLON,
			KeySeparator = SDLK_SEPARATOR,
			KeySlash = SDLK_SLASH,
			KeySleep = SDLK_SLEEP,
			KeySpace = SDLK_SPACE,
			KeyStop = SDLK_STOP,
			KeySysReq = SDLK_SYSREQ,
			KeyT = SDLK_t,
			KeyTab = SDLK_TAB,
			KeyU = SDLK_u,
			KeyUndo = SDLK_UNDO,
			KeyUp = SDLK_UP,
			KeyV = SDLK_v,
			KeyVolumeDown = SDLK_VOLUMEDOWN,
			KeyVolumeUp = SDLK_VOLUMEUP,
			KeyW = SDLK_w,
			KeyWWW = SDLK_WWW,
			KeyX = SDLK_x,
			KeyY = SDLK_y,
			KeyZ = SDLK_z;
			
		
		
		using SDLButton = decltype(SDL_BUTTON_LEFT);
		using SDLButtonMask = decltype(SDL_BUTTON_LMASK);
		using SDLScancode = SDL_Scancode;
		using SDLKeycode = SDL_Keycode;
		using SDLKeymod = SDL_Keymod;
		

		constexpr
		SDLButton key_to_SDLButton(Key key) {
			return key;
		}
		
		constexpr
		Key SDLButton_to_key(SDLButton button) {
			return button;
		}

		constexpr
		Mod SDLButtonMask_to_mod(SDLButtonMask mask) {
			return Mod(mask) << 26;
		}

		constexpr
		SDLButtonMask mod_to_SDLButtonMask(Mod mod) {
			return SDLButtonMask(mod >> 26);
		};			

		constexpr
		SDLScancode key_to_SDLScancode(Key key) {
			assert(SDLK_SCANCODE_MASK & key);
			return static_cast<SDLScancode>(~SDLK_SCANCODE_MASK & key);
		}
	   
	    constexpr
		Key SDLKeycode_to_key(SDLKeycode x) {
			return x;
		}
	   

	   
		
		
		Mod const
			ModNone = KMOD_NONE,
			ModLShift = KMOD_LSHIFT,
			ModRShift = KMOD_RSHIFT,
			ModLCtrl = KMOD_LCTRL,
			ModRCtrl = KMOD_RCTRL,
			ModLAlt = KMOD_LALT,
			ModRAlt = KMOD_RALT,
			ModLGui = KMOD_LGUI,
			ModRGui = KMOD_RGUI,
			ModNum = KMOD_NUM,
			ModCaps = KMOD_CAPS,
			ModMode = KMOD_MODE,
			ModGui = KMOD_GUI,		
			ModLMB = SDLButtonMask_to_mod(SDL_BUTTON_LMASK),
			ModMMB = SDLButtonMask_to_mod(SDL_BUTTON_MMASK),
			ModRMB = SDLButtonMask_to_mod(SDL_BUTTON_RMASK)
			;

		
		// TODO:
		
		constexpr
		Mod SDLKeymod_to_mod(uint16_t x) {
			return x;
		}
		
		// mod_to_SDLKeymod		
		// key_to_mod
		// mod_to_key
		
		
		/*
		inline char const* get_mod_name(Mod x) {
			switch (x) {
				case ModNone: return "None";
				case ModAlt: return "Alt";
				case ModCtrl: return "Ctrl";
				case ModShift: return "Shift";
				case ModButtonLeft: return "ButtomLeft";
				case ModButtonRight: return "ButtomRight";
			}
			return "<Invalid>";
		}*/

	   
		
		struct Keyboard{
		
			int N;
			uint8_t const* state{nullptr}; 
			
			bool operator[](Key const key) const {			
				auto ind = key_to_SDLScancode(key);
				assert(ind < N);
				return state[ind];
			}
			
			Mod get_mod() const {
				return SDL_GetModState();
			}
			
			void refresh() {
			
			}
			
			Keyboard & init() {
				state = SDL_GetKeyboardState(&N);
				return *this;
			}
		};
		
		
		struct Mouse{
			v2i pos;
			SDLButtonMask button;
			
			bool operator[](Key const key) const {			
				return button & SDL_BUTTON(key_to_SDLButton(key));
			}
			
			Mod get_mod() const {
				return SDLButtonMask_to_mod(button);
			}
			
			v2s get_pos() const {
				return v2s(pos);    // hmm, narrowing!
			}
			
			void refresh() {
				button = SDL_GetMouseState(&pos[0], &pos[1]);
			}
			
			Mouse & init() {
				refresh();
				return *this;
			}
		};

		
		void print_window_event(SDL_Event const& event);
		char const* get_key_name(Key x);
		char const* get_eventtype_name(EventType x);
		char const* get_mod_name(Mod x);

		void print_mod_table();

	} // input
} // front
