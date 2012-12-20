#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class FreeLookCamera
{
public:
	FreeLookCamera(float left, float up, glm::vec3 location, size_t width, size_t height)
	: left(left)
	, up(up)
	, location(location)
	, width(width)
	, height(height)
	{
		sf::Mouse::setPosition(sf::Vector2i(width/2, height/2));
		orientation = glm::quat(glm::vec3(left, up, 0.0f));
	}
	glm::mat4 GetViewMatrix() 
	{
		return glm::toMat4(-orientation) * glm::translate(glm::mat4(1.0f), -location);
	}
	void keyboard(float left, float up)
	{
		glm::vec4 translated = glm::vec4(left, 0.0f, up, 1.0f) * glm::toMat4(orientation);
		location += glm::vec3(translated.x, translated.y, translated.z);
	}
	void mouse(float x, float y) 
	{
		up += y * 0.001f;
		if (up > 1.5) up = 1.5;
		if (up < -1.5) up = -1.5;
		left += x * 0.001f;
		orientation = glm::quat(glm::vec3(up, 0, 0)) * glm::quat(glm::vec3(0, left, 0));
	}
	void Update()
	{
		float keyboardx = 0.0f;
		float keyboardy = 0.0f;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) keyboardy -= 0.2f;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) keyboardx -= 0.2f;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) keyboardy += 0.2f;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) keyboardx += 0.2f;
		sf::Vector2i mousepos = sf::Mouse::getPosition();
		int mousex = (width/2) - mousepos.x;
		int mousey = (height/2) - mousepos.y;
		mouse(-mousex, -mousey);
		keyboard(keyboardx, keyboardy);
		sf::Mouse::setPosition(sf::Vector2i(width/2, height/2));
	}
private:
	glm::quat orientation;
	float left, up;
	glm::vec3 location;
	size_t width, height;
};

#endif

