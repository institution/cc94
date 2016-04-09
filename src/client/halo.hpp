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
	uint8_t const ModNone = 0;
	uint8_t const ModAlt = 1 << 0;
	uint8_t const ModCtrl = 1 << 1;
	uint8_t const ModShift = 1 << 2;
	uint8_t const ModButton = 1 << 3;
	


	enum struct Event {
		None, Press, Release, Hover, Char
	};

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

