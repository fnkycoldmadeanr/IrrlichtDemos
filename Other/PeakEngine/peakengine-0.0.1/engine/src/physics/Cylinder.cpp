/*
Copyright (C) 2008  Mathias Gottschlag, Lukas Kropatschek

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "physics/Cylinder.h"
#include "physics/World.h"

namespace peak
{
	Cylinder::Cylinder() : Geometry()
	{
	}
	Cylinder::~Cylinder()
	{
	}
	
	bool Cylinder::init(World *world, float radius, float length)
	{
		transfgeom = dCreateGeomTransform(world->getSpace());
		dGeomSetData(transfgeom, this);
		geom = dCreateCylinder(0, radius, length);
		dGeomTransformSetGeom(transfgeom, geom);
		dGeomSetData(geom, this);
		return true;
	}
	bool Cylinder::init(World *world, float radius, float length, Quaternion rot)
	{
		transfgeom = dCreateGeomTransform(world->getSpace());
		dGeomSetData(transfgeom, this);
		geom = dCreateCylinder(0, radius, length);
		dGeomTransformSetGeom(transfgeom, geom);
		dGeomSetQuaternion(geom, rot.q);
		dGeomSetData(transfgeom, this);
		return true;
	}

	bool Cylinder::destroy(void)
	{
		dGeomDestroy(geom);
		dGeomDestroy(transfgeom);
		return true;
	}
}

