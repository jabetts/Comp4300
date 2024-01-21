#include "Assets.h"

Assets::Assets()
{

}

void Assets::addTexture(std::string name, std::string path)
{
	sf::Texture t;
	t.loadFromFile(path);

	m_textures[name] = t;
}

void Assets::addAnimation(std::string name, Animation animation)
{
	m_animations[name] = animation;
}

void Assets::addFont(std::string name, std::string path)
{
	sf::Font f;
	f.loadFromFile(path);

	m_fonts[name] = f;
}

sf::Font& Assets::getFont(std::string font)
{

	return m_fonts[font];
}


sf::Texture& Assets::getTexture(std::string name)
{
	return m_textures[name];
}

Animation& Assets::getAnimation(std::string name) 
{
	return m_animations[name];
}

void Assets::loadFromFile(std::string path)
{
	std::ifstream f(path);

	if (!f.is_open())
	{
		std::cerr << "unable to open file: " << path << std::endl;
		exit(1);
	}

	std::string in;
	std::string p;
	while (f >> in)
	{
		if (in == "Texture")
		{
			f >> in >> p;
			addTexture(in, p);
		}

		if (in == "Font")
		{
			f >> in >> p;
			addFont(in, p);
		}
		if (in == "Animation")
		{
			int F, S;
			
			f >> in >> p >> F >> S;

			Animation anim(in, m_textures[p], F, S);
			addAnimation(in, anim);
		}
	}

	for (auto& [name, texture] : m_textures)
	{
		std::cout << "Loaded texture: " << name << " " << texture.getSize().x
			<< " " << texture.getSize().y << std::endl;
	}
	for (auto& [name, font] : m_fonts)
	{
		std::cout << "Loaded font: " << name << std::endl;
	}
	for (auto& [name, anim] : m_animations)
	{
		std::cout << "Loaded animation: " << name << 
			" " << anim.getSize().x << " " << anim.getSize().y << std::endl;
	}


	f.close();

}
