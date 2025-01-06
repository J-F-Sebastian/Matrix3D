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

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

#include "m3d_vertex.hh"

m3d_vertex::m3d_vertex(const m3d_vertex &other)
    : position(other.position), normal(other.normal), tposition(other.tposition), tnormal(other.tnormal), prjposition(other.prjposition), prjnormal(other.prjnormal), scrposition(other.scrposition)
{
}

void m3d_vertex::print()
{
#ifdef DEBUG
	cout << "* Vertex" << endl
	     << "  Position ";
	position.print();
	cout << "  Normal   ";
	normal.print();
	cout << "  Position, transformed";
	tposition.print();
	cout << "  Normal, transformed   ";
	tnormal.print();
	cout << "  Projected position ";
	prjposition.print();
	cout << "  Projected normal   ";
	prjnormal.print();
	cout << "  Screen (" << scrposition.x << "," << scrposition.y << ")" << endl;
#endif
}
