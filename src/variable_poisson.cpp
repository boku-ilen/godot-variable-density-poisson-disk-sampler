#include "variable_poisson.h"
#include "godot_cpp/classes/random_number_generator.hpp"
#include <algorithm>
#include <cmath>

// For Windows
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

void VariablePoissonSampler2D::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("generate"), &VariablePoissonSampler2D::generate);
	godot::ClassDB::bind_method(D_METHOD("get_samples"), &VariablePoissonSampler2D::get_samples);
	godot::ClassDB::bind_method(D_METHOD("set_min_max_radius", "min_radius", "max_radius"), &VariablePoissonSampler2D::set_min_max_radius);
	godot::ClassDB::bind_method(D_METHOD("set_width_height", "width", "height"), &VariablePoissonSampler2D::set_width_height);
	godot::ClassDB::bind_method(D_METHOD("set_radius_callable", "get_radius_at"), &VariablePoissonSampler2D::set_radius_callable);
	godot::ClassDB::bind_method(D_METHOD("set_griddedness_callable", "get_griddedness_at"), &VariablePoissonSampler2D::set_griddedness_callable);
	godot::ClassDB::bind_method(D_METHOD("set_rejection_limit", "rejection_limit"), &VariablePoissonSampler2D::set_rejection_limit);
}

void VariablePoissonSampler2D::initialize() {
	spatial_grid = SpatialGrid2D<int>(width, height, min_radius, max_radius);
	active_list.reserve(spatial_grid.cells_per_x * spatial_grid.cells_per_y);
}

void VariablePoissonSampler2D::add_sample(Vector2 sample) {
	int sample_index = samples_list.size();

	samples_list.append(sample);
	active_list.emplace_back(sample_index);
}

int VariablePoissonSampler2D::get_random_active_list_index() {
	return rng->randi_range(0, active_list.size() - 1);
}

Vector2 VariablePoissonSampler2D::generate_random_point_in_annulus(Vector2 point, float radius, float griddedness) {
	float min = radius;
	float max = radius * (2.0 - griddedness * 0.5);

	float distance = rng->randf_range(min, max);
	float angle = rng->randf_range(0.0, M_PI * 2.0);

	if (griddedness > 0.0) {
		float quadrant = static_cast<float>(rng->randi_range(0, 3)) * (M_PI / 2.0);
		float deviation = rng->randfn(0.0, std::clamp((1.0 - griddedness) * 0.15, -M_PI / 4.0, M_PI / 4.0));

		angle = quadrant + deviation;
	}

	return Vector2(
		point.x + std::cos(angle) * distance,
		point.y + std::sin(angle) * distance
	);
}

void VariablePoissonSampler2D::generate_first_point() {
	add_sample(Vector2(rng->randf_range(0.0, width), rng->randf_range(0.0, height)));
}

bool VariablePoissonSampler2D::generate() {
	rng.instantiate();

	initialize();
	generate_first_point();

	// Main loop
	while (active_list.size() > 0) {
		bool sample_found = false;
		int active_index = get_random_active_list_index();

		Vector2 current_sample = samples_list[active_list[active_index]];

		for (int i = 0; i < rejection_limit; i++) {
			float radius = static_cast<float>(get_radius_at.call(current_sample.x, current_sample.y));
			float griddedness = get_griddedness_at.is_valid() ? static_cast<float>(get_griddedness_at.call(current_sample.x, current_sample.y)) : 0.0;

			Vector2 random_sample = generate_random_point_in_annulus(current_sample, radius, griddedness);

			if (spatial_grid.add_if_open(samples_list.size(), random_sample.x, random_sample.y, radius)) {
				add_sample(random_sample);
				sample_found = true;

				break;
			}
		}

		if (!sample_found) {
			// remove active_index from active_list
			active_list.erase(active_list.begin() + active_index);
		}
	}

	is_generating = false;
	return true;
}

Vector2 VariablePoissonSampler2D::generate_next_sample() {
	// TODO: Implement as a single step of the loop in `generate()`
	return Vector2(0.0, 0.0);
}

void VariablePoissonSampler2D::set_min_max_radius(float min_radius, float max_radius) {
	this->min_radius = min_radius;
	this->max_radius = max_radius;
}

void VariablePoissonSampler2D::set_width_height(float width, float height) {
	this->width = width;
	this->height = height;
}

void VariablePoissonSampler2D::set_radius_callable(Callable get_radius_at) {
	this->get_radius_at = get_radius_at;
}

void VariablePoissonSampler2D::set_griddedness_callable(Callable get_griddedness_at) {
	this->get_griddedness_at = get_griddedness_at;
}

void VariablePoissonSampler2D::set_rejection_limit(int rejetion_limit) {
	this->rejection_limit = rejetion_limit;
}

Array VariablePoissonSampler2D::get_samples() {
	return samples_list;
}