#include <iostream>

#include "m3d_vertex.hh"

using namespace std;

m3d_vertex::m3d_vertex(const m3d_vertex &other) : position(other.position), normal(other.normal)
{
}

void m3d_vertex::print()
{
#ifdef DEBUG
	cout << "Vertex" << endl
	     << "   +-> Position ";
	position.print();
	cout << "   +-> Normal ";
	normal.print();
#endif
}
