/*
Copyright (C) 2008  Mathias Gottschlag

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

#include "graphics/Light.h"
#include "graphics/GraphicsEngine.h"

#include <Horde3D.h>

namespace peak
{
	Light::Light(SceneNode *parent)
	{
		node = Horde3D::addLightNode(RootNode, "Light1", 0, "LIGHTING", "SHADOWMAP" );
		Horde3D::setNodeTransform(node, 0, 20, 0, 0, 0, 0, 1, 1, 1);
		Horde3D::setNodeParamf(node, LightNodeParams::Radius, 50.0f);
		if (parent)
		{
			setParent(parent);
		}
	}
	Light::~Light()
	{
	}
	
	void Light::setType(LightType type)
	{
	}
	LightType Light::getType(void)
	{
	}
}

