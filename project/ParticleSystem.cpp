#include "ParticleSystem.h"
#include <algorithm>
#include <labhelper.h>

using namespace glm;

ParticleSystem::ParticleSystem(int capacity) : max_size(capacity)
{
	gl_data_temp_buffer.resize(max_size);
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::init_gpu_data()
{
	glGenVertexArrays(1, &gl_vao);
	glBindVertexArray(gl_vao);

	glGenBuffers(1, &gl_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer);
	glBufferData(GL_ARRAY_BUFFER, max_size * sizeof(float), nullptr, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
}

void ParticleSystem::process_particles(float dt)
{
	for (Particle &particle : particles) {
		// Update alive particles!
		//particle.velocity.y -= 9.82f * dt;
		particle.pos += particle.velocity * dt;
		particle.lifetime += dt;
	}

	for (unsigned i = 0; i < particles.size(); ++i) {
		// Kill dead particles!
		if (particles[i].lifetime > particles[i].life_length)
			kill(i);
	}
}

void ParticleSystem::submit_to_gpu(const glm::mat4& viewMat)
{
	unsigned int num_active_particles = particles.size();

	gl_data_temp_buffer.clear();
	for (const Particle &particle : particles) {
		vec4 pos = viewMat * vec4(particle.pos, particle.lifetime / particle.life_length);
		gl_data_temp_buffer.push_back(pos);
	}

	// sort particles with sort from c++ standard library
	std::sort(gl_data_temp_buffer.begin(), std::next(gl_data_temp_buffer.begin(), num_active_particles),
		[](const vec4& lhs, const vec4& rhs) { return lhs.z < rhs.z; });

	glBindVertexArray(gl_vao);
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * num_active_particles, gl_data_temp_buffer.data());

	glDrawArrays(GL_POINTS, 0, num_active_particles);
}

void ParticleSystem::spawn(Particle particle) {
	if (particles.size() < max_size) {
		particles.push_back(particle);
	}
};

void ParticleSystem::kill(int id) {
	std::swap(particles[id], particles[particles.size() - 1]);
	particles.pop_back();
};
