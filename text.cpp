#include "text.h"


void render_text(sf::RenderWindow &win, const Text &text) {
	
	//width
			
	auto x = text.pos.x;
	auto y = text.pos.y;
	
	auto &img = text.f->GetImage();
	
	sf::Sprite s;
	s.SetImage(img);
	
	for (auto c: text.text) {
		auto &g = text.f->GetGlyph(c);
		auto &r = g.TexCoords;
		
		//cout << format("c=%1% x,y=%2%,%3%\n") % c % x % y;
		//cout << format("%1%;%2%;%3%;%4%\n") % r.Left % r.Top % r.Right % r.Bottom;
		
		
		s.SetPosition(x,y);
		s.SetSubRect(sf::IntRect(
			(r.Left * img.GetWidth()) +0.5, 
			(r.Top * img.GetHeight()) +0.5, 
			(r.Right * img.GetWidth()) +0.5, 
			(r.Bottom * img.GetHeight()) +0.5
		));
		win.Draw(s);
				
		x += g.Advance;
	}
	
}
