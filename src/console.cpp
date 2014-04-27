#include "console.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "serialize.hpp"

namespace col {

	using boost::str;
	
	Layout const ly(320, 200);

	
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
	
		
		if (type == sf::Event::KeyReleased) {
			//cerr << "key released!" << event.key.code << endl;
			
			if (event.key.code == sf::Keyboard::C) {
				
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
			else if (cmd == "help" or cmd == "?") {
				put("list-players");
				put("list-units");
				put("list-unit-types");
				put("create-player");
				put("create-unit");
				
				output.push_back("sel");				
				output.push_back("addp addi");
				output.push_back("move attack");
				output.push_back("enter exit");
				output.push_back("save turn");
			}
			else				
			if (es[0] == "turn") {
				envgame.turn();
				output.push_back(str(format("Turn %|| ") % envgame.turn_no));
			}
			else				
			if (es[0] == "endturn") {
				switch (es.size()) {
					default: {
						output.push_back("Usage: endturn [player_id]\n");
						break;
					}
					case 2: {
						envgame.exec(EndTurn(std::stoi(es.at(1))));
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
			else
			if (es.at(0) == "move") {
				switch (es.size()) {
					default:
						output.push_back("Usage: move [icon_id] dx dy");
						break;
					case 4:
						envgame.exec(AttackMove(
							envgame.current_player_id,
							stoi(es.at(1)), // id
							dir4vec(
								Coords(
									stoi(es.at(2)), // dx
									stoi(es.at(3))  // dy
								)
							)
						));
						break;
					case 3:
						/*
						envgame.exec(AttackMove(
							envgame.current_player_id,
							envgame.ref_icon_at(sel).id,
							dir4vec(
								Coords(
									stoi(es.at(1)), // dx
									stoi(es.at(2))  // dy
								)
							)
						));
						 */
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

			buffer = "";			
		}			
		else {
			cout << char(code);
			buffer.push_back(char(code));
		}

		cout.flush();
		++mod;
	}

}