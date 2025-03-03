/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/



#ifndef __TARGA_TO_OPENGL__ 
#define __TARGA_TO_OPENGL__ 

#include <toolbox_stdafx.h>

GLuint LoadTexture(const char *filename);
void UnloadTexture ( GLuint texture);

const char* FindTextureById (GLuint textureID);

#endif


