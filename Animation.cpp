#include "Animation.h"
#include "components.h"
#include <cmath>
#include <iostream>

Animation::Animation()
{
}

Animation::Animation(const std::string& name, const sf::Texture& t)
	:Animation(name, t, 1, 0)
{
}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
	: m_name(name), m_sprite(t), m_frameCount(frameCount), m_currentFrame(0), m_speed(speed)
{
	m_size = Vec2((float)t.getSize().x / frameCount, (float)t.getSize().y);

	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);

	m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
	
}

// updates the animation to show the next frame, depending on its speed
// animation loop when it reaches the end
void Animation::update()
{
	// TODO: 1) calculate the correct frame of animation to play based on currentFrame and speed
	//       2) set the texture rectangle property (see constructor for sample)
	
	/*
	// Below commented section is changing m_framecount in 60 frames.
	if (m_currentFrame % (60 / m_frameCount))
	{
		size_t frame = (m_currentFrame / (60 / m_frameCount)) % m_frameCount;

		m_sprite.setTextureRect(sf::IntRect(std::floor(frame) * m_size.x, 0, m_size.x, m_size.y));
	}
	*/

	if ((m_speed > 0) && m_currentFrame % m_speed == 0)
	{
		size_t frame = (m_currentFrame / m_speed) % m_frameCount;

		m_sprite.setTextureRect(sf::IntRect(std::floor(frame) * m_size.x, 0, m_size.x, m_size.y));
		
	}
	m_currentFrame++;
	
}

bool Animation::hasEnded() const
{
	//if (m_frameCount > 0)
		if (m_currentFrame  >= m_frameCount * m_speed) return true;
	return false;
}

const std::string& Animation::getName() const
{
	return m_name;
}

const Vec2& Animation::getSize() const
{
	return m_size;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}
