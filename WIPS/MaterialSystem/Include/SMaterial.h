// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_MATERIAL_H_INCLUDED__
#define __S_MATERIAL_H_INCLUDED__

#include "SColor.h"
#include "matrix4.h"
#include "irrArray.h"
#include "EMaterialTypes.h"
#include "EMaterialFlags.h"
#include "SMaterialLayer.h"
#include "IMaterialSystem.h"

namespace irr
{
namespace video
{
	class ITexture;

	//! Flag for EMT_ONETEXTURE_BLEND, ( BlendFactor ) BlendFunc = source * sourceFactor + dest * destFactor
	enum E_BLEND_FACTOR
	{
		EBF_ZERO	= 0,		//!< src & dest	(0, 0, 0, 0)
		EBF_ONE,			//!< src & dest	(1, 1, 1, 1)
		EBF_DST_COLOR, 			//!< src	(destR, destG, destB, destA)
		EBF_ONE_MINUS_DST_COLOR, 	//!< src	(1-destR, 1-destG, 1-destB, 1-destA)
		EBF_SRC_COLOR,			//!< dest	(srcR, srcG, srcB, srcA)
		EBF_ONE_MINUS_SRC_COLOR, 	//!< dest	(1-srcR, 1-srcG, 1-srcB, 1-srcA)
		EBF_SRC_ALPHA,			//!< src & dest	(srcA, srcA, srcA, srcA)
		EBF_ONE_MINUS_SRC_ALPHA,	//!< src & dest	(1-srcA, 1-srcA, 1-srcA, 1-srcA)
		EBF_DST_ALPHA,			//!< src & dest	(destA, destA, destA, destA)
		EBF_ONE_MINUS_DST_ALPHA,	//!< src & dest	(1-destA, 1-destA, 1-destA, 1-destA)
		EBF_SRC_ALPHA_SATURATE		//!< src	(min(srcA, 1-destA), idem, ...)
	};

	//! MaterialTypeParam: e.g. DirectX: D3DTOP_MODULATE, D3DTOP_MODULATE2X, D3DTOP_MODULATE4X
	enum E_MODULATE_FUNC
	{
		EMFN_MODULATE_1X	= 1,
		EMFN_MODULATE_2X	= 2,
		EMFN_MODULATE_4X	= 4
	};

	//! EMT_ONETEXTURE_BLEND: pack srcFact & dstFact and Modulo to MaterialTypeParam
	inline f32 pack_texureBlendFunc ( const E_BLEND_FACTOR srcFact, const E_BLEND_FACTOR dstFact, const E_MODULATE_FUNC modulate )
	{
		return (f32)(modulate << 16 | srcFact << 8 | dstFact);
	}

	//! EMT_ONETEXTURE_BLEND: unpack srcFact & dstFact and Modulo to MaterialTypeParam
	inline void unpack_texureBlendFunc ( E_BLEND_FACTOR &srcFact, E_BLEND_FACTOR &dstFact,
			E_MODULATE_FUNC &modulo, const f32 param )
	{
		const u32 state = (u32)param;
		modulo	= E_MODULATE_FUNC( ( state & 0x00FF0000 ) >> 16 );
		srcFact = E_BLEND_FACTOR ( ( state & 0x0000FF00 ) >> 8 );
		dstFact = E_BLEND_FACTOR ( ( state & 0x000000FF ) );
	}

	//! Maximum number of texture an SMaterial can have.
	const u32 MATERIAL_MAX_TEXTURES = 4;

	//! Struct for holding parameters for a material renderer
	class SMaterial
	{
	public:
		//! Default constructor. Creates a solid, lit material with white colors
		SMaterial()
		: MaterialType(EMT_SOLID), AmbientColor(255,255,255,255), DiffuseColor(255,255,255,255),
			EmissiveColor(0,0,0,0), SpecularColor(255,255,255,255),
			Shininess(0.0f), MaterialTypeParam(0.0f), MaterialTypeParam2(0.0f), Thickness(1.0f),
			Wireframe(false), PointCloud(false), GouraudShading(true), Lighting(true),
			ZWriteEnable(true), BackfaceCulling(true), FrontfaceCulling(false),
			FogEnable(false), NormalizeNormals(false), ZBuffer(1),MatValues(NULL)
		{ }

		//! Copy constructor
		/** \param other Material to copy from. */
		SMaterial(const SMaterial& other) : MatValues(NULL)
		{
			// These pointers are checked during assignment
			for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
				TextureLayer[i].TextureMatrix = 0;
			*this = other;
		}
		~SMaterial()
		{
			if(MatValues)
				MatValues->drop();
		}

		//! Assignment operator
		/** \param other Material to copy from. */
		SMaterial& operator=(const SMaterial& other)
		{
			// Check for self-assignment!
			if (this == &other)
				return *this;

			MaterialType = other.MaterialType;

			AmbientColor = other.AmbientColor;
			DiffuseColor = other.DiffuseColor;
			EmissiveColor = other.EmissiveColor;
			SpecularColor = other.SpecularColor;
			Shininess = other.Shininess;
			MaterialTypeParam = other.MaterialTypeParam;
			MaterialTypeParam2 = other.MaterialTypeParam2;
			Thickness = other.Thickness;
			for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
			{
				TextureLayer[i] = other.TextureLayer[i];
			}

			Wireframe = other.Wireframe;
			PointCloud = other.PointCloud;
			GouraudShading = other.GouraudShading;
			Lighting = other.Lighting;
			ZWriteEnable = other.ZWriteEnable;
			BackfaceCulling = other.BackfaceCulling;
			FrontfaceCulling = other.FrontfaceCulling;
			FogEnable = other.FogEnable;
			NormalizeNormals = other.NormalizeNormals;
			ZBuffer = other.ZBuffer;

			if(MatValues)
				delete []MatValues;
			MatValues = IMaterialSystem::copyAttributes(other.MatValues);

			return *this;
		}

		//! Type of the material. Specifies how everything is blended together
		E_MATERIAL_TYPE MaterialType;

		//! How much ambient light (a global light) is reflected by this material.
		/** The default is full white, meaning objects are completely
		globally illuminated. Reduce this if you want to see diffuse
		or specular light effects. */
		SColor AmbientColor;

		//! How much diffuse light coming from a light source is reflected by this material.
		/** The default is full white. */
		SColor DiffuseColor;

		//! Light emitted by this material. Default is to emit no light.
		SColor EmissiveColor;

		//! How much specular light (highlights from a light) is reflected.
		/** The default is to reflect white specular light. See
		SMaterial::Shininess on how to enable specular lights. */
		SColor SpecularColor;

		//! Value affecting the size of specular highlights.
		/** A value of 20 is common. If set to 0, no specular
		highlights are being used. To activate, simply set the
		shininess of a material to a value other than 0:
		\code
		sceneNode->getMaterial(0).Shininess = 20.0f;
		\endcode

		You can change the color of the highlights using
		\code
		sceneNode->getMaterial(0).SpecularColor.set(255,255,255,255);
		\endcode

		The specular color of the dynamic lights
		(SLight::SpecularColor) will influence the the highlight color
		too, but they are set to a useful value by default when
		creating the light scene node. Here is a simple example on how
		to use specular highlights:
		\code
		// load and display mesh
		scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(
		smgr->getMesh("data/faerie.md2"));
		node->setMaterialTexture(0, driver->getTexture("data/Faerie2.pcx")); // set diffuse texture
		node->setMaterialFlag(video::EMF_LIGHTING, true); // enable dynamic lighting
		node->getMaterial(0).Shininess = 20.0f; // set size of specular highlights

		// add white light
		scene::ILightSceneNode* light = smgr->addLightSceneNode(0,
			core::vector3df(5,5,5), video::SColorf(1.0f, 1.0f, 1.0f));
		\endcode */
		f32 Shininess;

		//! Free parameter, dependent on the material type.
		/** Mostly ignored, used for example in EMT_PARALLAX_MAP_SOLID
		and EMT_TRANSPARENT_ALPHA_CHANNEL. */
		f32 MaterialTypeParam;

		//! Second free parameter, dependent on the material type.
		/** Mostly ignored. */
		f32 MaterialTypeParam2;

		//! Thickness of non-3dimensional elements such as lines and points.
		f32 Thickness;

		//! Texture layer array.
		SMaterialLayer TextureLayer[MATERIAL_MAX_TEXTURES];

		//! Draw as wireframe or filled triangles? Default: false
		/** The user can access a material flag using
		\code material.Wireframe=true \endcode
		or \code material.setFlag(EMF_WIREFRAME, true); \endcode */
		bool Wireframe;

		//! Draw as point cloud or filled triangles? Default: false
		bool PointCloud;

		//! Flat or Gouraud shading? Default: true
		bool GouraudShading;

		//! Will this material be lighted? Default: true
		bool Lighting;

		//! Is the zbuffer writeable or is it read-only. Default: true.
		/** This flag is ignored if the MaterialType is a transparent
		type. */
		bool ZWriteEnable;

		//! Is backface culling enabled? Default: true
		bool BackfaceCulling;

		//! Is frontface culling enabled? Default: false
		bool FrontfaceCulling;

		//! Is fog enabled? Default: false
		bool FogEnable;

		//! Should normals be normalized? Default: false
		bool NormalizeNormals;

		//! Is the ZBuffer enabled? Default: 1
		/** Changed from bool to integer
		(0 == ZBuffer Off, 1 == ZBuffer LessEqual, 2 == ZBuffer Equal)
		*/
		char ZBuffer;

		//! Gets the texture transformation matrix for level i
		/** \param i The desired level. Must not be larger than MATERIAL_MAX_TEXTURES.
		\return Texture matrix for texture level i. */
		core::matrix4& getTextureMatrix(u32 i)
		{
			return TextureLayer[i].getTextureMatrix();
		}

		//! Gets the immutable texture transformation matrix for level i
		/** \param i The desired level.
		\return Texture matrix for texture level i, or identity matrix for levels larger than MATERIAL_MAX_TEXTURES. */
		const core::matrix4& getTextureMatrix(u32 i) const
		{
			if (i<MATERIAL_MAX_TEXTURES)
				return TextureLayer[i].getTextureMatrix();
			else
				return core::IdentityMatrix;
		}

		//! Sets the i-th texture transformation matrix
		/** \param i The desired level.
		\param mat Texture matrix for texture level i. */
		void setTextureMatrix(u32 i, const core::matrix4& mat)
		{
			if (i>=MATERIAL_MAX_TEXTURES)
				return;
			TextureLayer[i].setTextureMatrix(mat);
		}

		//! Gets the i-th texture
		/** \param i The desired level.
		\return Texture for texture level i, if defined, else 0. */
		ITexture* getTexture(u32 i) const
		{
			return i < MATERIAL_MAX_TEXTURES ? TextureLayer[i].Texture : 0;
		}

		//! Sets the i-th texture
		/** If i>=MATERIAL_MAX_TEXTURES this setting will be ignored.
		\param i The desired level.
		\param tex Texture for texture level i. */
		void setTexture(u32 i, ITexture* tex)
		{
			if (i>=MATERIAL_MAX_TEXTURES)
				return;
			TextureLayer[i].Texture = tex;
		}

		//! Sets the Material flag to the given value
		/** \param flag The flag to be set.
		\param value The new value for the flag. */
		void setFlag(E_MATERIAL_FLAG flag, bool value)
		{
			switch (flag)
			{
				case EMF_WIREFRAME:
					Wireframe = value; break;
				case EMF_POINTCLOUD:
					PointCloud = value; break;
				case EMF_GOURAUD_SHADING:
					GouraudShading = value; break;
				case EMF_LIGHTING:
					Lighting = value; break;
				case EMF_ZBUFFER:
					ZBuffer = value; break;
				case EMF_ZWRITE_ENABLE:
					ZWriteEnable = value; break;
				case EMF_BACK_FACE_CULLING:
					BackfaceCulling = value; break;
				case EMF_FRONT_FACE_CULLING:
					FrontfaceCulling = value; break;
				case EMF_BILINEAR_FILTER:
				{
					for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
						TextureLayer[i].BilinearFilter = value;
				}
				break;
				case EMF_TRILINEAR_FILTER:
				{
					for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
						TextureLayer[i].TrilinearFilter = value;
				}
				break;
				case EMF_ANISOTROPIC_FILTER:
				{
					for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
						TextureLayer[i].AnisotropicFilter = value;
				}
				break;
				case EMF_FOG_ENABLE:
					FogEnable = value; break;
				case EMF_NORMALIZE_NORMALS:
					NormalizeNormals = value; break;
				case EMF_TEXTURE_WRAP:
				{
					for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
						TextureLayer[i].TextureWrap = (E_TEXTURE_CLAMP)value;
				}
				break;
				default:
					break;
			}
		}

		//! Gets the Material flag
		/** \param flag The flag to query.
		\return The current value of the flag. */
		bool getFlag(E_MATERIAL_FLAG flag) const
		{
			switch (flag)
			{
				case EMF_WIREFRAME:
					return Wireframe;
				case EMF_POINTCLOUD:
					return PointCloud;
				case EMF_GOURAUD_SHADING:
					return GouraudShading;
				case EMF_LIGHTING:
					return Lighting;
				case EMF_ZBUFFER:
					return ZBuffer!=0;
				case EMF_ZWRITE_ENABLE:
					return ZWriteEnable;
				case EMF_BACK_FACE_CULLING:
					return BackfaceCulling;
				case EMF_FRONT_FACE_CULLING:
					return FrontfaceCulling;
				case EMF_BILINEAR_FILTER:
					return TextureLayer[0].BilinearFilter;
				case EMF_TRILINEAR_FILTER:
					return TextureLayer[0].TrilinearFilter;
				case EMF_ANISOTROPIC_FILTER:
					return TextureLayer[0].AnisotropicFilter;
				case EMF_FOG_ENABLE:
					return FogEnable;
				case EMF_NORMALIZE_NORMALS:
					return NormalizeNormals;
				case EMF_TEXTURE_WRAP:
					return !(TextureLayer[0].TextureWrap ||
							TextureLayer[1].TextureWrap ||
							TextureLayer[2].TextureWrap ||
							TextureLayer[3].TextureWrap);
				case EMF_MATERIAL_FLAG_COUNT:
					break;
			}

			return false;
		}

		//! Inequality operator
		/** \param b Material to compare to.
		\return True if the materials differ, else false. */
		inline bool operator!=(const SMaterial& b) const
		{
			bool different =
				MaterialType != b.MaterialType ||
				AmbientColor != b.AmbientColor ||
				DiffuseColor != b.DiffuseColor ||
				EmissiveColor != b.EmissiveColor ||
				SpecularColor != b.SpecularColor ||
				Shininess != b.Shininess ||
				MaterialTypeParam != b.MaterialTypeParam ||
				MaterialTypeParam2 != b.MaterialTypeParam2 ||
				Thickness != b.Thickness ||
				Wireframe != b.Wireframe ||
				PointCloud != b.PointCloud ||
				GouraudShading != b.GouraudShading ||
				Lighting != b.Lighting ||
				ZBuffer != b.ZBuffer ||
				ZWriteEnable != b.ZWriteEnable ||
				BackfaceCulling != b.BackfaceCulling ||
				FrontfaceCulling != b.FrontfaceCulling ||
				FogEnable != b.FogEnable ||
				NormalizeNormals != b.NormalizeNormals;
			for (u32 i=0; (i<MATERIAL_MAX_TEXTURES) && !different; ++i)
			{
				different |= (TextureLayer[i] != b.TextureLayer[i]);
			}
			return different;
		}

		//! Equality operator
		/** \param b Material to compare to.
		\return True if the materials are equal, else false. */
		inline bool operator==(const SMaterial& b) const
		{ return !(b!=*this); }

		bool isTransparent() const
		{
			return MaterialType==EMT_TRANSPARENT_ADD_COLOR ||
				MaterialType==EMT_TRANSPARENT_ALPHA_CHANNEL ||
				MaterialType==EMT_TRANSPARENT_VERTEX_ALPHA ||
				MaterialType==EMT_TRANSPARENT_REFLECTION_2_LAYER;
		}

		//! Set a float material value
		/** \param name value name.
			\param value the new value to set.
		*/
		void setValue(const c8 *name,f32 value)
		{
			if(!_stricmp(name,"Shininess"))
				Shininess = value;
			else if(!_stricmp(name,"MaterialTypeParam"))
				MaterialTypeParam = value;
			else if(!_stricmp(name,"MaterialTypeParam2"))
				MaterialTypeParam2 = value;
			else if(!_stricmp(name,"Thickness"))
				Thickness = value;
			else switch(IMaterialSystem::isBuiltinValue(name,io::EAT_FLOAT))
			{
				case io::EAT_BOOL:
					setValue(name,(bool)(value != 0));
					break;
				case io::EAT_COLOR:
					setValue(name,SColorf(value,value,value));
					break;
				case io::EAT_INT:
					setValue(name,(s32)value);
					break;
				case io::EAT_UNKNOWN:
					createAttributes();
					MatValues->setAttribute(name,value);
			}
		}
		//! Gets a float material value
		/** \param name value name.
		\return The material value. */
		f32 getFloatValue(const c8 *name) const
		{
			if(!_stricmp(name,"Shininess"))
				return Shininess;
			else if(!_stricmp(name,"MaterialTypeParam"))
				return MaterialTypeParam;
			else if(!_stricmp(name,"MaterialTypeParam2"))
				return MaterialTypeParam2;
			else if(!_stricmp(name,"Thickness"))
				return Thickness;
			else switch(IMaterialSystem::isBuiltinValue(name,io::EAT_FLOAT))
			{
				case io::EAT_BOOL:
					return (f32)getBoolValue(name);
				case io::EAT_INT:
					return (f32)getIntValue(name);
					break;
				case io::EAT_UNKNOWN:
					if(MatValues)
						return MatValues->getAttributeAsFloat(name);
				default:
					return 0.f;
			}
		}
		//! Set a bool material value
		/** \param name value name.
			\param value the new value to set.
		*/
		void setValue(const c8 *name,bool value)
		{
			if(!_stricmp(name,"Wireframe"))
				Wireframe = value;
			else if(!_stricmp(name,"PointCloud"))
				PointCloud = value;
			else if(!_stricmp(name,"GouraudShading"))
				GouraudShading = value;
			else if(!_stricmp(name,"Lighting"))
				Lighting = value;
			else if(!_stricmp(name,"ZWriteEnable"))
				ZWriteEnable = value;
			else if(!_stricmp(name,"BackfaceCulling"))
				BackfaceCulling = value;
			else if(!_stricmp(name,"FrontfaceCulling"))
				FrontfaceCulling = value;
			else if(!_stricmp(name,"BilinearFilter"))
				setFlag(EMF_BILINEAR_FILTER,value);
			else if(!_stricmp(name,"TrilinearFilter"))
				setFlag(EMF_TRILINEAR_FILTER,value);
			else if(!_stricmp(name,"AnisotropicFilter"))
				setFlag(EMF_ANISOTROPIC_FILTER,value);
			else if(!_stricmp(name,"FogEnable"))
				FogEnable = value;
			else if(!_stricmp(name,"Lighting"))
				Lighting = value;
			else if(!_stricmp(name,"NormalizeNormals"))
				NormalizeNormals = value;

			else switch(IMaterialSystem::isBuiltinValue(name,io::EAT_BOOL))
			{
				case io::EAT_FLOAT:
					setValue(name,(f32)value);
					break;
				case io::EAT_COLOR:
					setValue(name,SColorf((f32)value,(f32)value,(f32)value));
					break;
				case io::EAT_INT:
					setValue(name,(int)value);
					break;
				case io::EAT_UNKNOWN:
					createAttributes();
					MatValues->setAttribute(name,value);
			}
		}

		//! Gets a bool material value
		/** \param name value name.
		\return The material value. */
		bool getBoolValue(const c8 *name) const
		{
			if(!_stricmp(name,"Wireframe"))
				return Wireframe;
			else if(!_stricmp(name,"PointCloud"))
				return PointCloud;
			else if(!_stricmp(name,"GouraudShading"))
				return GouraudShading;
			else if(!_stricmp(name,"Lighting"))
				return Lighting;
			else if(!_stricmp(name,"ZWriteEnable"))
				return ZWriteEnable;
			else if(!_stricmp(name,"BackfaceCulling"))
				return BackfaceCulling;
			else if(!_stricmp(name,"FrontfaceCulling"))
				return FrontfaceCulling;
			else if(!_stricmp(name,"BilinearFilter"))
				return getFlag(EMF_BILINEAR_FILTER);
			else if(!_stricmp(name,"TrilinearFilter"))
				return getFlag(EMF_TRILINEAR_FILTER);
			else if(!_stricmp(name,"AnisotropicFilter"))
				return getFlag(EMF_ANISOTROPIC_FILTER);
			else if(!_stricmp(name,"FogEnable"))
				return FogEnable;
			else if(!_stricmp(name,"Lighting"))
				return Lighting;
			else if(!_stricmp(name,"NormalizeNormals"))
				return NormalizeNormals;

			else switch(IMaterialSystem::isBuiltinValue(name,io::EAT_BOOL))
			{
				case io::EAT_FLOAT:
					return getFloatValue(name) != 0;
					break;
				case io::EAT_INT:
					return getIntValue(name) != 0;
					break;
				case io::EAT_UNKNOWN:
					if(MatValues)
						MatValues->getAttributeAsBool(name);
				default:
					return false;
			}
		}
		//! Set a color material value
		/** \param name value name.
			\param value the new value to set.
		*/
		void setValue(const c8 *name,SColor &value)
		{
			if(!_stricmp(name,"AmbientColor"))
				AmbientColor = value;
			else if(!_stricmp(name,"DiffuseColor"))
				DiffuseColor = value;
			else if(!_stricmp(name,"EmissiveColor"))
				EmissiveColor = value;
			else if(!_stricmp(name,"SpecularColor"))
				SpecularColor = value;
			else switch(IMaterialSystem::isBuiltinValue(name,io::EAT_COLOR))
			{
				case io::EAT_UNKNOWN:
					createAttributes();
					MatValues->setAttribute(name,value);
			}
		}
		//! Gets a SColor material value
		/** \param name value name.
		\return The material value. */
		SColor getColorValue(const c8 *name) const
		{
			if(!_stricmp(name,"AmbientColor"))
				return AmbientColor;
			else if(!_stricmp(name,"DiffuseColor"))
				return DiffuseColor;
			else if(!_stricmp(name,"EmissiveColor"))
				return EmissiveColor;
			else if(!_stricmp(name,"SpecularColor"))
				return SpecularColor;
			else switch(IMaterialSystem::isBuiltinValue(name,io::EAT_COLOR))
			{
				case io::EAT_UNKNOWN:
					if(MatValues)
						return MatValues->getAttributeAsColor(name);
				default:
					return SColor();
			}
			
		}
		//! Set a color material value
		/** \param name value name.
			\param value the new value to set.
		*/
		void setValue(const c8 *name,SColorf &value)
		{
			setValue(name,value.toSColor());
		}
		//! Set a integer material value
		/** \param name value name.
			\param value the new value to set.
		*/
		void setValue(const c8 *name,s32 value)
		{
			if(!_stricmp(name,"ZBuffer"))
				ZBuffer = core::clamp(value,0,2);
			else if(!_stricmp(name,"TextureWrap"))
			{	
				for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
					TextureLayer[i].TextureWrap = (E_TEXTURE_CLAMP)core::clamp(value,(s32)ETC_REPEAT,(s32)ETC_MIRROR);	
			}
			else switch(IMaterialSystem::isBuiltinValue(name,io::EAT_INT))
			{
				case io::EAT_BOOL:
					setValue(name,(bool)(value != 0));
					break;
				case io::EAT_COLOR:
					setValue(name,SColor(255,value,value,value));
					break;
				case io::EAT_INT:
					setValue(name,(f32)value);
					break;
				case io::EAT_UNKNOWN:
					createAttributes();
					MatValues->setAttribute(name,value);
			}
		}
		//! Gets a integer material value
		/** \param name value name.
		\return The material value. */
		s32 getIntValue(const c8 *name) const
		{
			if(!_stricmp(name,"ZBuffer"))
				return (int)ZBuffer;
			else if(!_stricmp(name,"TextureWrap"))
			{	
				s32 ret = TextureLayer[0].TextureWrap;
				for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
				{
					if(ret != TextureLayer[i].TextureWrap)
						return ETC_DIFFERENT;
				}
				return ret;
			}
			else switch(IMaterialSystem::isBuiltinValue(name,io::EAT_INT))
			{
				case io::EAT_BOOL:
					return (s32)getBoolValue(name);
				case io::EAT_FLOAT:
					return (s32)getFloatValue(name);
				case io::EAT_UNKNOWN:
					if(MatValues)
						return MatValues->getAttributeAsInt(name);
				default:
					return 0;
			}
		}

		//! Set a vector3df material value
		/** \param name value name.
			\param value the new value to set.
		*/
		void setValue(const c8 *name,core::vector3df &value)
		{
			if(IMaterialSystem::isBuiltinValue(name) == io::EAT_UNKNOWN)
			{
					createAttributes();
					MatValues->setAttribute(name,value);
			}
		}
		//! Gets a vector3df material value
		/** \param name value name.
		\return The material value. */
		core::vector3df getVector3dValue(const c8 *name) const
		{
			if(IMaterialSystem::isBuiltinValue(name) == io::EAT_UNKNOWN && MatValues)
			{
				return MatValues->getAttributeAsVector3d(name);
			}
		}
		//! Set a position2di material value
		/** \param name value name.
			\param value the new value to set.
		*/
		void setValue(const c8 *name,core::position2di &value)
		{
			if(IMaterialSystem::isBuiltinValue(name) == io::EAT_UNKNOWN)
			{
					createAttributes();
					MatValues->setAttribute(name,value);
			}
		}
		//! Gets a position2di material value
		/** \param name value name.
		\return The material value. */
		core::position2di getPosition2dValue(const c8 *name) const
		{
			if(IMaterialSystem::isBuiltinValue(name) == io::EAT_UNKNOWN && MatValues)
			{
				return MatValues->getAttributeAsPosition2d(name);
			}
		}
		//! Set a matrix material value
		/** \param name value name.
			\param value the new value to set.
		*/
		void setValue(const c8 *name,core::matrix4 &value)
		{
			if(IMaterialSystem::isBuiltinValue(name) == io::EAT_UNKNOWN)
			{
					createAttributes();
					MatValues->setAttribute(name,value);
			}
		}

		//! Gets a matrix material value
		/** \param name value name.
		\return The material value. */
		const core::matrix4 getMatrixValue(const c8 *name) const
		{
			if(IMaterialSystem::isBuiltinValue(name) == io::EAT_UNKNOWN && MatValues)
			{
				return MatValues->getAttributeAsMatrix(name);
			}
		}
		
		private:
		
		io::IAttributes *MatValues;
		
		inline void createAttributes()
		{
			if(!MatValues)
				MatValues = IMaterialSystem::createMaterialAttributes();
		}

	};

} // end namespace video
} // end namespace irr

#endif

