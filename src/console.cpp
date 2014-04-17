#include "console.h"

namespace col {

	using boost::str;
	
	
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
	
		if (type == sf::Event::TextEntered) {
			handle_char(event.text.unicode);			
		}		
		else
		if (type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::C) {
				
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
				sel[0] = mp.x / 16;
				sel[1] = mp.y / 16;
				
				modified();
			}
		}
	}
	
	void Console::handle_char(uint16 code) {
		if (code == 13) {

			vector<string> es;
			split(es, buffer, is_any_of(" "));

			if (es[0] == "help") {
				output.push_back("lsp  lsi  lsut");
				output.push_back("sel");				
				output.push_back("addp addi");
				output.push_back("delp deli");
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
			else
			if (es[0] == "lsi") {
				for (auto &item: envgame.units) {
					cout << item.second << endl;
					output.push_back(str(format("%||\n") % item.second));
				}
			}
			else
			if (es[0] == "lsut") {
				for (auto &utp: *envgame.uts) {
					auto &ut = utp.second;
					auto x = format("%u  %s\n") % uint16(ut.id) % ut.name;
					output.push_back(str(x));
				}
			}
			else
			if (es[0] == "deli") {
				envgame.destroy<Unit>(std::stoi(es[1]));
			}
			else
			if (es[0] == "addi") {	
				switch (es.size()) {
					default: {
						output.push_back("Usage: addi type_id player_id\n");
						break;
					}
					case 3: {
						envgame.create_icon(
							std::stoi(es.at(1)), // type_id
							std::stoi(es.at(2)),  // player_id
							Coords(sel[0], sel[1]) // x y
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
			if (es[0] == "lsp") {
				for (auto &p: envgame.players) {
					auto x = format("%||\n") % p.second;
					output.push_back(str(x));
				}
			}
			else
			if (es[0] == "delp") {
				envgame.del_player(std::stoi(es[1]));
			}
			else
			if (es.at(0) == "save") {
				switch (es.size()) {
					default: {
						output.push_back("Usage: save filename");
						break;
					}
					case 2: {
						ofstream f(es.at(1), std::ios::binary);
						io::write<EnvGame>(f, envgame);
						break;
					}
				}

			}
			else
			if (es[0] == "addp") {
				if (es.size() < 3) {
					output.push_back("Usage: addp name colorname");					
				}
				else {				
					envgame.add_player(Player(
						envgame.next_key_player(),
						string(es[1]),
						make_color_by_name(string(es[2])),
						flag_id4color_name(string(es[2]))
					));
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