#pragma once

#include "Infrastructure.hpp"
#include "Graphics.hpp"
#include <glm\common.hpp>
#include "Physics.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;

struct Particle {
	Rect sprite;
	glm::dvec2 velocity;
	glm::dvec2 a;
	double mass;
};

class NBodyApp : public Application {

public:
	SpriteBatch* sp;
	std::vector<Particle> particles;

	static const int PARTICLE_COUNT = 100;
	static const double G;

	double scale;

	NBodyApp();

	~NBodyApp();

	void init() override;

	void loadResources() override;

	void setupScene() override;

	void cleanUp() override;

	void input() override;

	void update(TimeStep time) override;

	void render() override;
};

const double NBodyApp::G = 6.67384e-11;

NBodyApp::NBodyApp() {
	this->window = std::make_shared<Window>(800, 600, "NBody");
	sp = nullptr;
	particles.reserve(PARTICLE_COUNT+1);
	scale = 1;
}

NBodyApp::~NBodyApp() {
	if(sp) delete sp;
}

void NBodyApp::init() {
	sp = new SpriteBatch();
	Services::getGraphicsDevice()->setClearColor(Color(255, 255, 255, 255));
	srand(time(0));
}

void NBodyApp::loadResources() {

}

void NBodyApp::setupScene() {
	int max_r = 150, min_r = 20;
	Particle cp;
	cp.sprite.x = window->getWidth() / 2;
	cp.sprite.y = window->getHeight() / 2;
	cp.mass = 100;
	cp.sprite.width = 10;
	cp.sprite.height = 10;

	cp.velocity = glm::dvec2(0);
	cp.a = glm::dvec2(0);
	particles.push_back(cp);

	for (int i = 1; i < PARTICLE_COUNT; ++i) {
		Particle p;

		p.mass = rand() % 3;

		auto rad = min_r + rand() % max_r;
		auto rr = rand();
		auto angle = (rr % 360)*0.0174533;

		p.sprite.x = window->getWidth() / 2 + rad * cos(angle);
		p.sprite.y = window->getHeight() / 2 + rad * sin(angle);

		p.sprite.width = 5;
		p.sprite.height = 5;

		auto xdist = p.sprite.x - cp.sprite.x;
		auto ydist = p.sprite.y - cp.sprite.y;

		auto r = sqrt(xdist*xdist + ydist*ydist);

		auto ve = sqrt(NBodyApp::G * cp.mass / r);


		p.velocity = glm::vec2(ve * ydist/r, ve*-xdist/r);

		p.a = glm::vec2(0, 0);

		particles.push_back(p);
	}
}

void NBodyApp::cleanUp() {

}

void NBodyApp::input() {
	auto mouse_pos = Mouse::getPosition();

	if (Mouse::getButton(Mouse::Left_Button) == Mouse::State_Down) {
		
		Particle p;

		p.mass = rand()%3;

		p.sprite.x = mouse_pos.x;
		p.sprite.y = mouse_pos.y;

		p.sprite.width = 5;
		p.sprite.height = 5;

		p.velocity = glm::vec2(0, 0);
		p.a = glm::vec2(0, 0);

		particles.push_back(p);
		
	}

	if (Keyboard::getKey(Keyboard::Equals) == Mouse::State_Down) {
		scale *= 2;
		cout << scale << endl;
	}

	if (Keyboard::getKey(Keyboard::Minus) == Mouse::State_Down) {
		scale /= 2;
		cout << scale << endl;
	}
}

void NBodyApp::update(TimeStep time) {
	//cout << particles.size() << endl;

	glm::dvec2 F;
	double xdist, ydist;
	for (int i = 1; i < particles.size(); ++i) {
		particles[i].a = glm::dvec2(0);

		for (int j = 0; j < particles.size(); ++j) {
			if (i != j) {
				xdist = particles[i].sprite.x - particles[j].sprite.x;
				ydist = particles[i].sprite.y - particles[j].sprite.y;

				double r = max(sqrt(xdist*xdist + ydist*ydist),1.0);

				F.x = NBodyApp::G * particles[j].mass / (r*r);
				F.y = F.x;

				particles[i].a.x -= F.x * (xdist / r);
				particles[i].a.y -= F.y * (ydist / r);
			}
		}
	}


	for (int i = 0; i < particles.size(); ++i) {
		particles[i].velocity += particles[i].a * time.delta*scale;

		particles[i].sprite.x += particles[i].velocity.x*time.delta*scale;
		particles[i].sprite.y += particles[i].velocity.y*time.delta*scale;
	}
}

void NBodyApp::render() {
	sp->begin();
	int i = 0;
	for (auto particle : particles) {
		if(i == 0)
			sp->draw(NULL, particle.sprite, 0, glm::vec2(0,0), Color(0, 0, 0, 255));
		else
			sp->draw(NULL, particle.sprite, 0, glm::vec2(0, 0), Color(255, 0, 0, 255));
		i++;
	}
	sp->end();
}