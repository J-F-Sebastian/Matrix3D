/*
 * Matrix3D
 *
 * Copyright (C) 1995 - 2025 Diego Gallizioli
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef M3D_WORLD_H
#define M3D_WORLD_H

#include <list>

#include "m3d_object.hh"
#include "m3d_light_source.hh"
#include "m3d_camera.hh"

class m3d_world
{
public:
	/** Default constructor */
	m3d_world() : objects_list(), lights_list(), ambient_light(), camera() {};
	/** Default destructor */
	~m3d_world();

	m3d_world(const m3d_ambient_light &ambient, const m3d_camera &camera) : objects_list(), lights_list(), ambient_light(ambient), camera(camera) {};

	int add_object(m3d_render_object &object);
	int add_light_source(m3d_point_light_source &light_source);
	void set_ambient_light(m3d_ambient_light &_ambient_light);
	void set_ambient_light_intensity(float intensity);

	void sort(std::list<m3d_render_object *> &_objects_list);
	void print(void);

	std::list<m3d_render_object *> objects_list;
	std::list<m3d_point_light_source *> lights_list;
	m3d_ambient_light ambient_light;
	m3d_camera camera;
};

#endif // M3D_WORLD_H
