#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <functional>
#include <boost/optional.hpp>
#include "align.hpp"

#include "../front/front.hpp"

// event subsystem

namespace halo{

	using front::b2s;
	using front::v2s;
	
	using front::Key;
	
	using boost::optional;

	enum struct Dev {
		None, Mouse, Keyboard
	};


	using Mod = uint8_t;
	Mod const ModNone = 0;
	Mod const ModAlt = 1 << 0;
	Mod const ModCtrl = 1 << 1;
	Mod const ModShift = 1 << 2;	
	Mod const ModButtonLeft = 1 << 3;
	Mod const ModButtonRight = 1 << 4;	
	
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
	}


	enum struct Event {
		None, Press, Release, Hover, Char
	};
	
	inline char const* get_event_name(Event e) {
		switch (e) {
			case Event::None: return "None";
			case Event::Press: return "Press";
			case Event::Release: return "Release";
			case Event::Hover: return "Hover";
			case Event::Char: return "Char";
		}
		return "<Invalid>";
	}

	enum struct Button{
		None, Left, Right,
	};

	using Box = b2s;
	using Vec = v2s;


	struct Pattern{
		optional<Dev> dev;
		optional<Event> event;
		optional<Button> button;
		optional<Key> key;
		optional<Mod> mod;
		optional<Box> area;
		optional<char32_t> unicode;
		std::function<void()> callback;
		std::function<void(Pattern const&)> callback2;

		// any event -> discard
		Pattern() {}

		// unicode entered
		Pattern(Event event, std::function<void(Pattern const&)> callback2) {
			this->event = event;
			this->callback2 = callback2;
		}

		// unicode entered
		Pattern(Event event, char16_t unicode, std::function<void()> callback) {
			this->event = event;
			this->unicode = unicode;
			this->callback = callback;
		}


		// key press/release
		Pattern(Event event, Key key, std::function<void()> callback) {
			this->event = event;
			this->key = key;
			this->callback = callback;
		}
		
		// key+mod press/release
		Pattern(Event event, Mod mod, Key key, std::function<void()> callback) {
			this->event = event;
			this->key = key;
			this->callback = callback;
		}

		// mouse left/right press/release over area
		Pattern(Event event, Button button, Vec pos, Vec dim, std::function<void()> callback) {
			this->event = event;
			this->button = button;
			this->area = Box(pos,dim);
			this->callback = callback;
		}

		// mouse left/right press/release over area with modifier
		Pattern(Event event, Button button, Mod mod, Vec pos, Vec dim, std::function<void()> callback) {
			this->event = event;
			this->button = button;
			this->mod = mod;
			this->area = Box(pos,dim);
			this->callback = callback;
		}

		// mouse left/right press/release
		Pattern(Event event, Button button, std::function<void()> callback) {
			this->event = event;
			this->button = button;
			this->callback = callback;
		}

		// mouse move over area
		Pattern(Event event, Vec pos, Vec dim, std::function<void()> callback) {
			this->event = event;
			this->area = Box(pos,dim);
			this->callback = callback;
		}

		// mouse move over area with mod
		Pattern(Event event, Mod mod, Vec pos, Vec dim, std::function<void()> callback) {
			this->event = event;
			this->mod = mod;
			this->area = Box(pos,dim);
			this->callback = callback;
		}

		// mouse move
		Pattern(Event event, std::function<void()> callback) {
			this->event = event;
			this->callback = callback;
		}

		// any event
		Pattern(std::function<void()> callback) {
			this->callback = callback;
		}


		// device -> discard
		Pattern(Dev dev) {
			this->dev = dev;
		}

		// device
		Pattern(Dev dev, std::function<void()> callback) {
			this->dev = dev;
			this->callback = callback;
		}


		// ----------- autocommand



	};

	std::ostream & operator<<(std::ostream & o, Pattern const& p);
	




	using Patterns = std::vector<Pattern>;


	bool handle_event(Patterns const& ps, Pattern const& q, bool verbose = 0);


}

