#include <iostream>

#include "m3d_world.hh"

using namespace std;

static bool zsortobjects(const m3d_render_object *first,
			 const m3d_render_object *second)
{
	return (first->z_sorting < second->z_sorting) ? true : false;
}

m3d_world::~m3d_world()
{
	objects_list.clear();
	lights_list.clear();
}

int m3d_world::add_object(m3d_render_object &object)
{
	try
	{
		objects_list.push_back(&object);
	}
	catch (const bad_alloc &e)
	{
		cerr << "Exception: " << e.what() << endl;
		return (ENOMEM);
	}

	return (0);
}

int m3d_world::add_light_source(m3d_point_light_source &light_source)
{
	try
	{
		lights_list.push_back(&light_source);
	}
	catch (const bad_alloc &e)
	{
		cerr << "Exception: " << e.what() << endl;
		return (ENOMEM);
	}

	return (0);
}

void m3d_world::set_ambient_light(m3d_ambient_light &_ambient_light)
{
	ambient_light = _ambient_light;
}

void m3d_world::set_ambient_light_intensity(float intensity)
{
	ambient_light.set_src_intensity(intensity);
}

void m3d_world::sort(list<m3d_render_object *> &_objects_list)
{
	m3d_point temp;

	for (auto itro : _objects_list)
	{
		camera.transform_to_viewpoint(itro->center, temp);
		itro->z_sorting = temp.myvector[Z_C];
	}
	_objects_list.sort(zsortobjects);
}

void m3d_world::print()
{
#ifdef DEBUG
	cout << "World" << endl;
	for (auto itro : objects_list)
	{
		itro->print();
	}

	for (auto itls : lights_list)
	{
		itls->print();
	}
#endif
}
