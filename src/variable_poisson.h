#pragma once

#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"

using namespace godot;

class UniformPoissonSampler2D : public RefCounted {
	GDCLASS(UniformPoissonSampler2D, RefCounted)

protected:
	static void _bind_methods();

public:
	UniformPoissonSampler2D() = default;
	~UniformPoissonSampler2D() override = default;

	bool generate(float radius, float width, float height, int rejection_limit);

	Array get_samples();

private:
	void initialize();
    void generate_first_point();
    void add_sample(Vector2 sample);
    int get_spatial_grid_index(Vector2 sample);
    int get_random_active_list_index();
    Vector2 generate_random_point_in_annulus(Vector2 point);
    bool is_sample_out_of_bounds(Vector2 sample);
    bool is_sample_near_others(Vector2 sample);
    bool is_sample_near_sample_in_cell(int lookup_cell, Vector2 sample);

	float radius;
	float width;
	float height;
	float rejection_limit;

	bool is_generating;
	float cell_length;
	float cells_per_x;
	float cells_per_y;

	Array samples_list;

	std::vector<int> spatial_lookup;
	std::vector<int> active_list;

	Ref<RandomNumberGenerator> rng;
};
