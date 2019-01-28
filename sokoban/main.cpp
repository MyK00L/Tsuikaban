#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

std::ifstream in;
std::ofstream out;

struct Player {
	sf::Vector2i pos;
};
struct Cube {
	sf::Vector2i pos;
	sf::Color col;
	int n;
};
class Level {
public:
	std::vector<std::vector<char> > m;
	Player player;
	std::vector<Cube> cubes;
	bool move(const sf::Vector2i p, const sf::Vector2i dp){
		bool r = _move(p,dp);
		cubes.resize(std::remove_if(cubes.begin(),cubes.end(),[](Cube& c)->bool{return c.n==0;})-cubes.begin());
		return r;
	}
private:
	bool _move(const sf::Vector2i p, const sf::Vector2i dp) {
		if (p.y < 0 || p.x < 0 || p.y >= (int)m.size() || p.x >= (int)m[0].size())return 0;
		if (m[p.y][p.x] == '#')return 0;
		if (p == player.pos) {
			if (_move(p + dp, dp)) {
				player.pos += dp;
				return 1;
			}
			return 0;
		}
		for (auto& i : cubes) {
			if (i.pos == p) {
				if (p.y+dp.y < 0 || p.x+dp.x < 0 || p.y+dp.y >= (int)m.size() || p.x+dp.x >= (int)m[0].size())return 0;
				if (m[p.y + dp.y][p.x + dp.x] == 'D') {
					return 0;
				}
				for (auto& j : cubes) {
					if (j.pos == i.pos + dp && j.col==i.col) {
						j.n += i.n;
						i.n=0;
						return 1;
					}
				}
				if (_move(p + dp, dp)) {
					i.pos += dp;
					return 1;
				}
				return 0;
			}
		}
		if (m[p.y][p.x] == '.' || m[p.y][p.x]=='D')return 1;
		return 0;
	}
};
std::ifstream& operator>>(std::ifstream& in, Level& lvl) {
	size_t h, w, cn;
	in >> h >> w;
	lvl.m.resize(h);
	for (auto& i : lvl.m) {
		i.resize(w);
		for (auto& j : i) {
			in >> j;
		}
	}
	in >> lvl.player.pos.x >> lvl.player.pos.y;
	in >> cn;
	lvl.cubes.resize(cn);
	for (auto& i : lvl.cubes) {
		unsigned short r, g, b;
		in >> i.pos.x >> i.pos.y >> r >> g >> b >> i.n;
		i.col = { (sf::Uint8)r,(sf::Uint8)g,(sf::Uint8)b };
	}
	return in;
}

int main()
{
	std::ios_base::sync_with_stdio(0);
	sf::Texture groundTexture, wallTexture, playerTexture, cubeTexture, doorTexture;
	sf::Font font;
	if (!(groundTexture.loadFromFile("res/ground.png")
		&wallTexture.loadFromFile("res/wall.png")
		&playerTexture.loadFromFile("res/player.png")
		&cubeTexture.loadFromFile("res/cube.png")
		&doorTexture.loadFromFile("res/door.png")
		&font.loadFromFile("res/DejaVuSansMono.ttf"))) {
		return 1;
	}
	sf::Sprite groundSprite(groundTexture), wallSprite(wallTexture), playerSprite(playerTexture), cubeSprite(cubeTexture), doorSprite(doorTexture);
	sf::Text cubeTxt("0", font, 16);
	cubeTxt.setFillColor({ 0,0,0 });

	Level level;

	while (1) {
		in.open("res/save.dat");
		int ll = 0;
		if (!in.good()) {
			in.close();
			out.open("res/save.dat");
			out << 0 << std::endl;
			out.close();
		}
		else {
			in >> ll;
			in.close();
		}
		std::cout << "Last available level: " << ll << std::endl;
		int ln = ll;
		do {
			std::cout << "select level: ";
			std::cin >> ln;
		} while (ln > ll || ln<0);

		in.open("res/levels/level"+std::to_string(ln)+".txt");
		if(!in.good()){
			std::cout<<"level "<<ln<<" is not an available level"<<std::endl;
			std::cout<<"closing the game..."<<std::endl;
			return 0;
		}
		in >> level;
		in.close();

		sf::RenderWindow window(sf::VideoMode(512,512), "Level " + std::to_string(ln));
		window.setFramerateLimit(30);
		window.setKeyRepeatEnabled(0);

		float side = std::min((float)window.getSize().x / (float)level.m.size(), (float)window.getSize().y / (float)level.m[0].size());
		groundSprite.setScale(side/groundTexture.getSize().x,side / groundTexture.getSize().y);
		wallSprite.setScale(side / wallTexture.getSize().x, side / wallTexture.getSize().y);
		playerSprite.setScale(side / playerTexture.getSize().x, side / playerTexture.getSize().y);
		cubeSprite.setScale(side / cubeTexture.getSize().x, side / cubeTexture.getSize().y);
		doorSprite.setScale(side / doorTexture.getSize().x, side / doorTexture.getSize().y);
		cubeTxt.setCharacterSize(side/2.5f);
		window.setView(sf::View(sf::FloatRect(0.0f,0.0f,window.getSize().x,window.getSize().y)));

		while (window.isOpen()) {
			//Poll all events
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) {
					window.close();
				}
				else if(event.type == sf::Event::Resized){
					side = std::min((float)event.size.width / (float)level.m.size(), (float)event.size.height / (float)level.m[0].size());
					groundSprite.setScale(side / groundTexture.getSize().x, side / groundTexture.getSize().y);
					wallSprite.setScale(side / wallTexture.getSize().x, side / wallTexture.getSize().y);
					playerSprite.setScale(side / playerTexture.getSize().x, side / playerTexture.getSize().y);
					cubeSprite.setScale(side / cubeTexture.getSize().x, side / cubeTexture.getSize().y);
					doorSprite.setScale(side / doorTexture.getSize().x, side / doorTexture.getSize().y);
					cubeTxt.setCharacterSize(side/2.5f);
					window.setView(sf::View(sf::FloatRect(0.0f,0.0f,event.size.width,event.size.height)));
				}
				else if (event.type == sf::Event::KeyPressed) {
					if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
						level.move(level.player.pos, { -1,0 });
					}
					if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
						level.move(level.player.pos, { 1,0 });
					}
					if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
						level.move(level.player.pos, { 0,-1 });
					}
					if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
						level.move(level.player.pos, { 0,1 });
					}
				}
			}

			window.clear();
			//draw walls and ground
			for (int y = 0; y < (int)level.m.size(); ++y) {
				for (int x = 0; x < (int)level.m[y].size(); ++x) {
					if (level.m[y][x] == '.') {
						groundSprite.setPosition((float)x*side,(float)y*side);
						window.draw(groundSprite);
					}
					else if (level.m[y][x] == '#') {
						wallSprite.setPosition((float)x*side,(float)y*side);
						window.draw(wallSprite);
					}
					else if (level.m[y][x] == 'D') {
						doorSprite.setPosition((float)x*side,(float)y*side);
						window.draw(doorSprite);
					}
				}
			}
			//draw cubes
			for (auto& i : level.cubes) {
				cubeSprite.setPosition((float)i.pos.x*side,(float)i.pos.y*side);
				cubeSprite.setColor(i.col);
				window.draw(cubeSprite);
				cubeTxt.setString(std::to_string(i.n));
				cubeTxt.setOrigin((float)cubeTxt.getGlobalBounds().width/2.0f, (float)cubeTxt.getGlobalBounds().height/2.0f);
				cubeTxt.setPosition(((float)i.pos.x + 0.45f)*side,((float)i.pos.y + 0.4f)*side);
				window.draw(cubeTxt);
			}
			//draw player
			playerSprite.setPosition((float)level.player.pos.x*side,(float)level.player.pos.y*side);
			window.draw(playerSprite);
			window.display();

			if(level.m[level.player.pos.y][level.player.pos.x]=='D'){
				std::cout << "you won!" << std::endl;
				if(ln>=ll){
					out.open("res/save.dat");
					out<<ln+1<<std::endl;
					out.close();
				}
				window.close();
			}
		}
	}
	return 0;
}
