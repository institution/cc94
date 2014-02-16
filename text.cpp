#include "text.h"

namespace col {
	

int16 render_text(
		sf::RenderWindow &win, 
		Vector2<int16> const& pos,
		sf::Font const& font,
		string const& text,
		int16 width
	) {
	
	// width
			
	auto x = pos[0];
	auto y = pos[1];
	
	auto &img = font.GetImage();
	
	((sf::Image&)img).SetSmooth(false);
	
	sf::Sprite s;	
	s.SetImage(img);
	
	//auto line_height = font.GetCharacterSize();
	//cout << "char hh:" << hh << endl;
	
	for (auto c: text) {
		auto &g = font.GetGlyph(c);
		auto &rr = g.Rectangle;
		
		auto &r = g.TexCoords;
		cout << "char t:" << rr.Top << "    b:" << rr.Bottom << endl;
		
		//cout << format("c=%1% x,y=%2%,%3%\n") % c % x % y;
		//cout << format("%1%;%2%;%3%;%4%\n") % r.Left % r.Top % r.Right % r.Bottom;
				
		s.SetPosition(x + rr.Left ,y + rr.Top + 6);
		s.SetSubRect(sf::IntRect(
			(r.Left * img.GetWidth()) +0.5, 
			(r.Top * img.GetHeight()) +0.5, 
			(r.Right * img.GetWidth()) +0.5, 
			(r.Bottom * img.GetHeight()) +0.5
		));
		win.Draw(s);
				
		x += g.Advance;
	}
	
	return x;
}


} //col