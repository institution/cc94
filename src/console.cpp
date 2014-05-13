#include "console.h"


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "serialize.hpp"
#include "view_base.h"

namespace col {
	using boost::str;

	Layout const ly(SCREEN_W, SCREEN_H);


	Color make_color_by_name(const string &s) {
		if (s == "red") {
			return Color(255,0,0);
		}
		else
		if (s == "green") {
			return Color(0,255,0);
		}
		else
		if (s == "blue") {
			return Color(0,0,255);
		}
		else {
			return Color(0,0,0);
		}
	}

	uint8 flag_id4color_name(const string &s) {
		if (s == "red") {
			return 119;
		}
		else
		if (s == "blue") {
			return 120;
		}
		else
		if (s == "yellow") {
			return 120;
		}
		else
		if (s == "orange") {
			return 120;
		}
		else
		if (s == "usa") {
			return 131;
		}
		else {
			return 65;
		}
	}


	void Console::handle(sf::RenderWindow const& app, sf::Event const& event) {
		auto type = event.type;


		if (type == sf::Event::KeyPressed) {
			//cerr << "key released!" << event.key.code << endl;

			if (event.key.code == sf::Keyboard::Up) {

				if (chi == history.end()) {
					chi = history.begin();
				}

				if (history.size()) {
					buffer = *chi;
					++chi;

					modified();
				}
			}


			if (event.key.code == sf::Keyboard::Tilde or event.key.code == sf::Keyboard::Unknown) {

			}
		}
		else
		if (type == sf::Event::TextEntered) {


			if (event.text.unicode == u'`') {
				active = !active;
				modified();
			}
			else {
				if (active) {
					handle_char(event.text.unicode);
				}
			}
		}
		else
		if (type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Right)
			{
				sf::Vector2f mp = app.mapPixelToCoords(
					sf::Vector2i(
						event.mouseButton.x,
						event.mouseButton.y
					)
				);
				sel[0] = (mp.x - ly.map.pos[0]) / ly.TERR_W;
				sel[1] = (mp.y - ly.map.pos[1]) / ly.TERR_H;

				modified();
			}
		}
	}


	void Console::handle_char(uint16 code) {
		//cerr << "--" << code << endl;

		if (charset.find(code) == charset.end()) {
			return;
		}

		if (code == u'\b') {
			if (buffer.size()) {
				buffer.pop_back();
			}
		}
		else if (code == u'\r') {

			vector<string> es;
			split(es, buffer, is_any_of(" "));

			auto& cmd = es[0];

			put(buffer);

			if (cmd == "list-players") {
				for (auto &item: envgame.players) {
					put(str(format("%||") % item.second));
				}
			}
			else if (cmd == "comment") {

			}
			else if (cmd == "print") {
				put(buffer.substr(6));
			}
			else if (cmd == "list-orders") {
				put("build-colony");
				put("plow-fields");
				put("clear-forest");
				put("build-road");
				put("move");
			}
			else if (cmd == "help" or cmd == "?") {
				// save/load
				put("save");
				put("load");
				// editor
				put("list-players");
				put("list-units");
				put("list-unit-types");
				put("list-orders");
				put("create-player");
				put("create-unit");
				put("create-colony");
				put("delete-colony");
				put("set-biome");
				put("add-phys");
				// orders
				put("build-colony");
				put("plow-fields");
				put("clear-forest");
				put("build-road");
				put("move");
				put("attack");
				put("assign");
				put("leave");
				// game turn sequence
				put("start");
				put("ready");
				put("turn");
				put("info");
				// ai
				put("ai");
				//put("create-ai");				
				//put("exec-ai");
				//put("list-ais");
				// misc
				put("score");
				put("cls");
				put("print");
				put("comment");
				put("enter");
				put("exit");
			}
			else if (cmd == "add-phys") {
				switch (es.size()) {
					default:
						put("Usage: add-phys <phys-name>");
						break;
					case 2:
						if (envgame.in_bounds(sel)) {
							Phys::type p = get_phys_by_name(es.at(1));
							if (p != Phys::None) {
								envgame.ref_terr(sel).add(p);
							}
							else {
								put("invalid phys name");
							}
						}
						break;
				}
			}
			else if (cmd == "cls") {
				output.clear();
			}
			else if (cmd == "info") {
				switch (es.size()) {
					default:
						put("Usage: info");
						break;
					case 1:
						put("Turn: " + to_string(envgame.get_turn_no()) + "; " +
							"Player: " + envgame.get_current_player().name  + "; " +
							"State: " + to_string(envgame.state)
						);
						break;
				}
			}
			else if (cmd == "score") {
				switch (es.size()) {
					default:
						put("Usage: score <player_id>");
						break;
					case 2:
						auto f = envgame.get_result(stoi(es.at(1)));
						put(to_string(f));
						break;
				}
			}
			else if (cmd == "set-biome") {
				switch (es.size()) {
					default:
						put("Usage: set-biome <biome-name>");
						break;
					case 2:
						if (envgame.in_bounds(sel)) {
							Biome::type b = get_biome_by_name(es.at(1));
							if (b != Biome::None) {
								envgame.ref_terr(sel).set_biome(b);
							}
							else {
								put("invalid biome name");
							}
						}
						break;
				}
			}
			else if (cmd == "turn") {
				envgame.turn();
				put(str(format("Turn %||") % envgame.turn_no));
			}
			else if (cmd == "ai") {				
				// calculate and show preffered move for current player
				switch (es.size()) {
					default: {
						output.push_back("Usage: ai <num> <exec>\n");
						break;
					}
					case 3: {
						if (envgame.in_progress()) {
							auto const& pid = envgame.get_current_player().id;
							auto it = ais.find(pid);
							if (it == ais.end()) {
								put("creating ai for this player");
								create_ai(pid);
							}
							
							auto& ai = ais.at(pid);
							auto const& a = ai.calc(envgame, stoi(es.at(1)), 0);
							put(string("action: ") + to_string(a));
							if (stoi(es.at(2)) == 1) {
								exec(a);
							}

						}
						else {
							put("game in regress; start game first");
						}
						break;
					}
				}
			}
			else if (cmd == "start") {
				switch (es.size()) {
					default: {
						output.push_back("Usage: start\n");
						break;
					}
					case 1: {
						envgame.start();
						break;
					}
				}
			}
			else if (cmd == "ready") {
				switch (es.size()) {
					default: {
						output.push_back("Usage: ready [player_id]\n");
						break;
					}
					case 2: {
						exec(Ready(std::stoi(es.at(1))));
						break;
					}
					case 1: {
						exec(Ready(envgame.get_current_player().id));
						break;
					}
				}
			}
			else if (cmd == "list-units") {
				for (auto &item: envgame.units) {
					put(str(format("%||") % item.second));
				}
			}
			else if (cmd == "list-unit-types") {
				for (auto &utp: *envgame.uts) {
					auto &ut = utp.second;
					put(str(format("%u: %s") % uint16(ut.id) % ut.name));
				}
			}
			else
			if (es[0] == "deli") {
				envgame.destroy<Unit>(std::stoi(es[1]));
			}
			else if (cmd == "create-unit") {
				switch (es.size()) {
					default: {
						put("Usage: create-unit <type_id> <player_id>");
						break;
					}
					case 3: {
						auto& c = envgame.get<UnitType>(std::stoi(es.at(1))); // type_id
						auto& p = envgame.get<Player>(std::stoi(es.at(2)));  // player_id
						auto& t = envgame.ref_terr(Coords(sel[0], sel[1]));  // coords
						auto& u = envgame.create<Unit>(c, p);
						envgame.move_in(t, u);
						break;
					}
				}
			}
			else if (cmd == "create-player") {
				switch (es.size()) {
					default: {
						put("Usage: create-player <player-name> <color-name>");
						break;
					}
					case 3: {
						envgame.create<Player>(
							string(es[1]), // name
							make_color_by_name(string(es[2])), // color
							flag_id4color_name(string(es[2]))  // flag_id
						);
						break;
					}
				}
			}
			else if (cmd == "delete-colony") {
				switch (es.size()) {
					default:
						put("Usage: delete-colony");
						break;
					case 1:
						auto &t = envgame.get_terr(sel);
						if (t.has_colony()) {
							envgame.burn_colony(t);
						}
						else {
							put("there is no colony at selected location");
						}
						break;
				}
			}
			else
			if (es[0] == "sel") {
				switch (es.size()) {
					default: {
						output.push_back("Usage: sel x y\n");
						break;
					}
					case 3: {
						sel = Coords(std::stoi(es.at(1)), std::stoi(es.at(2)));
						break;
					}
				}
			}
			else
			if (es[0] == "setturn") {
				switch (es.size()) {
					default: {
						output.push_back("Usage1: setturn num\n");
						break;
					}
					case 2: {
						envgame.turn_no = stoi(es.at(1));
						break;
					}
				}
			}
			else
			if (es[0] == "delp") {
				envgame.del_player(std::stoi(es[1]));
			}
			else if (cmd == "save") {
				switch (es.size()) {
					default:
						put("Usage: save <filename>");
						break;
					case 2: {
							ofstream f(es.at(1), std::ios::binary);
							boost::archive::text_oarchive oa(f);
							oa << envgame;
						}
						break;
				}
			}
			else if (cmd == "load") {
				switch (es.size()) {
					default:
						put("Usage: load <filename>");
						break;
					case 2: {
							ifstream f(es.at(1), std::ios::binary);
							boost::archive::text_iarchive oa(f);
							oa >> envgame;
						}
						break;
				}
			}
			else
			if (es[0] == "set_owner") {
				envgame.set_owner(
					stoi(es[1]),
					stoi(es[2])
				);
			}
			else if (cmd == "build-road") {
				switch (es.size()) {
					default:
						put("Usage: build-road");
						break;
					case 1:
						if (envgame.has_defender(sel)) {
							auto ret = envgame.build_road(
								envgame.get_defender(envgame.get_terr(sel))
							);
							put(str(format("ret = %||") % ret));
						}
						else {
							put("no unit selected");
						}
						break;
				}

			}
			else if (cmd == "add-item") {
				switch (es.size()) {
					default:
						put("Usage: add-item <item-name> <amount>");
						break;
					case 3:
						auto number = stoi(es.at(2));
						auto item = get_item_by_name(es.at(1));

						auto &terr = envgame.get_terr(sel);
						terr.get_colony().add(Cargo(item, number));
						break;
				}
			}
			else if (cmd == "assign") {
				switch (es.size()) {
					default:
						put("Usage: assign <number> <unit-id> <item-name>");
						break;
					case 4:
						auto number = stoi(es.at(1));
						if (0 <= number and number <= 16+9) {
							auto unit_id = stoi(es.at(2));
							auto item = get_item_by_name(es.at(3));
							auto &unit = envgame.get<Unit>(unit_id);
							envgame.assign(number, unit, item);
						}
						else {
							put("number must be in [0,25] range");
						}
						break;
				}
			}
			else if (cmd == "leave") {
				switch (es.size()) {
					default:
						put("Usage: leave <number> <unit-id>");
						break;
					case 4:
						auto number = stoi(es.at(1));
						auto unit_id = stoi(es.at(2));
						auto &unit = envgame.get<Unit>(unit_id);
						envgame.leave(number, unit);
						break;
				}
			}
			else if (cmd == "build-colony") {
				switch (es.size()) {
					default:
						put("Usage: build-colony <name>");
						break;
					case 2:
						if (envgame.has_defender(sel)) {
							auto ret = envgame.colonize(
								envgame.get_defender(envgame.get_terr(sel)),
								es.at(1)
							);
						}
						else {
							put("no unit selected");
						}
						break;
				}

			}
			else if (cmd == "move") {
				switch (es.size()) {
					default:
						put("Usage: move <dx> <dy>");
						break;
					case 3:
						if (envgame.has_defender(sel)) {
							exec(OrderMove(							
								envgame.get_current_player().id,
								envgame.get_defender(envgame.get_terr(sel)).id,
								dir4vec(
									Coords(
										stoi(es.at(1)), // dx
										stoi(es.at(2))  // dy
									)
								)
							));
						}
						else {
							put("no unit selected");
						}
						break;
				}
			}
			else if (cmd == "attack") {
				switch (es.size()) {
					default:
						put("Usage: attack <dx> <dy>");
						break;
					case 3:
						if (envgame.has_defender(sel)) {
							exec(OrderAttack(
								envgame.get_current_player().id,
								envgame.get_defender(envgame.get_terr(sel)).id,
								dir4vec(
									Coords(
										stoi(es.at(1)), // dx
										stoi(es.at(2))  // dy
									)
								)
							));
						}
						else {
							put("no unit selected");
						}
						break;
				}
			}
			else
			if (es.at(0) == "enter") {
				switch (es.size()) {
					default: {
						output.push_back("Usage: enter [x y]");
						break;
					}
					case 1: {
						mode = Mode::COLONY;
						mod++;
						break;
					}
					case 3: {
						sel[0] = stoi(es[1]);
						sel[1] = stoi(es[2]);
						mode = Mode::COLONY;
						mod++;
						break;
					}
				}
			}
			else
			if (es.at(0) == "exit") {
				switch (es.size()) {
					default: {
						output.push_back("Usage: exit");
						break;
					}
					case 1: {
						mode = Mode::AMERICA;
						mod++;
						break;
					}
				}
			}
			else {
				output.push_back("ERROR: no such command");

			}

			history.push_front(buffer);
			buffer = "";
			chi = history.begin();

		}
		else {
			//cout << char(code);
			buffer.push_back(char(code));
		}

		//cout.flush();
		++mod;
	}

}