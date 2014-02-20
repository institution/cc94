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
				output = "lsi, addi, deli\nlsp, addp, delp\nmove\nattack\nturn\n";
			}
			else				
			if (es[0] == "turn") {
				env.exec(action::Turn());
				output = str(format("Turn %||.%|| ") % env.turn_no % (env.curr_player->name));
			}
			else
			if (es[0] == "lsi") {
				for (auto &icon: env.icons) {
					cout << icon.second << endl;						
				}
			}
			else
			if (es[0] == "lsut") {
				for (auto &utp: env.uts) {
					auto &ut = utp.second;
					cout << format("%u  %s\n") % uint16(ut.id) % ut.name;
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
						cout << "Usage1: addi type_id player_id" << endl;
						cout << "Usage2: addi type_id player_id x y" << endl;						
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
						cout << "Usage1: setturn num" << endl;
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
					cout << p.second << endl;						
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
						cout << "Usage: save filename" << endl;
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
					cout << "Usage: addp name color_name flag_id" << endl;
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
				env.exec(action::Move(					
					stoi(es.at(1)), // id
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
				env.exec(action::Attack(					
					stoi(es.at(1)), // id
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
						cout << "Usage: enter [x y]" << endl;
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
						cout << "Usage: exit" << endl;
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
				cout << "ERROR: no such command" << endl;
			}

			buffer = "";	
			cout << "> ";			
		}			
		else {
			cout << char(code);
			buffer.push_back(char(code));
		}

		cout.flush();
		++mod;
	}

}