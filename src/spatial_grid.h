#pragma once

#include <vector>

template <class T>
class SpatialGrid2D {

public:
	float width;
    float height;
    float cell_length;
    int cells_per_x;
    int cells_per_y;

    SpatialGrid2D() = default;
    SpatialGrid2D(float width, float height, float min_radius, float max_radius);

    bool add(T item, float x, float y, float radius);
    bool add_if_open(T item, float x, float y, float radius);
    void remove(T item);
    void clear();
    bool is_open(float x, float y, float radius);
    std::vector<T> get_at(float x, float y, float radius);

private:
struct SpatialGridItem {
    int index;

    float x;
    float y;
    float radius;
};

struct SpatialCell {
    int offset_x;
    int offset_y;

    float origin_x;
    float origin_y;
    float dimension;

    std::vector<SpatialGridItem> contents;
    
    SpatialCell(int x, int y, float dimension);
    bool contains_point(float x, float y);
    bool intersects_cell(float x, float y, float radius);
    bool intersects_child(float x, float y, float radius);

    std::vector<SpatialGridItem> get_intersections(float x, float y, float radius);
};

    void _add(T item, int cell_index, float x, float y, float radius);
    bool _is_open(int center_index, float x, float y, float radius);
    int _get_index(float x, float y);

    std::vector<SpatialCell> grid_cells;
    std::vector<T> grid_items;
};