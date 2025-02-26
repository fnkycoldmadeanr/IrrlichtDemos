// Copyright NVIDIA Corporation 2002-2005
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

#pragma once
/** \file */

#include "nvsg/NVSGVersion.h"

//! A string define serving for version safty
#define UPITID_VERSION        NVSG_VER_MAJOR_STR "." NVSG_VER_MINOR_STR

#define UPITID_SCENE_LOADER   0x0001    //!<  Unique Plug-In Type ID for Scene loaders
#define UPITID_SCENE_SAVER    0x0002    //!<  Unique Plug-In Type ID for Scene savers
#define UPITID_TEXTURE_LOADER 0x0004    //!<  Unique Plug-In Type ID for texture loaders (not yet implemented)
#define UPITID_TEXTURE_SAVER  0x0008    //!<  Unique Plug-In Type ID for texture savers
