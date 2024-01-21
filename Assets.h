#pragma once
#include <iostream>
#include <fstream>

#include "SFML/Graphics.hpp"
#include "Animation.h"

class Assets
{
	std::map<std::string, sf::Texture>		m_textures;
	std::map<std::string, Animation>        m_animations;
	//std::map<std::string, sf::Sound>        m_sounds;
	std::map<std::string, sf::Font>         m_fonts;

	

public:
	Assets();
	// TODO need to work out this file to load assets - use the spec to work out how to load them
	// its going to need a map of files and names
	
	void addTexture(std::string name, std::string path);
	void addAnimation(std::string name, Animation animation);
	//void addSound(std::string name, std::string path);
	void addFont(std::string name, std::string path);
	
	sf::Texture& getTexture(std::string name);
	Animation& getAnimation(std::string name) ;
	// sf::Sound& getSound(std::string name);
	sf::Font& getFont(std::string font);

	void loadFromFile(std::string path);

};

