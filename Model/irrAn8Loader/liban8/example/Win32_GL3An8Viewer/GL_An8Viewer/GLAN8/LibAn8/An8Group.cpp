// This file in a part of LibAn8, Copyright (c) 2002-2007, Gr�gory Smialek
// It's licence can be found in an8loader.h included in this package
// WebSite: texel3d.free.fr
#include "An8Loader.h"
#include <math.h>
using namespace std;

An8Group::An8Group(void)
{
	BaseOrigin = AN8XVECTOR3(0.0f,0.0f,0.0f);
	BaseOrientation = AN8XQUATERNION(0.0f,0.0f,0.0f,1.0f);
	AN8XMatrixIdentity(&Matrix);

	NbrMesh = 0;
	NbrCube = 0;
	NbrSphere = 0;
	NbrCylinder = 0;
	NbrGroup = 0;
	NbrSubdivision = 0;
}

An8Group::~An8Group(void)
{

}

void An8Group::Load(AN8XPARSER*p_parser,An8Object* object)
{

	int groupStart =p_parser->IndexOf(1,"g");
	if(groupStart<0) return;

	// Name
	p_parser->ReadLine();
	p_parser->GetName(&Name);

	// Base
	p_parser->ReadLine();
	if(p_parser->FindWord("base {"))
	{
		// BaseOrigin
		p_parser->ReadLine();
		if(p_parser->FindWord("origin {"))
		{
			sscanf(p_parser->GetLine(),"      origin { (%f %f %f\n",
				&BaseOrigin.x,
				&BaseOrigin.y,
				&BaseOrigin.z
			);
			p_parser->ReadLine();
		}
		// BaseOrientation
		if(p_parser->FindWord("orientation {"))
		{
			sscanf(p_parser->GetLine(),"      orientation { (%f %f %f %f\n",
				&BaseOrientation.x,
				&BaseOrientation.y,
				&BaseOrientation.z,
				&BaseOrientation.w
			);
			p_parser->ReadLine();
		}
	} // End of if base

	int groupEnd =p_parser->IndexOf(1,"}");
	// While we do not find the end of the object
	// And while we are not at the end of the group
	while(p_parser->Length()!=1+LINERETURN && groupStart != groupEnd)
	{
		if(p_parser->FindWord("group {"))
		{
			++NbrGroup;
			An8Group newGroup;
			newGroup.Load(p_parser,object);
			vGroups.push_back(newGroup);
		}
		else if(p_parser->FindWord("modifier {") ||p_parser->FindWord("textcom {") ||p_parser->FindWord("pathcom {") ||p_parser->FindWord("image {"))
		{
			int iStart=0;
			while(p_parser->GetLine()[iStart]==' ')
              ++iStart;
			while(p_parser->Length()!=(iStart+2) )
				p_parser->ReadLine();
		}
		else if(p_parser->FindWord("mesh {") ||p_parser->FindWord("subdivision {"))
		{
			++NbrMesh;
			An8Mesh newMesh;
			if(p_parser->FindWord("subdivision {"))
			{
			   ++NbrSubdivision;
		    }
			newMesh.Load(p_parser,object);
			vMeshes.push_back(newMesh);
		}
		else if(p_parser->FindWord("morphtarget {"))
		{
            do{
             p_parser->ReadLine();
            }
            while(!p_parser->FindWord("  }"));
        }
		else if(p_parser->FindWord("cube {"))
		{
			++NbrCube;
			An8Cube newCube;
			newCube.Load(p_parser);
			newCube.ComputeMaterialID(object->vMaterials);
			vCubes.push_back(newCube);
		}
		else if(p_parser->FindWord("sphere {"))
		{
			++NbrSphere;
			An8Sphere newSphere;
			newSphere.Load(p_parser);
			newSphere.ComputeMaterialID(object->vMaterials);
			vSpheres.push_back(newSphere);
		}
		else if(p_parser->FindWord("cylinder {"))
		{
			++NbrCylinder;
			An8Cylinder newCylinder;
			newCylinder.Load(p_parser);
			newCylinder.ComputeMaterialID(object->vMaterials);
			vCylinders.push_back(newCylinder);
		}
		p_parser->ReadLine();
	    groupEnd =p_parser->IndexOf(1,"}");
	}

}

vector<An8Mesh> An8Group::GetMeshes(void)
{
	return vMeshes;
}

vector<An8Cube> An8Group::GetCubes(void)
{
	return vCubes;
}

vector<An8Sphere> An8Group::GetSpheres(void)
{
	return vSpheres;
}

vector<An8Cylinder> An8Group::GetCylinders(void)
{
	return vCylinders;
}

void An8Group::Ungroup(void)
{
	for( int i = 0 ; i < NbrGroup ; ++i )
	{
		// Groups
		vGroups[i].Ungroup();

		// Meshes
		vector<An8Mesh> vSubMeshes = vGroups[i].GetMeshes();
		for(int j = 0 ; j< (int)vSubMeshes.size() ; ++j )
		{
			AN8XBaseConcatenate(
					&vSubMeshes[j].BaseOrigin, &vSubMeshes[j].BaseOrientation,
					&vGroups[i].BaseOrigin, &vGroups[i].BaseOrientation,
					&vSubMeshes[j].BaseOrigin, &vSubMeshes[j].BaseOrientation);

			vMeshes.push_back(vSubMeshes[j]);
			++NbrMesh;
			// end new
		}

		// Cubes
		vector<An8Cube> vSubCubes = vGroups[i].GetCubes();
		for(int j = 0 ; j< (int)vSubCubes.size() ; ++j )
		{
			AN8XBaseConcatenate(
					&vSubCubes[j].BaseOrigin, &vSubCubes[j].BaseOrientation,
					&vGroups[i].BaseOrigin, &vGroups[i].BaseOrientation,
					&vSubCubes[j].BaseOrigin, &vSubCubes[j].BaseOrientation);

			vCubes.push_back(vSubCubes[j]);
			++NbrCube;
			// end new
		}

		// Spheres
		vector<An8Sphere> vSubSpheres = vGroups[i].GetSpheres();
		for(int j = 0 ; j< (int)vSubSpheres.size() ; ++j )
		{
			AN8XBaseConcatenate(
					&vSubSpheres[j].BaseOrigin, &vSubSpheres[j].BaseOrientation,
					&vGroups[i].BaseOrigin, &vGroups[i].BaseOrientation,
					&vSubSpheres[j].BaseOrigin, &vSubSpheres[j].BaseOrientation);

			vSpheres.push_back(vSubSpheres[j]);
			++NbrSphere;
			// end new
		}

		// Cylinders
		vector<An8Cylinder> vSubCylinders = vGroups[i].GetCylinders();
		for(int j = 0 ; j< (int)vSubCylinders.size() ; ++j )
		{
			AN8XBaseConcatenate(
					&vSubCylinders[j].BaseOrigin, &vSubCylinders[j].BaseOrientation,
					&vGroups[i].BaseOrigin, &vGroups[i].BaseOrientation,
					&vSubCylinders[j].BaseOrigin, &vSubCylinders[j].BaseOrientation);

			vCylinders.push_back(vSubCylinders[j]);
			++NbrCylinder;


		}
	}
	if( vGroups.empty() == false )
	{
		vGroups.clear();
		NbrGroup = 0;
	}
}

void An8Group::Triangulize(int numMesh)
{
	if( numMesh > -1 ) return;

	for(int i = 0 ; i < NbrMesh ; ++i)
	{
		vMeshes[i].Triangulize();
	}

	for(int i = 0 ; i < NbrGroup ; ++i)
	{
		vGroups[i].Triangulize();
	}
}

void An8Group::ComputeFaceNormal(void)
{
	for(int i = 0 ; i < NbrMesh ; ++i)
	{
		vMeshes[i].ComputeFaceNormal();
	}

	for(int i = 0 ; i < NbrGroup ; ++i)
	{
		vGroups[i].ComputeFaceNormal();
	}
}

void An8Group::ComputeMatrix(bool includeTranslation)
{
	AN8XMATRIX matrixTmp;
	AN8XMatrixRotationQuaternion( &matrixTmp , &BaseOrientation );
	if( includeTranslation == true )
	{
		matrixTmp[12] = BaseOrigin.x;
		matrixTmp[13] = BaseOrigin.y;
		matrixTmp[14] = BaseOrigin.z;
	}
	/*if( isGL == true )
	{*/
		AN8XMatrixCopy( &Matrix , &matrixTmp);
/*	}
	else
	{
		AN8XMatrixTranspose( &Matrix , &matrixTmp);
	}*/

}

void An8Group::ComputeBoundingElements(void)
{
	BoundingBox.Min.x = BoundingBox.Min.y = BoundingBox.Min.z =  9999999.0f;
	BoundingBox.Max.x = BoundingBox.Max.y = BoundingBox.Max.z = -9999999.0f;

	for(int i = 0 ; i < NbrGroup ; ++i)
	{
		vGroups[i].ComputeBoundingElements();

		if( vGroups[i].BoundingBox.Min.x< BoundingBox.Min.x)
          BoundingBox.Min.x = vGroups[i].BoundingBox.Min.x;

        if( vGroups[i].BoundingBox.Max.x > BoundingBox.Max.x)
          BoundingBox.Max.x = vGroups[i].BoundingBox.Max.x;

        if( vGroups[i].BoundingBox.Min.y < BoundingBox.Min.y)
          BoundingBox.Min.y = vGroups[i].BoundingBox.Min.y;

        if( vGroups[i].BoundingBox.Max.y > BoundingBox.Max.y)
          BoundingBox.Max.y = vGroups[i].BoundingBox.Max.y;

        if( vGroups[i].BoundingBox.Min.z < BoundingBox.Min.z )
          BoundingBox.Min.z = vGroups[i].BoundingBox.Min.z;

        if( vGroups[i].BoundingBox.Max.z > BoundingBox.Max.z)
          BoundingBox.Max.z = vGroups[i].BoundingBox.Max.z;
	}

	for(int i = 0 ; i < NbrMesh ; ++i)
	{
		vMeshes[i].ComputeBoundingElements();

        if( vMeshes[i].BoundingBox.Min.x< BoundingBox.Min.x)
          BoundingBox.Min.x = vMeshes[i].BoundingBox.Min.x;

        if( vMeshes[i].BoundingBox.Max.x > BoundingBox.Max.x)
          BoundingBox.Max.x = vMeshes[i].BoundingBox.Max.x;

        if( vMeshes[i].BoundingBox.Min.y < BoundingBox.Min.y)
          BoundingBox.Min.y = vMeshes[i].BoundingBox.Min.y;

        if( vMeshes[i].BoundingBox.Max.y > BoundingBox.Max.y)
          BoundingBox.Max.y = vMeshes[i].BoundingBox.Max.y;

        if( vMeshes[i].BoundingBox.Min.z < BoundingBox.Min.z )
          BoundingBox.Min.z = vMeshes[i].BoundingBox.Min.z;

        if( vMeshes[i].BoundingBox.Max.z > BoundingBox.Max.z)
          BoundingBox.Max.z = vMeshes[i].BoundingBox.Max.z;
	}

	for(int i = 0 ; i < NbrSphere ; ++i)
	{
		vSpheres[i].ComputeBoundingElements();

		if( vSpheres[i].BoundingBox.Min.x< BoundingBox.Min.x)
          BoundingBox.Min.x = vSpheres[i].BoundingBox.Min.x;

        if( vSpheres[i].BoundingBox.Max.x > BoundingBox.Max.x)
          BoundingBox.Max.x = vSpheres[i].BoundingBox.Max.x;

        if( vSpheres[i].BoundingBox.Min.y < BoundingBox.Min.y)
          BoundingBox.Min.y = vSpheres[i].BoundingBox.Min.y;

        if( vSpheres[i].BoundingBox.Max.y > BoundingBox.Max.y)
          BoundingBox.Max.y = vSpheres[i].BoundingBox.Max.y;

        if( vSpheres[i].BoundingBox.Min.z < BoundingBox.Min.z )
          BoundingBox.Min.z = vSpheres[i].BoundingBox.Min.z;

        if( vSpheres[i].BoundingBox.Max.z > BoundingBox.Max.z)
          BoundingBox.Max.z = vSpheres[i].BoundingBox.Max.z;
	}

	for(int i = 0 ; i < NbrCube ; ++i)
	{
		vCubes[i].ComputeBoundingElements();

		if( vCubes[i].BoundingBox.Min.x< BoundingBox.Min.x)
          BoundingBox.Min.x = vCubes[i].BoundingBox.Min.x;

        if( vCubes[i].BoundingBox.Max.x > BoundingBox.Max.x)
          BoundingBox.Max.x = vCubes[i].BoundingBox.Max.x;

        if( vCubes[i].BoundingBox.Min.y < BoundingBox.Min.y)
          BoundingBox.Min.y = vCubes[i].BoundingBox.Min.y;

        if( vCubes[i].BoundingBox.Max.y > BoundingBox.Max.y)
          BoundingBox.Max.y = vCubes[i].BoundingBox.Max.y;

        if( vCubes[i].BoundingBox.Min.z < BoundingBox.Min.z )
          BoundingBox.Min.z = vCubes[i].BoundingBox.Min.z;

        if( vCubes[i].BoundingBox.Max.z > BoundingBox.Max.z)
          BoundingBox.Max.z = vCubes[i].BoundingBox.Max.z;
	}

	for(int i = 0 ; i < NbrCylinder ; ++i)
	{
		vCylinders[i].ComputeBoundingElements();

		if( vCylinders[i].BoundingBox.Min.x< BoundingBox.Min.x)
          BoundingBox.Min.x = vCylinders[i].BoundingBox.Min.x;

        if( vCylinders[i].BoundingBox.Max.x > BoundingBox.Max.x)
          BoundingBox.Max.x = vCylinders[i].BoundingBox.Max.x;

        if( vCylinders[i].BoundingBox.Min.y < BoundingBox.Min.y)
          BoundingBox.Min.y = vCylinders[i].BoundingBox.Min.y;

        if( vCylinders[i].BoundingBox.Max.y > BoundingBox.Max.y)
          BoundingBox.Max.y = vCylinders[i].BoundingBox.Max.y;

        if( vCylinders[i].BoundingBox.Min.z < BoundingBox.Min.z )
          BoundingBox.Min.z = vCylinders[i].BoundingBox.Min.z;

        if( vCylinders[i].BoundingBox.Max.z > BoundingBox.Max.z)
          BoundingBox.Max.z = vCylinders[i].BoundingBox.Max.z;
	}

	BoundingSphereCenter.x = (BoundingBox.Min.x+BoundingBox.Max.x)/2.0f; // X
	BoundingSphereCenter.y = (BoundingBox.Min.y+BoundingBox.Max.y)/2.0f; // Y
	BoundingSphereCenter.z = (BoundingBox.Min.z+BoundingBox.Max.z)/2.0f; // Z

	BoundingSphereRadius = 
		 (BoundingBox.Max.x - BoundingBox.Min.x)*(BoundingBox.Max.x - BoundingBox.Min.x)
		+(BoundingBox.Max.y - BoundingBox.Min.y)*(BoundingBox.Max.y - BoundingBox.Min.y)
		+(BoundingBox.Max.z - BoundingBox.Min.z)*(BoundingBox.Max.z - BoundingBox.Min.z);

	BoundingSphereRadius = sqrt(BoundingSphereRadius);
	BoundingSphereRadius /= 2.0f;

}

void An8Group::ComputeAllPositions(AN8XQUATERNION *i_pBaseOrientation,AN8XVECTOR3* i_pBaseOrigin)
{
	if(i_pBaseOrientation != NULL && i_pBaseOrigin != NULL)
	{
		AN8XBaseConcatenate(
					&BaseOrigin, &BaseOrientation,
					i_pBaseOrigin, i_pBaseOrientation,
					&BaseOrigin, &BaseOrientation);
	}

	for(int i = 0 ; i < NbrMesh ; ++i)
	{
        vMeshes[i].ComputePosition(&BaseOrientation,&BaseOrigin);
	}
	for(int i = 0 ; i < NbrSphere ; ++i)
	{
        vSpheres[i].ComputePosition(&BaseOrientation,&BaseOrigin);
	}
	for(int i = 0 ; i < NbrCube ; ++i)
	{
        vCubes[i].ComputePosition(&BaseOrientation,&BaseOrigin);
	}
	for(int i = 0 ; i < NbrCylinder ; ++i)
	{
        vCylinders[i].ComputePosition(&BaseOrientation,&BaseOrigin);
	}
	for(int i = 0 ; i < NbrGroup ; ++i)
	{
        vGroups[i].ComputeAllPositions(&BaseOrientation,&BaseOrigin);
	}

	BaseOrientation.x = 0.0f;
	BaseOrientation.y = 0.0f;
	BaseOrientation.z = 0.0f;
	BaseOrientation.w = 1.0f;

	BaseOrigin.x = 0.0f;
	BaseOrigin.y = 0.0f;
	BaseOrigin.z = 0.0f;
}

void An8Group::ComputeAllMatrices(bool includeTranslation)
{
	ComputeMatrix(includeTranslation);

	for(int i = 0 ; i < NbrMesh ; ++i)
	{
        vMeshes[i].ComputeMatrix(includeTranslation);
	}

	for(int i = 0 ; i < NbrCube ; ++i)
	{
        vCubes[i].ComputeMatrix(includeTranslation);
	}

	for(int i = 0 ; i < NbrSphere ; ++i)
	{
        vSpheres[i].ComputeMatrix(includeTranslation);
	}

	for(int i = 0 ; i < NbrCylinder ; ++i)
	{
        vCylinders[i].ComputeMatrix(includeTranslation);
	}

	for(int i = 0 ; i < NbrGroup ; ++i)
	{
        vGroups[i].ComputeAllMatrices(includeTranslation);
	}
}

int An8Group::GetNbrMesh(void)
{
	int nbrMesh = NbrMesh;

	for(int i = 0 ; i < NbrGroup ; ++i)
	{
		nbrMesh += vGroups[i].GetNbrMesh();
	}
	return nbrMesh;
}



bool An8Group::HasNormal(void)
{

	bool result = false;

	for( int i = 0 ; i < NbrMesh ; ++i)
	{
		if( vMeshes[i].NbrNormal > 0 )
		{
			result = true;
			break;
		}
	}
	for( int i = 0 ; i < NbrGroup ; ++i)
	{
		if( vGroups[i].HasNormal() == true )
		{
			result = true;
			break;
		}
	}
	return result;
}

void An8Group::ConvertAllToMesh( void )
{
	for(int i = 0 ; i < NbrCube ; ++i)
	{
		An8Mesh* cubeMesh = NULL;
		cubeMesh = AN8XMeshFromCube( cubeMesh, &vCubes[i], AN8_NORMAL );
		if( cubeMesh != NULL)
		{
			vMeshes.push_back(*cubeMesh);
			++NbrMesh;
			delete cubeMesh;
			cubeMesh = NULL;
		}
	}
	if( vCubes.empty() == false )
	{
		vCubes.clear();
		NbrCube = 0;
	}

	for(int i = 0 ; i < NbrSphere ; ++i)
	{
		An8Mesh* sphereMesh = NULL;
		sphereMesh = AN8XMeshFromSphere( sphereMesh, &vSpheres[i], AN8_NORMAL );
		if( sphereMesh != NULL )
		{
			vMeshes.push_back(*sphereMesh);
			++NbrMesh;
			delete sphereMesh;
			sphereMesh = NULL;
		}
	}
	if( vSpheres.empty() == false )
	{
		vSpheres.clear();
		NbrSphere = 0;
	}

	for(int i = 0 ; i < NbrCylinder ; ++i)
	{
		An8Mesh* cylinderMesh = NULL;
		cylinderMesh = AN8XMeshFromCylinder( cylinderMesh, &vCylinders[i], AN8_NORMAL );
		if( cylinderMesh != NULL )
		{
			vMeshes.push_back(*cylinderMesh);
			++NbrMesh;
			delete cylinderMesh;
			cylinderMesh = NULL;
		}
	}
	if( vCylinders.empty() == false )
	{
		vCylinders.clear();
		NbrCylinder = 0;
	}
	for(int i = 0 ; i < NbrGroup ; ++i)
	{
        vGroups[i].ConvertAllToMesh();
	}
}

int An8Group::GetMeshID(std::string name)
{
	for(int i = 0; i < (int)this->vMeshes.size();++i )
	{
		if( name.compare( this->vMeshes[i].Name) ==0)
		{
			return i;
		}
	}
	return -1;
}
