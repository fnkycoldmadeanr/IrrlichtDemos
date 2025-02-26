// Copyright (C) 2007-2008 David Kment
// For license see CDFFMeshFileLoader.h

#include "CDFFMeshFileLoader.h"

#include <SAnimatedMesh.h>
#include <SMeshBuffer.h>
#include <irrString.h>
#include <IAttributes.h>
#include <IrrlichtDevice.h>


CDFFMeshFileLoader::CDFFMeshFileLoader(IrrlichtDevice* device)
{
    Mesh = NULL;

	FileSystem = device->getFileSystem();
	FileSystem->grab();

	Driver = device->getVideoDriver();
	Driver->grab();

	Parameters = device->getSceneManager()->getParameters();

	Logger = device->getLogger();
}

CDFFMeshFileLoader::~CDFFMeshFileLoader()
{
	if (FileSystem) {FileSystem->drop();}

	if (Driver) {Driver->drop();}

	if (Mesh) {Mesh->drop();}

	if (Logger) {Logger->drop();}
}

bool CDFFMeshFileLoader::isALoadableFileExtensiona(const c8* filename) const
{//const c8*
	return strstr(filename, ".dff") != 0;
}

bool CDFFMeshFileLoader::isALoadableFileExtension(const io::path& filename) const
{//const c8*
	//return strstr(filename, ".dff") != 0;
}

IAnimatedMesh* CDFFMeshFileLoader::createMesh(io::IReadFile* infile)
{
    Failed = false;
    SubsAreTriStrips = false;

	Mesh = new SMesh();

    File = infile;

    if(File->getSize() == 0) {return NULL;}

    unsigned int sectionType, sectionSize;
    unsigned short unknown, version;

    File->read(&sectionType, sizeof(int));
    File->read(&sectionSize, sizeof(int));
    File->read(&unknown, sizeof(short));
    File->read(&version, sizeof(short));

    if(version == DFF_VERSION_GTA3_1 || version == DFF_VERSION_GTA3_2 || version == DFF_VERSION_GTA3_3)
    {
        // GTA3 DFF file
        // note: DFF_VERSION_GTA3_2 file version is also used in GTAVC
    }
    else if(version == DFF_VERSION_GTAVC_1 || version == DFF_VERSION_GTAVC_2)
    {
        // GTAVC DFF file
    }
    else if(version == DFF_VERSION_GTASA)
    {
        // GTASA or ManHunt DFF file
    }
    else
    {
        // unsupported DFF file
        c8 tmp[255];
        sprintf(tmp, "%d", version);
///        Logger->log("unsupported DFF file", infile->getFileName(), ELL_WARNING);
///        Logger->log("version", tmp);
        return NULL;
    }

    FileVersion = version;

    parseDFFSection(sectionSize, 0, 1);

    compileMesh();

    // return NULL if DFF parsing failed
    if(Failed)
    {
///        Logger->log("Error while parsing DFF file", infile->getFileName(), ELL_WARNING);
        return NULL;
    }

    // calculate bounding box
	for (u32 j=0; j<Mesh->MeshBuffers.size(); ++j)
    {
        ((SMeshBuffer*)Mesh->MeshBuffers[j])->recalculateBoundingBox();
    }
	Mesh->recalculateBoundingBox();

    SAnimatedMesh* am = new SAnimatedMesh();
    am->Type = EAMT_UNKNOWN;
    am->addMesh(Mesh);
    am->recalculateBoundingBox();
    Mesh->drop();
    Mesh = 0;
    return am;
}

void CDFFMeshFileLoader::parseDFFSection(int size, int depthlevel, int parentType)
{
    if(size == 0) {return;}

    unsigned int sectionType, sectionSize;
    unsigned short sectionunknown = 0, sectionVersion = 0;

    int endoffset = File->getPos() + size;

    while(File->getPos() < endoffset)
    {
        File->read(&sectionType, sizeof(int));
        File->read(&sectionSize, sizeof(int));
        File->read(&sectionunknown, sizeof(short));
        File->read(&sectionVersion, sizeof(short));

        // uncomment to output the CLUMP structure in stdout
        // for(int i=0; i < depthlevel; i++) {fprintf(stdout, "  ");}
        // fprintf(stdout, "sectionType: %d, sectionSize: %d\n", sectionType, sectionSize);

        switch(sectionType)
        {
            case DFF_RW_DATA:
                processDFFData(sectionSize, parentType, sectionVersion);
                break;

            case DFF_RW_STRING:
                processDFFString(sectionSize, parentType, sectionVersion);
                break;

            case DFF_RW_FRAME:
                // not yet implemented, skip sectionSize bytes
                File->seek(sectionSize, true);
                break;

            case DFF_RW_MATERIALSPLIT:
                processDFFData(sectionSize, DFF_RW_MATERIALSPLIT, sectionVersion);
                break;

            // all below can contain more sections
            case DFF_RW_EXTENSION:
                parseDFFSection(sectionSize, depthlevel + 1, sectionType);
                break;

            case DFF_RW_TEXTURE:
                parseDFFSection(sectionSize, depthlevel + 1, sectionType);
                break;

            case DFF_RW_MATERIAL:
                parseDFFSection(sectionSize, depthlevel + 1, sectionType);
                break;

            case DFF_RW_MATERIALLIST:
                parseDFFSection(sectionSize, depthlevel + 1, sectionType);
                break;

            case DFF_RW_FRAMELIST:
                parseDFFSection(sectionSize, depthlevel + 1, sectionType);
                break;

            case DFF_RW_GEOMETRY:
                parseDFFSection(sectionSize, depthlevel + 1, sectionType);
                break;

            case DFF_RW_CLUMP:
                parseDFFSection(sectionSize, depthlevel + 1, sectionType);
                break;

            case DFF_RW_ATOMIC:
                parseDFFSection(sectionSize, depthlevel + 1, sectionType);
                break;

            case DFF_RW_GEOMETRYLIST:
                parseDFFSection(sectionSize, depthlevel + 1, sectionType);
                break;

            default:
                // unknown section type, skip sectionSize bytes
                File->seek(sectionSize, true);
                break;
        }
    }
}

void CDFFMeshFileLoader::processDFFData(int size, int type, int version)
{
    unsigned int i, j;

    if(type == DFF_RW_MATERIALSPLIT)
    {
        unsigned int triangleSplit, splitCount, faceCount;

        File->read(&triangleSplit, sizeof(int));
        File->read(&splitCount, sizeof(int));
        File->read(&faceCount, sizeof(int));

        if(triangleSplit != 0)
        {
            SubsAreTriStrips = true;
        }

        unsigned int faceIndex, materialIndex, vertex;

        SubFaceCounts.reallocate(splitCount);
        SubMaterialIndexes.reallocate(splitCount);

        // DFF_RW_MATERIALSPLIT overwrites our already existing indexes
        Faces.clear();
        Faces.reallocate(faceCount);

        for(i=0; i<splitCount; i++)
        {
            File->read(&faceIndex, sizeof(int));
            File->read(&materialIndex, sizeof(int));

            SubFaceCounts.push_back(faceIndex);
            SubMaterialIndexes.push_back(materialIndex);

            for(j=0; j<faceIndex; j++)
            {
                File->read(&vertex, sizeof(int));
                Faces.push_back(vertex);
            }
        }
    }

    else if(type == DFF_RW_GEOMETRY)
    {
        unsigned short flags, unknown;
        unsigned int triangleCount, vertexCount, morphTargetCount;

        File->read(&flags, sizeof(short));
        File->read(&unknown, sizeof(short));
        File->read(&triangleCount, sizeof(int));
        File->read(&vertexCount, sizeof(int));
        File->read(&morphTargetCount, sizeof(int)); // aka frame count

        if (version < DFF_VERSION_GTAVC_2)
        {
            // geometry has lighting
            // ignore lighting for now, skip 12 bytes
            File->seek(12, true);
        }

        if (flags & 8)
        {
            // geometry has color info
            // ignore colors for now, skip vertexCount*4 bytes
            File->seek(vertexCount*4, true);
        }

        if (flags & 4)
        {
            // geometry has texture coordinates

            TexCoords.reallocate(vertexCount * 2);

            float coord_x, coord_y;

            for (i=0; i < vertexCount; i++)
            {
                File->read(&coord_x, sizeof(float));
                File->read(&coord_y, sizeof(float));

                // flip x coordinate
                if(coord_x > 0.5f)
                {
                    coord_x = (0.5f + (0.5 - coord_x));
                }
                else
                {
                    coord_x = (0.5f + (coord_x + 0.5));
                }

                TexCoords.push_back(coord_x);
                TexCoords.push_back(coord_y);
            }
        }

        // get geometry faces

        Faces.reallocate(triangleCount * 3);

        unsigned short indexflags = 0, index1 = 0, index2 = 0, index3 = 0;

        for (i=0; i < triangleCount; i++)
        {
            File->read(&index2, sizeof(short));
            File->read(&index1, sizeof(short));
            File->read(&indexflags, sizeof(short));
            File->read(&index3, sizeof(short));

            Faces.push_back(index1);
            Faces.push_back(index2);
            Faces.push_back(index3);
        }

        // skip 24 bytes of extra data
        File->seek(24, true);

        // get geometry vertices

        Vertices.reallocate(vertexCount * 3);

        float vertex_x, vertex_y, vertex_z;

        for (i=0; i < vertexCount; i++)
        {
            File->read(&vertex_x, sizeof(float));
            File->read(&vertex_y, sizeof(float));
            File->read(&vertex_z, sizeof(float));

            Vertices.push_back(vertex_x);
            Vertices.push_back(vertex_y);
            Vertices.push_back(vertex_z);
        }

        if (flags & 16)
        {
            // geometry has normals info

            Normals.reallocate(vertexCount * 3);

            float normal_x, normal_y, normal_z;

            for (i=0; i < vertexCount; i++)
            {
                File->read(&normal_x, sizeof(float));
                File->read(&normal_y, sizeof(float));
                File->read(&normal_z, sizeof(float));

                Normals.push_back(normal_x);
                Normals.push_back(normal_y);
                Normals.push_back(normal_z);
            }
        }
    }
    else
    {
        // unknown data section type, skip size bytes
        File->seek(size, true);
    }
}

void CDFFMeshFileLoader::processDFFString(int size, int type, int version)
{
    char *temp = new char[size + 1];

    File->read(temp, size);

    temp[size] = 0;

    stringc str = temp;

    delete [] temp;

    if(type == DFF_RW_TEXTURE)
    {
        if(str.size() > 0)
        {
            Textures.push_back(str);
        }
    }
}

void CDFFMeshFileLoader::compileMesh()
{
    scene::SMeshBuffer* meshBuffer;

    unsigned int subFaceOffset = 0, vertexOffset = 0, facesOffset = 0, texOffset = 0;

    for(subFaceOffset=0; subFaceOffset<SubFaceCounts.size(); subFaceOffset++)
    {
        // some bounds checking
        if(texOffset >= SubMaterialIndexes.size()) {Failed = true; break;}
        if(SubMaterialIndexes[texOffset] >= Textures.size()) {Failed = true; break;}

        // add texture

        meshBuffer = new scene::SMeshBuffer();

        stringc texturePath = Parameters->getAttributeAsString("DFF_TEXTURE_PATH");

        texturePath += Textures[SubMaterialIndexes[texOffset++]];

        meshBuffer->Material.MaterialType = video::EMT_SOLID;
        // meshBuffer->Material.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
        meshBuffer->Material.Wireframe = false;
        meshBuffer->Material.Lighting = true;
        meshBuffer->Material.BackfaceCulling = false;
        meshBuffer->Material.NormalizeNormals = false;
        meshBuffer->Material.EmissiveColor.set(255, 255, 255, 255);
        // meshBuffer->Material.BilinearFilter = true;

        video::ITexture *tmpTexture = NULL;

        stringc fileName;

        fileName = texturePath + ".tga";
        if(FileSystem->existFile(fileName.c_str())) {tmpTexture = Driver->getTexture(fileName.c_str());}

        fileName = texturePath + ".jpg";
        if(FileSystem->existFile(fileName.c_str())) {tmpTexture = Driver->getTexture(fileName.c_str());}

        fileName = texturePath + ".png";
        if(FileSystem->existFile(fileName.c_str())) {tmpTexture = Driver->getTexture(fileName.c_str());}

        if(tmpTexture)
        {
            meshBuffer->getMaterial().setTexture(0, tmpTexture);
        }
        else
        {
            Logger->log("Texture does not exist", fileName.c_str(), ELL_WARNING);
        }

        // add vertexes & faces

        video::S3DVertex currentvertex;

        unsigned int face;

        meshBuffer->Indices.reallocate(meshBuffer->Indices.size() + SubFaceCounts[subFaceOffset]);
        meshBuffer->Vertices.reallocate(meshBuffer->Vertices.size() + SubFaceCounts[subFaceOffset]);

        for(vertexOffset = 0; vertexOffset < SubFaceCounts[subFaceOffset]; vertexOffset++)
        {
            face = Faces[facesOffset++];

            // bounds check
            if(Vertices.size() < ((face * 3) + 2)) {Failed = true; break;}

            currentvertex.Pos.X = Vertices[face * 3];
            currentvertex.Pos.Y = Vertices[(face * 3) + 1];
            currentvertex.Pos.Z = Vertices[(face * 3) + 2];

            // check wether we have normals
            if(Normals.size() > 0)
            {
                // bounds check
                if(Normals.size() < ((face * 3) + 2)) {Failed = true; break;}

                currentvertex.Normal.X = Normals[face * 3];
                currentvertex.Normal.Y = Normals[(face * 3) + 1];
                currentvertex.Normal.Z = Normals[(face * 3) + 2];
            }

            // bounds check
            if(TexCoords.size() < ((face * 2) + 1)) {Failed = true; break;}

            currentvertex.TCoords.X = TexCoords[face * 2];
            currentvertex.TCoords.Y = TexCoords[(face * 2) + 1];

            if(SubsAreTriStrips)
            {
                if(vertexOffset < 3)
                {
                    meshBuffer->Indices.push_back(vertexOffset);
                }
                else
                {
                    meshBuffer->Indices.push_back(vertexOffset - 2);
                    meshBuffer->Indices.push_back(vertexOffset - 1);
                    meshBuffer->Indices.push_back(vertexOffset);
                }
            }
            else
            {
                meshBuffer->Indices.push_back(vertexOffset);
            }

            meshBuffer->Vertices.push_back(currentvertex);
        }

        Mesh->addMeshBuffer(meshBuffer);
        meshBuffer->drop();
    }

    SubFaceCounts.clear();
    SubMaterialIndexes.clear();
    TexCoords.clear();
    Normals.clear();
    Faces.clear();
    Vertices.clear();
    Textures.clear();
}
