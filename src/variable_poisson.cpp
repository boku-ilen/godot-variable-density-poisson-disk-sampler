#include "variable_poisson.h"
#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/core/print_string.hpp"
#include <algorithm>
#include <cmath>

void VariablePoissonSampler2D::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("generate", "radius", "width", "height", "rejection_limit"), &VariablePoissonSampler2D::generate);
	godot::ClassDB::bind_method(D_METHOD("get_samples"), &VariablePoissonSampler2D::get_samples);
}

void VariablePoissonSampler2D::initialize() {
	is_generating = true;

	spatial_grid = SpatialGrid2D<int>(width, height, min_radius, max_radius);
	active_list.resize(spatial_grid.cells_per_x * spatial_grid.cells_per_y);
}

void VariablePoissonSampler2D::add_sample(Vector2 sample) {
	int sample_index = samples_list.size();

	samples_list.append(sample);
	active_list.emplace_back(sample_index);
}

int VariablePoissonSampler2D::get_random_active_list_index() {
	return rng->randi_range(0, active_list.size());
}

Vector2 VariablePoissonSampler2D::generate_random_point_in_annulus(Vector2 point, float radius) {
	float min = radius;
	float max = radius * 2.0;

	float distance = rng->randf_range(min, max);
	float angle = rng->randf_range(0.0, M_PI * 2.0);

	return Vector2(
		point.x + std::cos(angle) * distance,
		point.y + std::sin(angle) * distance
	);
}

void VariablePoissonSampler2D::generate_first_point() {
	add_sample(Vector2(rng->randf_range(0.0, width), rng->randf_range(0.0, height)));
}

bool VariablePoissonSampler2D::generate(Callable get_radius_at, float min_radius, float max_radius, float width, float height, int rejection_limit) {
	rng.instantiate();

	this->get_radius_at = get_radius_at;
	this->min_radius = min_radius;
	this->max_radius = max_radius;
	this->width = width;
	this->height = height;
	this->rejection_limit = rejection_limit;

	if (is_generating) return false;

	initialize();
	generate_first_point();

	// Main loop
	while (active_list.size() > 0) {
		bool sample_found = false;
		int active_index = get_random_active_list_index();

		Vector2 current_sample = samples_list[active_list[active_index]];

		for (int i = 0; i < rejection_limit; i++) {
			float radius = get_radius_at.call(current_sample.x, current_sample.y);
			Vector2 random_sample = generate_random_point_in_annulus(current_sample, radius);

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

Array VariablePoissonSampler2D::get_samples() {
	return samples_list;
}