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

	/// Generate random samples with the previously set parameters.
	/// Requires that at least `set_min_max_radius`, `set_width_height`, and `set_radius_callable` have been called before.
	/// Returns whether the generation was successful.
	bool generate();

	/// Generate one random sample, instead of doing a full generation with `generate`.
	/// Useful if the generation process should be visualized or split into multiple frames.
	Vector2 generate_next_sample();

	/// Set the expected minimum and maximum radius which will be returned by `get_radius_at`.
	/// Not used for the point generation logic directly, but for the underlying spatial grid optimization.
	void set_min_max_radius(float min_radius, float max_radius);

	/// Points will be generated between 0,0 and width,height.
	void set_width_height(float width, float height);

	/// Set the function to call for getting a minimum radius between random points at a given x,y position.
	void set_radius_callable(Callable get_radius_at);

	/// Set the function to call for getting the griddedness of points at a given x,y position.
	/// The griddedness factor should be between 0.0 and 1.0. It controls whether points are placed entirely randomly (0.0) or
	/// if they closely follow a more artificial-looking grid (1.0), e.g. when generating points for plants within a plantation.
	void set_griddedness_callable(Callable get_griddedness_at);

	/// Set the maximum number of attempts to generate a new random point.
	void set_rejection_limit(int rejetion_limit);

	/// Get the samples generated after a `generate` call.
	Array get_samples();

private:
	void initialize();
    void generate_first_point();
    void add_sample(Vector2 sample);
    int get_random_active_list_index();
    Vector2 generate_random_point_in_annulus(Vector2 point, float radius, float griddedness);

	float min_radius = 0.0;
	float max_radius = 10.0;
	float width;
	float height;
	int rejection_limit = 30;

	bool is_generating;

	Array samples_list;

	SpatialGrid2D<int> spatial_grid;
	std::vector<int> active_list;

	Ref<RandomNumberGenerator> rng;

	Callable get_radius_at;
	Callable get_griddedness_at;
};
