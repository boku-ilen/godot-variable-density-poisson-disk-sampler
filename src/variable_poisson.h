#pragma once

#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"

#include "spatial_grid.h"

using namespace godot;

class VariablePoissonSampler2D : public RefCounted {
	GDCLASS(VariablePoissonSampler2D, RefCounted)

protected:
	static void _bind_methods();

public:
	VariablePoissonSampler2D() = default;
	~VariablePoissonSampler2D() override = default;

	bool generate(Callable get_radius_at, float min_radius, float max_radius, float width, float height, int rejection_limit);

	Array get_samples();

private:
	void initialize();
    void generate_first_point();
    void add_sample(Vector2 sample);
    int get_random_active_list_index();
    Vector2 generate_random_point_in_annulus(Vector2 point, float radius);

	float min_radius;
	float max_radius;
	float width;
	float height;
	float rejection_limit;

	bool is_generating;

	Array samples_list;

	SpatialGrid2D<int> spatial_grid;
	std::vector<int> active_list;

	Ref<RandomNumberGenerator> rng;

	Callable get_radius_at;
};
