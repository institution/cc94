#include "backend.h"


namespace backend{

	
	void Back::set_logical_dim(v2i const& dim) {
		
		target = boost::make_unique<Texture>(SDL_CreateTexture(ren,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET,
			dim[0], dim[1]
		));

		if (SDL_SetRenderTarget(ren, target->sdl_tex())) {
			error_sdl();
		}
			
		clear();
	}
	
	void Back::flip() {

		//SDL_RenderPresent(ren);
		if (target) {
			
			// texture target size
			auto log = this->get_logical_dim();
			auto log_rect = make_sdl_rect({0,0}, log);
						
			// switch to default target
			SDL_SetRenderTarget(ren, nullptr);
			phys_dim = get_output_dim();
			
			// default target size
			auto out = phys_dim;
			auto out_rect = make_sdl_rect({0,0}, out);
			
			// set viewport
			SDL_RenderSetViewport(ren, &out_rect);
			
			
			SDL_RenderCopy(ren, target->sdl_tex(), 
				&log_rect, &out_rect
			);
			SDL_RenderPresent(ren);
			
			// switch back to texture target
			SDL_SetRenderTarget(ren, target->sdl_tex());
		}
		else {
			SDL_RenderPresent(ren);
		}
		

		
	}
	

	Keycode const
		Key::Num0 = SDLK_0,
		Key::Num1 = SDLK_1,
		Key::Num2 = SDLK_2,
		Key::Num3 = SDLK_3,
		Key::Num4 = SDLK_4,
		Key::Num5 = SDLK_5,
		Key::Num6 = SDLK_6,
		Key::Num7 = SDLK_7,
		Key::Num8 = SDLK_8,
		Key::Num9 = SDLK_9,
		Key::A = SDLK_a,
		Key::Quote = SDLK_QUOTE,
		Key::B = SDLK_b,
		Key::Backslash = SDLK_BACKSLASH,
		Key::Backspace = SDLK_BACKSPACE,
		Key::C = SDLK_c,
		Key::CapsLock = SDLK_CAPSLOCK,
		Key::Comma = SDLK_COMMA,
		Key::D = SDLK_d,
		Key::Delete = SDLK_DELETE,
		Key::Down = SDLK_DOWN,
		Key::E = SDLK_e,
		Key::End = SDLK_END,
		Key::Equals = SDLK_EQUALS,
		Key::Esc = SDLK_ESCAPE,
		Key::F = SDLK_f,
		Key::F1 = SDLK_F1,
		Key::F10 = SDLK_F10,
		Key::F11 = SDLK_F11,
		Key::F12 = SDLK_F12,
		Key::F13 = SDLK_F13,
		Key::F14 = SDLK_F14,
		Key::F15 = SDLK_F15,
		Key::F16 = SDLK_F16,
		Key::F17 = SDLK_F17,
		Key::F18 = SDLK_F18,
		Key::F19 = SDLK_F19,
		Key::F2 = SDLK_F2,
		Key::F20 = SDLK_F20,
		Key::F21 = SDLK_F21,
		Key::F22 = SDLK_F22,
		Key::F23 = SDLK_F23,
		Key::F24 = SDLK_F24,
		Key::F3 = SDLK_F3,
		Key::F4 = SDLK_F4,
		Key::F5 = SDLK_F5,
		Key::F6 = SDLK_F6,
		Key::F7 = SDLK_F7,
		Key::F8 = SDLK_F8,
		Key::F9 = SDLK_F9,
		Key::FIND = SDLK_FIND,
		Key::G = SDLK_g,
		Key::Backquote = SDLK_BACKQUOTE,
		Key::H = SDLK_h,
		Key::Home = SDLK_HOME,
		Key::I = SDLK_i,
		Key::Insert = SDLK_INSERT,
		Key::J = SDLK_j,
		Key::K = SDLK_k,
		Key::Numpad0 = SDLK_KP_0,
		Key::Numpad00 = SDLK_KP_00,
		Key::Numpad000 = SDLK_KP_000,
		Key::Numpad1 = SDLK_KP_1,
		Key::Numpad2 = SDLK_KP_2,
		Key::Numpad3 = SDLK_KP_3,
		Key::Numpad4 = SDLK_KP_4,
		Key::Numpad5 = SDLK_KP_5,
		Key::Numpad6 = SDLK_KP_6,
		Key::Numpad7 = SDLK_KP_7,
		Key::Numpad8 = SDLK_KP_8,
		Key::Numpad9 = SDLK_KP_9,
		Key::NumpadA = SDLK_KP_A,
		Key::NumpadAmpersand = SDLK_KP_AMPERSAND,
		Key::NumpadAt = SDLK_KP_AT,
		Key::NumpadBackspace = SDLK_KP_BACKSPACE,
		Key::NumpadBinary = SDLK_KP_BINARY,
		Key::NumpadColon = SDLK_KP_COLON,
		Key::NumpadComma = SDLK_KP_COMMA,
		Key::NumpadDecimal = SDLK_KP_DECIMAL,
		Key::NumpadDivide = SDLK_KP_DIVIDE,
		Key::NumpadEnter = SDLK_KP_ENTER,
		Key::NumpadMinus = SDLK_KP_MINUS,
		Key::NumpadMultiply = SDLK_KP_MULTIPLY,
		Key::NumpadPeriod = SDLK_KP_PERIOD,
		Key::NumpadPlus = SDLK_KP_PLUS,
		Key::NumpadVerticalBar = SDLK_KP_VERTICALBAR,
		Key::NumpadXor = SDLK_KP_XOR,
		Key::L = SDLK_l,
		Key::LAlt = SDLK_LALT,
		Key::LCtrl = SDLK_LCTRL,
		Key::Left = SDLK_LEFT,
		Key::LEFTBRACKET = SDLK_LEFTBRACKET,
		Key::LGUI = SDLK_LGUI,
		Key::LShift = SDLK_LSHIFT,
		Key::M = SDLK_m,
		Key::Menu = SDLK_MENU,
		Key::Minus = SDLK_MINUS,
		Key::Mode = SDLK_MODE,
		Key::Mute = SDLK_MUTE,
		Key::N = SDLK_n,
		Key::NumLockClear = SDLK_NUMLOCKCLEAR,
		Key::O = SDLK_o,
		Key::Oper = SDLK_OPER,
		Key::Out = SDLK_OUT,
		Key::P = SDLK_p,
		Key::PageDown = SDLK_PAGEDOWN,
		Key::PageUp = SDLK_PAGEUP,
		Key::Paste = SDLK_PASTE,
		Key::Pause = SDLK_PAUSE,
		Key::Period = SDLK_PERIOD,
		Key::Power = SDLK_POWER,
		Key::PrintScreen = SDLK_PRINTSCREEN,
		Key::Prior = SDLK_PRIOR,
		Key::Q = SDLK_q,
		Key::R = SDLK_r,
		Key::RAlt = SDLK_RALT,
		Key::RCtrl = SDLK_RCTRL,
		Key::Enter = SDLK_RETURN,
		Key::Return = SDLK_RETURN,
		Key::RETURN2 = SDLK_RETURN2,
		Key::RGui = SDLK_RGUI,
		Key::Right = SDLK_RIGHT,
		Key::RightBracket = SDLK_RIGHTBRACKET,
		Key::RShift = SDLK_RSHIFT,
		Key::S = SDLK_s,
		Key::ScrollLock = SDLK_SCROLLLOCK,
		Key::Select = SDLK_SELECT,
		Key::SemiColon = SDLK_SEMICOLON,
		Key::Separator = SDLK_SEPARATOR,
		Key::Slash = SDLK_SLASH,
		Key::Sleep = SDLK_SLEEP,
		Key::Space = SDLK_SPACE,
		Key::Stop = SDLK_STOP,
		Key::SysReq = SDLK_SYSREQ,
		Key::T = SDLK_t,
		Key::Tab = SDLK_TAB,
		Key::U = SDLK_u,
		Key::Undo = SDLK_UNDO,
		Key::Unknown = SDLK_UNKNOWN,
		Key::Up = SDLK_UP,
		Key::V = SDLK_v,
		Key::VolumeDown = SDLK_VOLUMEDOWN,
		Key::VolumeUp = SDLK_VOLUMEUP,
		Key::W = SDLK_w,
		Key::WWW = SDLK_WWW,
		Key::X = SDLK_x,
		Key::Y = SDLK_y,
		Key::Z = SDLK_z;


	void print_window_event(Event const& event)
	{
		if (event.type == SDL_WINDOWEVENT) {
			switch (event.window.event) {
			case SDL_WINDOWEVENT_SHOWN:
				print("Window %|| shown\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_HIDDEN:
				print("Window %|| hidden\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				print("Window %|| exposed\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_MOVED:
				print("Window %|| moved to %||,%||\n",
						event.window.windowID, event.window.data1,
						event.window.data2);
				break;
			case SDL_WINDOWEVENT_RESIZED:
				print("Window %|| resized to %||x%||\n",
						event.window.windowID, event.window.data1,
						event.window.data2);
				break;
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				print("Window %|| size changed to %||x%||\n",
						event.window.windowID, event.window.data1,
						event.window.data2);
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				print("Window %|| minimized\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				print("Window %|| maximized\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_RESTORED:
				print("Window %|| restored\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_ENTER:
				print("Mouse entered window %||\n",
						event.window.windowID);
				break;
			case SDL_WINDOWEVENT_LEAVE:
				print("Mouse left window %||\n", event.window.windowID);
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				print("Window %|| gained keyboard focus\n",
						event.window.windowID);
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				print("Window %|| lost keyboard focus\n",
						event.window.windowID);
				break;
			case SDL_WINDOWEVENT_CLOSE:
				print("Window %|| closed\n", event.window.windowID);
				break;
			default:
				print("Window %|| got unknown event %||\n",
						event.window.windowID, event.window.event);
				break;
			}
		}
	}

	
}
