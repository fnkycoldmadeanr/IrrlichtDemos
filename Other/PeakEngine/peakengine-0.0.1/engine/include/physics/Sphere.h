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

#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Geometry.h"

//tolua_begin
namespace peak
{
	/**
	 * \brief Class representing a solid sphere.
	 */
	class Sphere: public Geometry
	{
		public:
			Sphere();
			virtual ~Sphere();
			
			/**
			 * \brief Creates the sphere based on the given radius.
			 * \param world World to playe cube into
			 * \param radius Radius of the sphere
			 * \return Returns false if the sphere could not be created.
			 */
			bool init(World *world, float radius);
			/**
			 * \brief Destroys the sphere and removes it from the world.
			 * \return Returns false if an error occurred.
			 */
			virtual bool destroy(void);
		private:
			//tolua_end
			//tolua_begin
	};
}
//tolua_end

#endif

