#include "variable_poisson.h"
#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/core/print_string.hpp"
#include <algorithm>
#include <cmath>

void UniformPoissonSampler2D::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("generate", "radius", "width", "height", "rejection_limit"), &UniformPoissonSampler2D::generate);
	godot::ClassDB::bind_method(D_METHOD("get_samples"), &UniformPoissonSampler2D::get_samples);
}

void UniformPoissonSampler2D::initialize() {
	is_generating = true;
	cell_length = radius / std::sqrt(2);

	cells_per_x = std::ceil(width / cell_length);
	cells_per_y = std::ceil(height / cell_length);

	int total_cells = cells_per_x * cells_per_y;

	spatial_lookup.resize(total_cells);
	active_list.resize(total_cells);
	
	// All grid cells start empty (expressed as -1)
	std::fill(spatial_lookup.begin(), spatial_lookup.end(), -1);
}

int UniformPoissonSampler2D::get_spatial_grid_index(Vector2 sample) {
	int dx = std::floor(sample.x / cell_length);
	int dy = std::floor(sample.y / cell_length);

	return (dx + (dy * cells_per_x));
}

void UniformPoissonSampler2D::add_sample(Vector2 sample) {
	int sample_index = samples_list.size();
	int spatial_index = get_spatial_grid_index(sample);

	samples_list.append(sample);
	active_list.emplace_back(sample_index);

	spatial_lookup[spatial_index] = sample_index;
}

bool UniformPoissonSampler2D::is_sample_out_of_bounds(Vector2 sample) {
	return sample.x < 0.0 || sample.x > width || sample.y < 0.0 || sample.y > height;
}

bool UniformPoissonSampler2D::is_sample_near_others(Vector2 sample) {
	int prospective_cell = get_spatial_grid_index(sample);

	if ((prospective_cell == -1) || spatial_lookup[prospective_cell] != -1) return true;

	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			int neighbor = prospective_cell + x + (y * cells_per_x);

			if (is_sample_near_sample_in_cell(neighbor, sample)) return true;
		}
	}

	return false;
}

bool UniformPoissonSampler2D::is_sample_near_sample_in_cell(int lookup_cell, Vector2 sample) {
	if (lookup_cell < 0 || lookup_cell >= spatial_lookup.size()) return false;

	int cell_sample_index = spatial_lookup[lookup_cell];

	if (cell_sample_index == -1) return false;

	return sample.distance_to(samples_list[cell_sample_index]) <= radius;
}

int UniformPoissonSampler2D::get_random_active_list_index() {
	return rng->randi_range(0, active_list.size());
}

Vector2 UniformPoissonSampler2D::generate_random_point_in_annulus(Vector2 point) {
	float min = radius;
	float max = radius * 2.0;

	float distance = rng->randf_range(min, max);
	float angle = rng->randf_range(0.0, M_PI * 2.0);

	return Vector2(
		point.x + std::cos(angle) * distance,
		point.y + std::sin(angle) * distance
	);
}

void UniformPoissonSampler2D::generate_first_point() {
	add_sample(Vector2(rng->randf_range(0.0, width), rng->randf_range(0.0, height)));
}

bool UniformPoissonSampler2D::generate(float radius, float width, float height, int rejection_limit) {
	rng.instantiate();

	this->radius = radius;
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
			Vector2 random_sample = generate_random_point_in_annulus(current_sample);

			if (!is_sample_out_of_bounds(random_sample) && !is_sample_near_others(random_sample)) {
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

Array UniformPoissonSampler2D::get_samples() {
	return samples_list;
}