// *************************************************************************************************
// Horde3D Shader Utility Library
// --------------------------------------
//		- Deferred shading functions -
//
// Copyright (C) 2006-2008 Nicolas Schulz
//
// You may use the following code in projects based on the Horde3D graphics engine.
//
// *************************************************************************************************

#extension GL_ARB_draw_buffers : enable

void setMatID( const float id ) { gl_FragData[0].a = id; }
void setPos( const vec3 pos ) { gl_FragData[0].rgb = pos; }
void setNormal( const vec3 normal ) { gl_FragData[1].rgb = normal; }
void setAlbedo( const vec3 albedo ) { gl_FragData[2].rgb = albedo; }
void setSpecMask( const float alpha ) { gl_FragData[2].a = alpha; }