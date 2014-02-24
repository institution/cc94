#include "console.h"

namespace col {

	
	
	
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
	
	
	
	void Console::handle(sf::RenderWindow const& app, sf::Event const& event) {
		auto type = event.Type;
	
		if (type == sf::Event::TextEntered) {
			handle_char(event.Text.Unicode);			
		}		
		else
		if (event.Type == sf::Event::KeyReleased) {
			if (event.Key.Code == sf::Key::C) {
				
			}				
		}
		else
		if (type == sf::Event::MouseButtonPressed)
		{
			if (event.MouseButton.Button == sf::Mouse::Right)
			{
				sf::Vector2f mp = app.ConvertCoords(
					event.MouseButton.X, 
					event.MouseButton.Y
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
				output.push_back("lsi, addi, deli");
				output.push_back("lsp, addp, delp");
				output.push_back("move");
				output.push_back("attack");
				output.push_back("turn");
			}
			else				
			if (es[0] == "turn") {
				env.exec(action::Turn());
				output.push_back(str(format("Turn %||.%|| ") % env.turn_no % (env.curr_player->name)));
			}
			else
			if (es[0] == "lsi") {
				for (auto &icon: env.icons) {
					output.push_back(str(format("%||\n") % icon.second));
				}
			}
			else
			if (es[0] == "lsut") {
				for (auto &utp: env.uts) {
					auto &ut = utp.second;
					auto x = format("%u  %s\n") % uint16(ut.id) % ut.name;
					output.push_back(str(x));
				}
			}
			else
			if (es[0] == "deli") {
				env.destroy_icon(std::stoi(es[1]));
			}
			else
			if (es[0] == "addi") {	
				switch (es.size()) {
					default: {
						output.push_back("Usage1: addi type_id player_id\n");
						output.push_back("Usage2: addi type_id player_id x y\n");
						break;
					}
					case 3: {
						env.create_icon(
							std::stoi(es.at(1)), // type_id
							std::stoi(es.at(2)),  // player_id
							Coords(sel[0], sel[1]) // x y
						);
						break;
					}							
					case 5: {
						env.create_icon(
							std::stoi(es.at(1)), // type_id
							std::stoi(es.at(2)),  // player_id
							Coords(std::stoi(es.at(3)), std::stoi(es.at(4))) // x y
						);
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
						env.turn_no = stoi(es.at(1));
						break;
					}
				}
			}
			else
			if (es[0] == "lsp") {
				for (auto &p: env.players) {
					auto x = format("%||\n") % p.second;
					output.push_back(str(x));
				}
			}
			else
			if (es[0] == "delp") {
				env.del_player(std::stoi(es[1]));
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
						io::write(f, env);
						break;
					}
				}

			}
			else
			if (es[0] == "addp") {
				if (es.size() < 2+1) {
					output.push_back("Usage: addp name color_name flag_id");					
				}
				else {				
					env.add_player(Player(
						env.next_key_player(),
						string(es[1]),
						make_color_by_name(string(es[2])),
						stoi(es[3])
					));
				}
			}
			else
			if (es[0] == "set_owner") {
				env.set_owner(
					stoi(es[1]),
					stoi(es[2])
				);
			}
			else
			if (es.at(0) == "move") {					
				env.exec(action::AttackMove(					
					env.icon4id(stoi(es.at(1))), // id
					dir4vec(
						Coords(
							stoi(es.at(2)), // dx
							stoi(es.at(3))  // dy
						)
					)
				));
			}
			else
			if (es.at(0) == "attack") {
				env.exec(action::AttackMove(
					env.icon4id(stoi(es.at(1))), // id
					dir4vec(
						Coords(
							stoi(es.at(2)), // dx
							stoi(es.at(3))  // dy
						)
					)
				));
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