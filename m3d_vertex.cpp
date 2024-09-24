#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

#include "m3d_vertex.hh"

m3d_vertex::m3d_vertex(const m3d_vertex &other)
    : position(other.position), normal(other.normal), prjposition(other.prjposition), camnormal(other.camnormal), scrposition(other.scrposition)
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
	cout << "  Projected position ";
	prjposition.print();
	cout << "  Projected normal   ";
	camnormal.print();
	cout << "  Screen (" << scrposition.x << "," << scrposition.y << ")" << endl;
#endif
}
