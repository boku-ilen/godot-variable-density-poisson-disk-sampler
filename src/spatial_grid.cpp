#include "spatial_grid.h"
#include <cmath>

template <class T>
SpatialGrid2D<T>::SpatialGrid2D(float width, float height, float min_radius, float max_radius) {
    this->width = width;
    this->height = height;

    cell_length = ((min_radius + max_radius) * 0.5) / std::sqrt(2);
    cells_per_x = std::ceil(width / cell_length);
    cells_per_y = std::ceil(height / cell_length);

    int total_cells = cells_per_x * cells_per_y;

    grid_cells = std::vector<SpatialCell>(total_cells);
    grid_items = std::vector<T>(total_cells);

    for (int y = 0; y < cells_per_y; ++y) {
        for (int x = 0; x < cells_per_x; ++x) {
            grid_cells.add(SpatialCell(x, y, cell_length));
        }
    }
}

template <class T>
bool SpatialGrid2D<T>::add(T item, float x, float y, float radius) {
    int index = _get_index(x, y);

    if (index == -1) return false;

    _add(item, index, x, y, radius);

    return true;
}

template <class T>
bool SpatialGrid2D<T>::add_if_open(T item, float x, float y, float radius) {
    int index = _get_index(x, y);

    if (!_is_open(index, x, y, radius)) return false;

    _add(item, index, x, y, radius);

    return true;
}

template <class T>
void SpatialGrid2D<T>::_add(T item, int cell_index, float x, float y, float radius) {
    int cell_radius = std::ceil(radius / cell_length);

    int index = grid_items.size();
    grid_items.emplace_back(item);

    SpatialGridItem spatial_item(index, x, y, radius);

    for (int iy = -cell_radius; iy <= cell_radius; iy++) {
        for (int ix = -cell_radius; ix <= cell_radius; ix++) {
            int neighbor = cell_index + ix + (iy * cells_per_x);

            if (neighbor < 0 || neighbor >= grid_cells.size()) {
                continue;
            }

            if (grid_cells[neighbor].intersects_cell(x, y, radius)) {
                grid_cells[neighbor].contents.add(spatial_item);
            }
        }
    }
}

template <class T>
void SpatialGrid2D<T>::remove(T item) {
    int index = 0;
    int grid_items_size = grid_items.size();

    // Remove reference to item in grid_items
    while (index < grid_items_size) {
        if (grid_items[index] == item) {
            grid_items.erase(index);
            break;
        }

        index++;
    }

    // Remove index reference in all other cells
    for (SpatialCell cell : grid_cells) {
        for (int j = (cell.contents.size() - 1); j >= 0; j--) {
            if (cell.contents[j].index == index) {
                cell.contents.erase(j);
            }
        }
    }
}

template <class T>
void SpatialGrid2D<T>::clear() {
    grid_items.clear();

    for(SpatialCell cell : grid_cells) {
        cell.contents.clear();
    }
}

template <class T>
bool SpatialGrid2D<T>::is_open(float x, float y, float radius) {
    return _is_open(_get_index(x, y), x, y, radius);
}

template <class T>
bool SpatialGrid2D<T>::_is_open(int center_index, float x, float y, float radius) {
    if (center_index == -1) return false;

    int cell_radius = std::ceil(radius / cell_length);

    // Check if the circle is too close to any in the prospective cell and the neighboring cells.
    for (int iy = -cell_radius; iy <= cell_radius; ++iy) {
        for (int ix = -cell_radius; ix <= cell_radius; ++ix) {
            int neighbor = center_index + ix + (iy * cells_per_x);

            if (neighbor < 0 || (neighbor >= grid_cells.size()))
            {
                continue;
            }

            if (grid_cells[neighbor].intersects_cell(x, y, radius) &&
                grid_cells[neighbor].intersects_child(x, y, radius)) {
                return false;
            }
        }
    }

    return true;
}

template <class T>
int SpatialGrid2D<T>::_get_index(float x, float y) {
    if ((x < 0.0) || (x > width) || (y < 0.0) || (y > height)) return -1;

    int dx = std::floor(x / cell_length);
    int dy = std::floor(y / cell_length);

    return (dx + (dy * cells_per_x));
}

template <class T>
std::vector<T> SpatialGrid2D<T>::get_at(float x, float y, float radius) {
    std::vector<T> results;

    int cell_index = _get_index(x, y);

    if (cell_index != -1) {
        std::vector<SpatialGridItem> items = grid_cells[cell_index].get_intersections(x, y, radius);

        for (int i = 0; i < items.size(); i++) {
            results.emplace_back(grid_items[items[i].index]);
        }
    }

    return results;
}

template <class T>
SpatialGrid2D<T>::SpatialCell::SpatialCell(int x, int y, float dimension) {
    this->offset_x = x;
    this->offset_y = y;

    this->origin_x = static_cast<float>(offset_x) * dimension;
    this->origin_y = static_cast<float>(offset_y) * dimension;

    this->dimension = dimension;

    this->contents = std::vector<SpatialGridItem>();
}

template <class T>
bool SpatialGrid2D<T>::SpatialCell::contains_point(float x, float y) {
    return (x >= origin_x) && (x < (origin_x + dimension)) &&
            (y >= origin_y) && (y < (origin_y + dimension));
}

template <class T>
bool SpatialGrid2D<T>::SpatialCell::intersects_cell(float x, float y, float radius) {
    // Box Circle Intersection
    float circle_distance_x = std::abs(x - origin_x);
    float circle_distance_y = std::abs(y - origin_y);

    if (circle_distance_x > (dimension / 2.0 + radius)) return false;
    if (circle_distance_y > (dimension / 2.0 + radius)) return false;

    if (circle_distance_x <= (dimension / 2.0)) return true; 
    if (circle_distance_y <= (dimension / 2.0)) return true;

    float corner_distance_squared = std::pow((circle_distance_x - dimension / 2.0), 2.0) +
        std::pow((circle_distance_y - dimension / 2.0), 2.0);

    return (corner_distance_squared <= pow(radius, 2.0));
}

template <class T>
bool SpatialGrid2D<T>::SpatialCell::intersects_child(float x, float y, float radius) {
    for (int i = 0; i < contents.size(); i++) {
        if (pow((x - contents[i].x), 2.0) + pow((y - contents[i].y), 2.0) < pow(radius, 2.0)) {
            return true;
        }
    }

    return false;
}

template <class T>
std::vector<typename SpatialGrid2D<T>::SpatialGridItem> SpatialGrid2D<T>::SpatialCell::get_intersections(float x, float y, float radius) {
    std::vector<SpatialGridItem> intersections;

    for (int i = 0; i < contents.size(); i++) {
        if (pow((x - contents[i].x), 2.0) + pow((y - contents[i].y), 2.0) < pow(radius, 2.0)) {
            intersections.emplace_back(contents[i]);
        }
    }

    return intersections;
}