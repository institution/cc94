#ifndef BACKEND_H_583674
#define BACKEND_H_583674

#include <SDL2/SDL.h>
#include <format.hpp>
#include <filesys.hpp>
#include <aga2.hpp>

#ifdef __EMSCRIPTEN__
#include "emscripten/emscripten.h"
#endif
			
#include "lodepng/lodepng.h"
#include "box.hpp"


namespace backend{
	
	
	inline void error_sdl() {
		error("%||\n", SDL_GetError());		
	}
	
	
	inline SDL_Rect make_sdl_rect(v2i const& pos, v2i const& dim) {
		SDL_Rect dst;
		dst.x = pos[0]; dst.y = pos[1]; dst.w = dim[0]; dst.h = dim[1];
		return dst;
	}
	
	
	struct Color {
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			uint8_t r,g,b,a;		
		#else
			uint8_t a,b,g,r;		
		#endif
		
		Color() = default;
		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a): r(r), g(g), b(b), a(a) {}
		Color(uint8_t r, uint8_t g, uint8_t b): Color(r,g,b,255) {}
		
	};
	
	inline std::ostream & operator<<(std::ostream & o, Color const& c) {
		o << format("(%|| %|| %|| %||)", uint(c.r), uint(c.g), uint(c.b), uint(c.a));
		return o;
	}
	
	inline bool operator!=(Color const& x, Color const& y) {
		return x.r != y.r or x.g != y.g or x.b != y.b or x.a != y.a;
	}
	
	inline bool operator==(Color const& x, Color const& y) {
		return x.r == y.r and x.g == y.g and x.b == y.b and x.a == y.a;
	}
	
	
	struct Texture{
		SDL_Texture *tex{nullptr};
		
		Texture() = default;
		Texture(Texture const&) = delete;
		Texture(SDL_Texture *tex): tex(tex) {}
		Texture(Texture && o) noexcept: tex(std::move(o.tex)) {
			o.tex = nullptr;
		}

		void operator=(Texture && o) {
			assert(tex == nullptr);
			tex = std::move(o.tex);
			o.tex = nullptr;
		}

		~Texture() {
			if (tex) SDL_DestroyTexture(tex);			
		}
		
		v2i get_dim() const {
			int w,h;
			SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
			return v2i(w,h);
		}
		
		
		
	};
	
	
		
	struct Surface{
		private:
		SDL_Surface *s{nullptr};
		public:
		
		Surface() = default;
		Surface(Surface const&) = delete;
		Surface(Surface && o) noexcept: s(std::move(o.s)) {
			o.s = nullptr;
		}
		
		void operator=(Surface && o) {
			assert(s == nullptr);
			s = std::move(o.s);
			assert(o.s == nullptr);
		}
		
		Surface(v2i const& dim) {
			init(dim);
		}
		
		Surface(v2i const& dim, Color const& c) {
			init(dim);
			auto r = make_sdl_rect({0,0}, dim);
			SDL_FillRect(s, &r, 
				SDL_MapRGBA(s->format, c.r, c.g, c.b, c.a)
			);			
		}
		
		~Surface() {
			if (s) {
				SDL_FreeSurface(s);
			}
		}
		
		SDL_Surface* get() { 
			return s; 
		}
		
		Surface & init(v2i const& dim) {
			if (s) error("surface already exists");
			
			uint32_t rmask, gmask, bmask, amask;
			#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				rmask = 0xff000000;
				gmask = 0x00ff0000;
				bmask = 0x0000ff00;
				amask = 0x000000ff;
			#else
				rmask = 0x000000ff;
				gmask = 0x0000ff00;
				bmask = 0x00ff0000;
				amask = 0xff000000;
			#endif
			
			unsigned width = dim[0];
			unsigned height = dim[1];
			
			s = SDL_CreateRGBSurface(
				0,
				width, height,
				32,
				rmask, gmask, bmask, amask
			);
			
			if (s == nullptr) error_sdl();
			
			
			
			return *this;
		}
		
		
		Surface & load_png(filesys::Path const& path) {
			std::vector<unsigned char> pixels;
			unsigned width, height;			
			unsigned err = lodepng::decode(pixels, width, height, path);
			if (err) {
				print_err("ERROR: lodepng: %||: %||\n", lodepng_error_text(err), path);
				error();
			}
			
			init(v2i(width, height));
			
			uint8_t * ptr = &pixels[0];
			
			for (size_t j=0; j<height; ++j) {
				for (size_t i=0; i<width; ++i) {
					uint8_t r = *ptr;
					++ptr;
					uint8_t g = *ptr;
					++ptr;
					uint8_t b = *ptr;
					++ptr;
					uint8_t a = *ptr;
					++ptr;
					set_pixel({i,j}, Color(r,g,b,a));
				}
			}
			
			return *this;
		}
		
		v2i get_dim() const {
			return v2i(s->w, s->h);
		}
		
		uint32_t * _pixptr(v2i const& pos) const {
			auto pixel = reinterpret_cast<uint8_t*>(s->pixels);
			pixel += pos[0] * sizeof(uint32_t) + pos[1] * s->pitch;
			return reinterpret_cast<uint32_t*>(pixel);			
		}
		
		Color get_pixel(v2i const& pos) const {
			assert(0 <= pos[0]);
			assert(0 <= pos[1]);
			assert(pos[0] < s->w);
			assert(pos[1] < s->h);
			
			auto pix = _pixptr(pos);
			Color c;
			SDL_GetRGBA(*pix, s->format, &c.r, &c.g, &c.b, &c.a);
			return c;
		}
		
		void set_pixel(v2i const& pos, Color const& c) {
			assert(0 <= pos[0]);
			assert(0 <= pos[1]);
			assert(pos[0] < s->w);
			assert(pos[1] < s->h);			
			
			auto pix = _pixptr(pos);			
			*pix = SDL_MapRGBA(s->format, c.r, c.g, c.b, c.a);
		}
		
		
		
	};
	
	inline v2i get_dim(Surface const& s) { return s.get_dim(); }
	
	inline v2i get_dim(Texture const& t) { return t.get_dim(); }
	

	using Event = SDL_Event;
	using EventType = SDL_EventType;

	EventType const
		//EventAudioDeviceAdded = SDL_AUDIODEVICEADDED,
		//EventAudioDeviceRemoved = SDL_AUDIODEVICEREMOVED,
		EventControllerAxisMotion = SDL_CONTROLLERAXISMOTION,
		EventControllerButtonDown = SDL_CONTROLLERBUTTONDOWN,
		EventControllerButtonUp = SDL_CONTROLLERBUTTONUP,
		EventControllerDeviceAdded = SDL_CONTROLLERDEVICEADDED,
		EventControllerDeviceRemoved = SDL_CONTROLLERDEVICEREMOVED,
		EventControllerDeviceMapped = SDL_CONTROLLERDEVICEREMAPPED,
		EventDollarGesture = SDL_DOLLARGESTURE,
		EventDollarRecord = SDL_DOLLARRECORD,
		EventDropFile = SDL_DROPFILE,
		//EventFingerMotion = SDL_FINGERMOTION,
		//EventFingerDown = SDL_FINGERDOWN
		EventFingerUp = SDL_FINGERUP,
		EventKeyDown = SDL_KEYDOWN,
		EventKeyUp = SDL_KEYUP,
		EventJoyAxisMotion = SDL_JOYAXISMOTION,
		EventJoyBallMotion = SDL_JOYBALLMOTION,
		EventJoyHatMotion = SDL_JOYHATMOTION,
		EventJoyButtonDown = SDL_JOYBUTTONDOWN,
		EventJoyButtonUp = SDL_JOYBUTTONUP,
		EventJoyDeviceAdded = SDL_JOYDEVICEADDED,
		EventJoyDeviceRemoved = SDL_JOYDEVICEREMOVED,
		EventMouseMotion = SDL_MOUSEMOTION,
		EventMouseButtonDown = SDL_MOUSEBUTTONDOWN,
		EventMouseButtonUp = SDL_MOUSEBUTTONUP,
		EventMouseWheel = SDL_MOUSEWHEEL,
		EventMultiGesture = SDL_MULTIGESTURE,
		EventQuit = SDL_QUIT,
		EventSysWMEvent = SDL_SYSWMEVENT,
		EventTextEditing = SDL_TEXTEDITING,
		EventTextInput = SDL_TEXTINPUT,
		EventUserEvent = SDL_USEREVENT,
		EventWindowEvent = SDL_WINDOWEVENT;

	void print_window_event(Event const& event);

	using Tick = int64_t;

	struct Back{
		
		SDL_Window *win{nullptr};
		SDL_Renderer *ren{nullptr};
		
		
		bool verbose{true};
		bool done{false};
		
		Back() = default;
			
		Back(std::string const& title, v2i dim) {
			init(title, dim);
		}
		
		void stop() {
			done = true;
		}
		
		

		
		void init(std::string const& title, v2i dim) {
			
			if (verbose)
				print("Backend.init; title=%||\n", title);
			
			#ifdef __EMSCRIPTEN__
				auto r = SDL_Init(SDL_INIT_VIDEO);
			#else		
				auto r = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
			#endif		
	
			
			if (r != 0) error_sdl();			
			
			win = SDL_CreateWindow(
				title.c_str(), 
				 SDL_WINDOWPOS_UNDEFINED,  SDL_WINDOWPOS_UNDEFINED, 
				dim[0], dim[1], 
				SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE
			);
			
			if (win == nullptr) error_sdl();
			
			ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);			
			
			clear();
			flip();
				
			//SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_OPENGL, &win, &ren);
			
		}
		
		void clear() {
			// clear screen
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
			SDL_RenderClear(ren);
		}
		
		void set_renderer_dim() {
			
		}
		
		void set_logical_dim(v2i dim) {
			SDL_RenderSetLogicalSize(ren, dim[0], dim[1]);			
		}
		
		
		static 
		Surface make_surface_from_png(filesys::Path const& path) {
			Surface a;
			a.load_png(path);
			return a;
		}
		
		v2i get_logical_dim() const {
			v2i dim;
			SDL_RenderGetLogicalSize(ren, &dim[0], &dim[1]);
			return dim;
		}
		
		v2i get_output_dim() const {
			v2i dim;
			SDL_GetRendererOutputSize(ren, &dim[0], &dim[1]);
			return dim;
		}
		
		Texture make_texture(Surface & surface) {
			SDL_Texture *sdl_tex = SDL_CreateTextureFromSurface(ren, surface.get());

			if (sdl_tex == nullptr) {
				error_sdl();				
			}

			return Texture(sdl_tex);
		}

		
		
		void flip() {
			SDL_RenderPresent(ren);
		}
		
		
		Texture load(filesys::Path const& path) {
			Texture tex;
			load(tex, path);
			return tex;
		}
		
		void load(Texture & tex, filesys::Path const& path) {
			
			auto s = Surface();
			s.load_png(path);
			
			load_surface(tex, s);			
		}
		
		void load_surface(Texture & tex, Surface & s) {
			SDL_Texture *t = SDL_CreateTextureFromSurface(ren, s.get());
						
			if (t == nullptr) error_sdl();
			
			tex.tex = t;
		}
		
		
		void _render_line(v2i const& pos, v2i const& end) {
			SDL_RenderDrawLine(ren, pos[0], pos[1], end[0], end[1]);
		}
		
		void render_line(v2i const& pos, v2i const& end, Color const& c) {
			SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
			_render_line(pos, end);
		}
				
		void render_hline(v2i const& pos, int len, Color const& c) {
			if (len)
				render_line(pos, pos + v2i(len-1, 0), c);
		}
		
		void render_vline(v2i const& pos, int len, Color const& c) {
			if (len)
				render_line(pos, pos + v2i(0, len-1), c);
		}
		
		void render_rect(v2i const& pos, v2i const& dim, Color const& c) {
			SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
			SDL_Rect r = make_sdl_rect(pos, dim);
			SDL_RenderDrawRect(ren, &r);
		}
		
		void render_rect(Box const& box, Color const& c) {
			render_rect(box.pos, box.dim, c);
		}
		
		void render_fill(v2i const& pos, v2i const& dim, Color const& c) {
			SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
			SDL_Rect r = make_sdl_rect(pos, dim);
			SDL_RenderFillRect(ren, &r);
		}
		
		void render_fill(Box const& box, Color const& c) {
			render_fill(box.pos, box.dim, c);
		}
		
		
		//void render(Tex const& t, Box const& src, Box const& dst)
		void render(Texture const& t, v2i const& pos) {			
			SDL_Rect dst = make_sdl_rect(pos, t.get_dim());
			SDL_RenderCopy(ren, t.tex, nullptr, &dst);
		}
		
		void render_clip(Texture const& t, v2i const& pos, Box const& clip) {
			SDL_Rect dst = make_sdl_rect(pos, clip.dim);
			SDL_Rect src = make_sdl_rect(clip.pos, clip.dim);
			SDL_RenderCopy(ren, t.tex, &src, &dst);
		}
		
		
		v2i get_dim() {
			v2i dim;
			SDL_GetWindowSize(win, &dim[0], &dim[1]);
			return dim;
		}
		
		~Back() {
			if (ren) SDL_DestroyRenderer(ren);
			if (win) SDL_DestroyWindow(win);
			SDL_Quit();
		}
		
		
		bool pool_event(Event & event) {
			return SDL_PollEvent(&event);
		}
		
		Tick get_ticks() {
			#ifdef __EMSCRIPTEN__
				return emscripten_get_now();
			#else		
				return SDL_GetTicks();
			#endif
		}
			
		
		
	};
	
	inline uint8_t const* get_keyboard_state() {
		return SDL_GetKeyboardState(0);
	}
	

	using Button = uint32_t;
	Button const
		ButtonLeft = SDL_BUTTON_LEFT,
		ButtonMiddle = SDL_BUTTON_MIDDLE,
		ButtonRight = SDL_BUTTON_RIGHT;
		
		
	
		
	
	using Keycode = SDL_Keycode;
	
	
	struct Key{
		static
		Keycode const
			Num0,
			Num1,
			Num2,
			Num3,
			Num4,
			Num5,
			Num6,
			Num7,
			Num8,
			Num9,
			A,
			Quote,
			B,
			Backslash,
			Backspace,
			C,
			CapsLock,
			Comma,
			D,
			Delete,
			Down,
			E,
			End,
			Equals,
			Esc,
			F,
			F1,
			F10,
			F11,
			F12,
			F13,
			F14,
			F15,
			F16,
			F17,
			F18,
			F19,
			F2,
			F20,
			F21,
			F22,
			F23,
			F24,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			FIND,
			G,
			Backquote,
			H,
			Home,
			I,
			Insert,
			J,
			K,
			Numpad0,
			Numpad00,
			Numpad000,
			Numpad1,
			Numpad2,
			Numpad3,
			Numpad4,
			Numpad5,
			Numpad6,
			Numpad7,
			Numpad8,
			Numpad9,
			NumpadA,
			NumpadAmpersand,
			NumpadAt,
			NumpadBackspace,
			NumpadBinary,
			NumpadColon,
			NumpadComma,
			NumpadDecimal,
			NumpadDivide,
			NumpadEnter,
			NumpadMinus,
			NumpadMultiply,
			NumpadPeriod,
			NumpadPlus,
			NumpadVerticalBar,
			NumpadXor,
			L,
			LAlt,
			LCtrl,
			Left,
			LEFTBRACKET,
			LGUI,
			LShift,
			M,
			Menu,
			Minus,
			Mode,
			Mute,
			N,
			NumLockClear,
			O,
			Oper,
			Out,
			P,
			PageDown,
			PageUp,
			Paste,
			Pause,
			Period,
			Power,
			PrintScreen,
			Prior,
			Q,
			R,
			RAlt,
			RCtrl,
			Enter,
			Return,
			RETURN2,
			RGui,
			Right,
			RightBracket,
			RShift,
			S,
			ScrollLock,
			Select,
			SemiColon,
			Separator,
			Slash,
			Sleep,
			Space,
			Stop,
			SysReq,
			T,
			Tab,
			U,
			Undo,
			Unknown,
			Up,
			V,
			VolumeDown,
			VolumeUp,
			W,
			WWW,
			X,
			Y,
			Z;
	};
	
		
	struct Mouse{
		v2i pos;
		uint32_t fbuttons;
		bool operator[](Button const button) const {
			return fbuttons & SDL_BUTTON(button);
		}
		v2i get_pos() const {
			return pos;
		}		
	};
		
	inline Mouse get_mouse() {
		Mouse m;
		m.fbuttons = SDL_GetMouseState(&m.pos[0], &m.pos[1]);
		return m;
	}
		
	using Device = uint32_t ;
	Device const
		DeviceNone = 0,
		DeviceKeyboard = 1,
		DeviceMouse = 1;
	
	
	
	// SDL_RenderSetLogicalSize
	

	
	inline void render_line(Back & back, v2i const& pos, v2i const& end, Color const& c) {
		back.render_line(pos, end, c);
	}
			
	inline void render_hline(Back & back, v2i const& pos, int len, Color const& c) {
		back.render_hline(pos, len, c);
	}
	
	inline void render_vline(Back & back, v2i const& pos, int len, Color const& c) {
		back.render_vline(pos, len, c);
	}
	
	inline void render_rect(Back & back, Box const& box, Color const& c) {
		back.render_rect(box, c);
	}
	
	inline void render_fill(Back & back, Box const& box, Color const& c) {
		back.render_fill(box, c);
		
	}

	inline void render(Back & back, Texture const& t, v2i const& pos) {
		back.render(t, pos);
	}
	
	
}

#endif
