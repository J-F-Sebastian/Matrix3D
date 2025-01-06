#include "m3d_renderer_wireframe.hh"
/*
 * WIREFRAME RENDERING
 */

void m3d_renderer_wireframe::render(m3d_world &world)
{
	m3d_point temp;
	m3d_color ctemp;
	SDL_Point toscreen[M3D_MAX_TRIANGLES * 3];
	unsigned i, j, k;

	// Compute visible objects
	compute_visible_list_and_sort(world);

	// Fill the surface black
	display->clear_renderer();
	for (auto itro : vislist)
	{
		ctemp = itro->color;
		display->set_color(ctemp.getChannel(m3d_color::R_CHANNEL),
				   ctemp.getChannel(m3d_color::G_CHANNEL),
				   ctemp.getChannel(m3d_color::B_CHANNEL));

		i = k = 0;
		for (auto &triangle : itro->mesh)
		{
			if (itro->trivisible[i++])
			{
				for (j = 0; j < 3; j++)
				{
					toscreen[k++] = itro->vertices[triangle.index[j]].scrposition;
				}
				toscreen[k] = toscreen[k - 3];
				k++;
			}
		}
		display->draw_lines(toscreen, k);
	}

	// Present the rendered lines
	display->show_renderer();
}
