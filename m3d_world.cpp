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

void m3d_world::set_ambient_light(m3d_ambient_light &ambient_light)
{
    this->ambient_light = ambient_light;
}

void m3d_world::set_ambient_light_intensity(float intensity)
{
    ambient_light.set_src_intensity(intensity);
}

void m3d_world::sort(list<m3d_render_object *> &objects_list)
{
    list<m3d_render_object *>::iterator itro = objects_list.begin();
    m3d_point temp;

    while (itro != objects_list.end())
    {
        camera.transform_to_viewpoint((*itro)->center, temp);
        (*itro)->z_sorting = temp.myvector[Z_C];
        ++itro;
    }
    objects_list.sort(zsortobjects);
}

void m3d_world::print()
{
#ifdef NDEBUG
    list<m3d_render_object *>::iterator itro;
    list<m3d_point_light_source *>::iterator itls;

    cout << "World" << endl;
    for (itro = objects_list.begin(); itro != objects_list.end(); itro++)
    {
        (*itro)->print();
    }

    for (itls = lights_list.begin(); itls != lights_list.end(); itls++)
    {
        (*itls)->print();
    }
#endif
}
