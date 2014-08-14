#ifndef HALO_H
#define HALO_H

#include <SFML/System.hpp> // TODO: no need for this here
#include <SFML/Window.hpp> // TODO: no need for this here

#include <vector>
#include <cstdint>
#include <string>
#include <functional>
#include <boost/optional.hpp>
#include "box.h"

// event subsystem

namespace halo{

	using col::Box2;
	using col::v2i;
	using boost::optional;

	enum struct Dev {
		None, Mouse, Keyboard
	};



	uint8_t const ModNone = 0;
	uint8_t const ModAlt = 1 << 0;
	uint8_t const ModCtrl = 1 << 1;
	uint8_t const ModShift = 1 << 2;


	enum struct Event {
		None, Press, Release, Hover, Char
	};

	enum struct Button{
		None, Left, Right,
	};

	using Key = sf::Keyboard::Key;


	struct Pattern{
		optional<Dev> dev;
		optional<Event> event;
		optional<Button> button;
		optional<Key> key;
		optional<uint8_t> mod;
		optional<Box2> area;
		optional<uint32_t> unicode;
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
		Pattern(Event event, uint32_t unicode, std::function<void()> callback) {
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

		// mouse left/right press/release left/right over area
		Pattern(Event event, Button button, v2i const& pos, v2i const& dim, std::function<void()> callback) {
			this->event = event;
			this->button = button;
			this->area = Box2(pos,dim);
			this->callback = callback;
		}

		// mouse left/right press/release
		Pattern(Event event, Button button, std::function<void()> callback) {
			this->event = event;
			this->button = button;
			this->callback = callback;
		}

		// mouse move over area
		Pattern(Event event, v2i const& pos, v2i const& dim, std::function<void()> callback) {
			this->event = event;
			this->area = Box2(pos,dim);
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

	using Patterns = std::vector<Pattern>;


	bool handle_event(Patterns const& ps, Pattern const& q);


}


#endif
