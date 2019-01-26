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
sf::Color keyToCol(sf::Keyboard::Key k){
	if(k==sf::Keyboard::R)return {255,0,0};
	if(k==sf::Keyboard::G)return {0,255,0};
	if(k==sf::Keyboard::B)return {0,0,255};
	if(k==sf::Keyboard::Y)return {255,255,0};
	if(k==sf::Keyboard::M)return {255,0,255};
	if(k==sf::Keyboard::L)return {0,255,255};
	return {0,0,0};
}
class Level {
public:
	std::vector<std::vector<char> > m;
	Player player;
	std::vector<Cube> cubes;
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
std::ofstream& operator<<(std::ofstream& out, Level& lvl) {
	out<<lvl.m.size()<<' '<<lvl.m[0].size()<<'\n';
	for(auto& i:lvl.m){
		for (auto& j : i) {
			out<<j;
		}out<<'\n';
	}
	out << lvl.player.pos.x << ' ' << lvl.player.pos.y << '\n';
	out << lvl.cubes.size() << '\n';
	for (auto& i : lvl.cubes) {
		out << i.pos.x << ' ' << i.pos.y << ' ' << (int)i.col.r << ' ' << (int)i.col.g << ' ' << (int)i.col.b << ' ' << i.n << '\n';
	}
	return out;
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
	sf::Sprite groundSprite(groundTexture), wallSprite(wallTexture), playerSprite(playerTexture), cubeSprite(cubeTexture), doorSprite(doorTexture), cursorSprite(cubeTexture);
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
		} while (ln > ll+1 || ln<0);

		in.open("res/levels/level"+std::to_string(ln)+".txt");
		if(!in.good()){
			in.close();
			out.open("res/levels/level"+std::to_string(ln)+".txt");
			out<<16<<' '<<16<<'\n';
			for(int y=0; y<16; ++y){
				for(int x=0; x<16; ++x){
					out<<'.';
				}out<<'\n';
			}
			out<<1<<' '<<1<<'\n'<<0<<std::endl;
			out.close();
			in.open("res/levels/level"+std::to_string(ln)+".txt");
		}
		in >> level;
		in.close();

		sf::RenderWindow window(sf::VideoMode(512,512), "Level " + std::to_string(ln));
		window.setFramerateLimit(30);
		window.setKeyRepeatEnabled(0);
		float side = std::min(window.getSize().y / level.m.size(), window.getSize().x / level.m[0].size());
		groundSprite.setScale({ side / groundTexture.getSize().x, side / groundTexture.getSize().y });
		wallSprite.setScale({ side / wallTexture.getSize().x, side / wallTexture.getSize().y });
		playerSprite.setScale({ side / playerTexture.getSize().x, side / playerTexture.getSize().y });
		cubeSprite.setScale({ side / cubeTexture.getSize().x, side / cubeTexture.getSize().y });
		doorSprite.setScale({ side / doorTexture.getSize().x, side / doorTexture.getSize().y });

		cursorSprite.setScale({side/(cubeTexture.getSize().x*2.0f),side/(cubeTexture.getSize().y*2.0f)});

		cubeTxt.setCharacterSize(side/2.5f);

		sf::Vector2i cursor = {0,0};

		while (window.isOpen()) {
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) {
					window.close();
				}
				else if (event.type == sf::Event::KeyPressed) {
					if (event.key.code == sf::Keyboard::Left) {
						if(cursor.x)--cursor.x;
					}
					if (event.key.code == sf::Keyboard::Right) {
						if(cursor.x<(int)level.m[0].size()-1)++cursor.x;
					}
					if (event.key.code == sf::Keyboard::Up) {
						if(cursor.y)--cursor.y;
					}
					if (event.key.code == sf::Keyboard::Down) {
						if(cursor.y<(int)level.m.size()-1)++cursor.y;
					}
					if (event.key.code == sf::Keyboard::Q) {
						level.m[cursor.y][cursor.x]='.';
					}
					if (event.key.code == sf::Keyboard::W) {
						level.m[cursor.y][cursor.x]='#';
					}
					if (event.key.code == sf::Keyboard::E) {
						level.m[cursor.y][cursor.x]='D';
					}
					if (event.key.code == sf::Keyboard::P){
						level.player.pos=cursor;
					}
					if (event.key.code == sf::Keyboard::Add){
						for(auto & i:level.cubes){
							if(i.pos==cursor)i.n++;
						}
					}
					if (event.key.code == sf::Keyboard::Hyphen || event.key.code == sf::Keyboard::Subtract){
						for(auto & i:level.cubes){
							if(i.pos==cursor)i.n--;
						}
					}
					if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Delete){
						for(auto i = level.cubes.begin(); i!=level.cubes.end(); ++i){
							if(i->pos==cursor){
								if(next(i)!=level.cubes.end()){
									std::swap(*i,level.cubes.back());
								}
								level.cubes.pop_back();
								break;
							}
						}
					}
					sf::Color c = keyToCol(event.key.code);
					if(c!=sf::Color(0,0,0)){
						bool f=0;
						for(auto & i:level.cubes){
							if(i.pos.x==cursor.x&&i.pos.y==cursor.y){
								f=1;
								i.col=c;
								break;
							}
						}
						if(!f){
							level.cubes.push_back({{cursor.x,cursor.y},c,0});
						}
					}
				}
			}
			window.clear();
			//draw walls and ground
			for (int y = 0; y < (int)level.m.size(); ++y) {
				for (int x = 0; x < (int)level.m[y].size(); ++x) {
					if (level.m[y][x] == '.') {
						groundSprite.setPosition({ x*side,y*side });
						window.draw(groundSprite);
					}
					else if (level.m[y][x] == '#') {
						wallSprite.setPosition({ x*side,y*side });
						window.draw(wallSprite);
					}
					else if (level.m[y][x] == 'D') {
						doorSprite.setPosition({ x*side,y*side });
						window.draw(doorSprite);
					}
				}
			}
			//draw cubes
			for (auto& i : level.cubes) {
				cubeSprite.setPosition({ i.pos.x*side,i.pos.y*side });
				cubeSprite.setColor(i.col);
				window.draw(cubeSprite);
				cubeTxt.setString(std::to_string(i.n));
				cubeTxt.setOrigin(cubeTxt.getGlobalBounds().width/2.0f, cubeTxt.getGlobalBounds().height/2.0f);
				cubeTxt.setPosition({ ((float)i.pos.x + 0.45f)*side,((float)i.pos.y + 0.4f)*side });
				window.draw(cubeTxt);
			}
			//draw player
			playerSprite.setPosition({ level.player.pos.x*side,level.player.pos.y*side });
			window.draw(playerSprite);
			cursorSprite.setPosition({ cursor.x*side,cursor.y*side });
			window.draw(cursorSprite);
			window.display();
		}
		std::cout<<"Save level?(Y/n)"<<std::endl;
		char save='y';
		std::cin>>save;
		if(save!='n'){
			out.open("res/levels/level"+std::to_string(ln)+".txt");
			out<<level;
			out.close();
		}
	}
	return 0;
}
