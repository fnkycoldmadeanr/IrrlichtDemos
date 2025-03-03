#include "CMeshCombiner.h"
#include <iostream>
// Rect Packer stuff

// Pack rectangles into box with dimensions sizeOfPackxsizeOfPack. Packed outputs new rectangles
void CRectPacker::pack(const irr::core::array<irr::core::rect<irr::u32> > &rects, irr::core::array<irr::core::array<SPacked> > &packed, irr::u32 sizeOfPack) {
	clear();
	mPackSize = sizeOfPack;
	for (int x = 0; x < rects.size(); x++) {
		SRect addRect;
		addRect.pos = rects[x];
		addRect.id = x;
		addRect.packed = false;
		mRects.push_back(addRect);
	}
	mRects.sort();

	while (mNumPacked < mRects.size()) {
		SRect empty;
		empty.id = -1;
		empty.children[0] = -1;
		empty.children[1] = -1;
		empty.pos = irr::core::rect<irr::u32>(0,0,mPackSize,mPackSize);
		int oldSize = mPacks.size();
		mPacks.push_back(empty);
		mRoots.push_back(oldSize);
		fill(oldSize);
	}



	for (int x = 0; x < mRoots.size(); x++) {
		irr::core::array<SPacked> arr;
		packed.push_back(arr);
		packed[x].clear();
		addPackToArray(mRoots[x],packed[x]);
	}
}

// Clear
void CRectPacker::clear()
{
    mPackSize = 0;
    mNumPacked = 0;
    mRects.clear();
    mPacks.clear();
    mRoots.clear();
}


// Fill the pack with rects
void CRectPacker::fill(int pack) {
	if (isPackValid(pack)) {
		for (int x = 0; x < mRects.size(); x++) {
			if (!mRects[x].packed) {
				if (fits(mRects[x],mPacks[pack])) {
					++mNumPacked;
					split(pack,x);
					fill(mPacks[pack].children[0]);
					fill(mPacks[pack].children[1]);
					return;
				}
			}
		}
	}
}

// Pack rect into partition to the left, top, right, or bottom
void CRectPacker::split(int pack, int rect) {
	SRect left = mPacks[pack];
	SRect right = mPacks[pack];
	SRect bottom = mPacks[pack];
	SRect top = mPacks[pack];
	int leftWidth = mRects[rect].pos.getWidth();
	int leftX = left.pos.UpperLeftCorner.X;
	int leftY = left.pos.UpperLeftCorner.Y + mRects[rect].pos.getHeight();
	int leftHeight = left.pos.getHeight() - mRects[rect].pos.getHeight();
	left.pos = irr::core::rect<irr::u32>(leftX,leftY,leftX+leftWidth,leftHeight+leftY);
	int rightY = right.pos.UpperLeftCorner.Y;
	int rightX = right.pos.UpperLeftCorner.X + mRects[rect].pos.getWidth();
	int rightWidth = right.pos.getWidth() - mRects[rect].pos.getWidth();
	int rightHeight = right.pos.getHeight();
	right.pos = irr::core::rect<irr::u32>(rightX,rightY,rightX+rightWidth,rightY+rightHeight);
	int bottomWidth = bottom.pos.getWidth()-mRects[rect].pos.getWidth();
	int bottomX = bottom.pos.UpperLeftCorner.X + mRects[rect].pos.getWidth();
	int bottomY = bottom.pos.UpperLeftCorner.Y;
	int bottomHeight = mRects[rect].pos.getHeight();
	bottom.pos = irr::core::rect<irr::u32>(bottomX,bottomY,bottomX+bottomWidth,bottomHeight+bottomY);
	int topY = top.pos.UpperLeftCorner.Y + mRects[rect].pos.getHeight();
	int topX = top.pos.UpperLeftCorner.X;
	int topWidth = top.pos.getWidth();
	int topHeight = top.pos.getHeight() - mRects[rect].pos.getHeight();
	top.pos = irr::core::rect<irr::u32>(topX,topY,topX+topWidth,topY+topHeight);

	int maxLeftRightArea = left.pos.getArea();
	if (right.pos.getArea() > maxLeftRightArea)
		maxLeftRightArea = right.pos.getArea();
	int maxTopBottomArea = top.pos.getArea();
	if (bottom.pos.getArea() > maxTopBottomArea)
		maxTopBottomArea = bottom.pos.getArea();

    if (maxLeftRightArea > maxTopBottomArea) {

        if (left.pos.getArea() > right.pos.getArea()) {
            mPacks.push_back(left);
            mPacks.push_back(right);
        }
		else {
            mPacks.push_back(right);
            mPacks.push_back(left);
        }
    }
	else {
        if (bottom.pos.getArea() > top.pos.getArea()) {
            mPacks.push_back(bottom);
            mPacks.push_back(top);
        }
		else {
            mPacks.push_back(top);
            mPacks.push_back(bottom);
        }
    }

	mPacks[pack].pos = irr::core::rect<irr::u32>(mPacks[pack].pos.UpperLeftCorner.X,mPacks[pack].pos.UpperLeftCorner.Y,mPacks[pack].pos.UpperLeftCorner.X+mRects[rect].pos.getWidth(),mPacks[pack].pos.UpperLeftCorner.Y+mRects[rect].pos.getHeight()) ;

    mPacks[pack].id = mRects[rect].id;
    mPacks[pack].children[0] = mPacks.size() - 2;
    mPacks[pack].children[1] = mPacks.size() - 1;


    mRects[rect].packed = true;
}

// Return if the rectangle fits within the second
bool CRectPacker::fits(SRect rect1, SRect rect2) {
	if (rect1.pos.getWidth() <= rect2.pos.getWidth() && rect1.pos.getHeight() <= rect2.pos.getHeight()) {
        return true;
	}
	else {

        return false;
	}

}

// Fill arrays with packs from root
void CRectPacker::addPackToArray(int pack, irr::core::array<SPacked> &addArray) const {
	if (isPackValid(pack)) {
		if (mPacks[pack].id != -1) {

			SPacked packed;
			packed.id = mPacks[pack].id;
			packed.pos = mPacks[pack].pos;
			addArray.push_back(packed);
			if (mPacks[pack].children[0] != -1)
				addPackToArray(mPacks[pack].children[0], addArray);
			if (mPacks[pack].children[1] != -1)
				addPackToArray(mPacks[pack].children[1], addArray);
		}
	}
}

// Return if pack is within boundaires
bool CRectPacker::isPackValid(int i) const {
	return i >= 0 && i < mPacks.size();
}

// Return if rect is within boundaries
bool CRectPacker::isRectValid(int i) const {
	return i >= 0 && i < mRects.size();
}

// Mesh combiner functions
CMeshCombiner::CMeshCombiner() {
	mSizingTolerance = 0.8f;
}

CMeshCombiner::CMeshCombiner(irr::f32 sizingTolerance, TEXTURE_PADDING_TECHNIQUE technique ) {
	mSizingTolerance = sizingTolerance;
	mTexturePaddingTechnique = technique;
}

// Find a good area to pack rects into. If the area of the rects divided by the area of the nearest power of two calculated is above a certain tolerance, the size is doubled
irr::core::dimension2du CMeshCombiner::findOptimalPackingArea(irr::core::array<irr::core::rect<irr::u32> > rectangles) {
	irr::u32 totalArea = 0;
	for (int x = 0; x < rectangles.size(); x++) {
		totalArea += rectangles[x].getArea();
	}
	irr::u32 smallestDimension = ceil(sqrtf(totalArea));
	irr::u32 closestPowerOfTwo = pow(2.0f,ceil(logf(smallestDimension)/logf(2)));
	if ((irr::f32)smallestDimension/(irr::f32)closestPowerOfTwo > mSizingTolerance)
		closestPowerOfTwo *= 2;
	return irr::core::dimension2du(closestPowerOfTwo,closestPowerOfTwo);
}

// Pack textures into a larger one and return it. outPositions returns the new positions of the textures in the order they appeared in the array
irr::video::ITexture* CMeshCombiner::packTextures(irr::video::IVideoDriver *driver, irr::core::array<irr::video::ITexture*> textures, irr::core::array<irr::core::position2di> &outPositions) {

	irr::video::ITexture* newTexture;
	int area = 0;
	for (int x = 0; x < textures.size(); x++) {
		area += textures[x]->getSize().getArea();
	}
	float squareDim = sqrt((float)(area*1.5));
	int nearestPow2 = ceil(log((float)squareDim)/log(2.0f));
	int safePadding = pow(2.0f,nearestPow2)/32+4;
	if (safePadding == 0)
		safePadding = 1;

	irr::core::array<irr::core::rect<irr::u32> > textureRects;
	for (int x = 0; x < textures.size(); x++) {
		textureRects.push_back(irr::core::rect<irr::u32>(0,0,textures[x]->getSize().Width+safePadding,textures[x]->getSize().Height+safePadding));
	}

	irr::core::dimension2du textureSize = findOptimalPackingArea(textureRects);

	// This is used to tell if an actual texture occupies a certain pixel
	bool *occupied = new bool[textureSize.getArea()];
	for (int i = 0; i < textureSize.getArea(); i++)
		occupied[i] = false;

	irr::core::array<irr::core::array<CRectPacker::SPacked> > packed;
	CRectPacker packer;
	packer.pack(textureRects,packed,textureSize.Height);
	irr::video::IImage* packedImage = driver->createImage(textures[0]->getColorFormat(),textureSize);
	irr::video::IImage** textureImages = new irr::video::IImage*[textures.size()];
	for (int x = 0; x < textures.size(); x++) {
		textureImages[x] = driver->createImage(textures[x],irr::core::vector2di(0,0),textures[x]->getSize());
		outPositions.push_back(irr::core::position2di(0,0));
	}
	for (int x = 0; x < packed[0].size(); x++) {
		irr::video::IImage* im = textureImages[packed[0][x].id];
		int xPos = packed[0][x].pos.UpperLeftCorner.X+safePadding/2;
		int yPos = packed[0][x].pos.UpperLeftCorner.Y+safePadding/2;
		int width = packed[0][x].pos.getWidth();
		int height = packed[0][x].pos.getWidth();

		// Anywhere the texture exists, mark the pixel as occupied
		for (int u = xPos; u < xPos+width; u++) {
			for (int v = yPos; v < yPos+height; v++) {
				occupied[u+v*textureSize.Width] = true;
			}
		}

		im->copyTo(packedImage,irr::core::vector2di(xPos,yPos));
		if (mTexturePaddingTechnique == ETPT_EXPAND) {
			for (int y = 0; y < safePadding/2; y++) {
				im->copyTo(packedImage,irr::core::vector2di(xPos-y,yPos),irr::core::rect<irr::s32>(0,0,1,textures[packed[0][x].id]->getSize().Height));
				im->copyTo(packedImage,irr::core::vector2di(xPos+y+textures[packed[0][x].id]->getSize().Width,yPos),irr::core::rect<irr::s32>(0,0,1,textures[packed[0][x].id]->getSize().Height));
				im->copyTo(packedImage,irr::core::vector2di(xPos,yPos-y),irr::core::rect<irr::s32>(0,0,textures[packed[0][x].id]->getSize().Width,1));
				im->copyTo(packedImage,irr::core::vector2di(xPos,yPos+y+textures[packed[0][x].id]->getSize().Height),irr::core::rect<irr::s32>(0,0,textures[packed[0][x].id]->getSize().Width,1));
			}
		}
		else if (mTexturePaddingTechnique == ETPT_TILE) {
			im->copyTo(packedImage,irr::core::vector2di(xPos,yPos-safePadding/2),irr::core::rect<irr::s32>(0,textures[packed[0][x].id]->getSize().Height-safePadding/2,textures[packed[0][x].id]->getSize().Width,textures[packed[0][x].id]->getSize().Height));
			im->copyTo(packedImage,irr::core::vector2di(xPos,yPos+textures[packed[0][x].id]->getSize().Height),irr::core::rect<irr::s32>(0,0,textures[packed[0][x].id]->getSize().Width,safePadding/2));
			im->copyTo(packedImage,irr::core::vector2di(xPos-safePadding/2,yPos),irr::core::rect<irr::s32>(textures[packed[0][x].id]->getSize().Width-safePadding/2,0,textures[packed[0][x].id]->getSize().Width,textures[packed[0][x].id]->getSize().Height));
			im->copyTo(packedImage,irr::core::vector2di(xPos+textures[packed[0][x].id]->getSize().Width,yPos),irr::core::rect<irr::s32>(0,0,safePadding/2,textures[packed[0][x].id]->getSize().Height));
			im->copyTo(packedImage,irr::core::vector2di(xPos-safePadding/2,yPos-safePadding/2),irr::core::rect<irr::s32>(textures[packed[0][x].id]->getSize().Width-safePadding/2,textures[packed[0][x].id]->getSize().Height-safePadding/2,textures[packed[0][x].id]->getSize().Width,textures[packed[0][x].id]->getSize().Height));
			im->copyTo(packedImage,irr::core::vector2di(xPos+textures[packed[0][x].id]->getSize().Width,yPos-safePadding/2),irr::core::rect<irr::s32>(0,textures[packed[0][x].id]->getSize().Height-safePadding/2,safePadding/2,textures[packed[0][x].id]->getSize().Height));
			im->copyTo(packedImage,irr::core::vector2di(xPos-safePadding/2,yPos+textures[packed[0][x].id]->getSize().Height),irr::core::rect<irr::s32>(textures[packed[0][x].id]->getSize().Width-safePadding/2,0,textures[packed[0][x].id]->getSize().Width,safePadding/2));
			im->copyTo(packedImage,irr::core::vector2di(xPos+textures[packed[0][x].id]->getSize().Width,yPos+textures[packed[0][x].id]->getSize().Height),irr::core::rect<irr::s32>(0,0,safePadding/2,safePadding/2));
		}
		outPositions[packed[0][x].id].set(xPos,yPos);
	}

	// Create mipmap data
	int mipmapSize = 0;
	int lowestDim = (packedImage->getDimension().Width < packedImage->getDimension().Height) ? packedImage->getDimension().Width : packedImage->getDimension().Height;
	int highestDim = (packedImage->getDimension().Width > packedImage->getDimension().Height) ? packedImage->getDimension().Width : packedImage->getDimension().Height;
	int mipmapCount = 0;
	for (mipmapCount = 0; lowestDim > 1; lowestDim/=2, highestDim /=2, mipmapCount++) {
		if (mipmapCount >= 1)
			mipmapSize += lowestDim*highestDim;
	}
	irr::u8* mipmapData = new irr::u8[mipmapSize*4];
	irr::u8* imageData = (irr::u8*)packedImage->lock();

	// To prevent bleeding artifacts, do NOT blend any unoccupied pixels with occupied pixels. If an unoccupied pixel would be blended with an occupied one, use only occupied pixels instead
	int currentIndex = 0;
	for (int i = 0; i < mipmapCount; i++) {

		int blendSize = pow(2.0,i+1);
		int currentLevelWidth = packedImage->getDimension().Width/blendSize;
		int currentLevelHeight = packedImage->getDimension().Height/blendSize;



		// Create mipmap of current level
		for (int x = 0; x < currentLevelWidth; x++) {
			for (int y = 0; y < currentLevelHeight; y++) {

				// Check to see if it has any occupied pixels that will be blended
				int occupiedPixelR, occupiedPixelG, occupiedPixelB, occupiedPixelA;
				occupiedPixelR = occupiedPixelG = occupiedPixelB = occupiedPixelA = 0;
				int occupiedPixelCount = 0;

				int unoccupiedPixelR, unoccupiedPixelG, unoccupiedPixelB, unoccupiedPixelA;
				unoccupiedPixelR = unoccupiedPixelG = unoccupiedPixelB = unoccupiedPixelA = 0;
				int unoccupiedPixelCount = 0;


				for (int u = x*blendSize; u < std::min(x*blendSize+blendSize,(int)packedImage->getDimension().Width); u++) {
					for (int v = y*blendSize; v < std::min(y*blendSize+blendSize,(int)packedImage->getDimension().Height); v++) {
						int realX = u*4;
						int realY = v*4;
						if (!occupied[u+v*packedImage->getDimension().Width]) {
							unoccupiedPixelB += imageData[realX+realY*packedImage->getDimension().Width];
							unoccupiedPixelG += imageData[realX+realY*packedImage->getDimension().Width+1];
							unoccupiedPixelR += imageData[realX+realY*packedImage->getDimension().Width+2];
							unoccupiedPixelA += imageData[realX+realY*packedImage->getDimension().Width+3];
							unoccupiedPixelCount++;
						}
						else {
							occupiedPixelB += imageData[realX+realY*packedImage->getDimension().Width];
							occupiedPixelG += imageData[realX+realY*packedImage->getDimension().Width+1];
							occupiedPixelR += imageData[realX+realY*packedImage->getDimension().Width+2];
							occupiedPixelA += imageData[realX+realY*packedImage->getDimension().Width+3];
							occupiedPixelCount++;
						}

					}
				}

				if (occupiedPixelCount == 0 && unoccupiedPixelCount != 0) {
					unoccupiedPixelB /= unoccupiedPixelCount;
					unoccupiedPixelG /= unoccupiedPixelCount;
					unoccupiedPixelR /= unoccupiedPixelCount;
					unoccupiedPixelA /= unoccupiedPixelCount;
					mipmapData[currentIndex+x*4+y*4*currentLevelWidth] = unoccupiedPixelB;
					mipmapData[currentIndex+x*4+y*4*currentLevelWidth+1] = unoccupiedPixelG;
					mipmapData[currentIndex+x*4+y*4*currentLevelWidth+2] = unoccupiedPixelR;
					mipmapData[currentIndex+x*4+y*4*currentLevelWidth+3] = unoccupiedPixelA;
				}

				else if (occupiedPixelCount != 0) {
					occupiedPixelB /= occupiedPixelCount;
					occupiedPixelG /= occupiedPixelCount;
					occupiedPixelR /= occupiedPixelCount;
					occupiedPixelA /= occupiedPixelCount;
					mipmapData[currentIndex+x*4+y*4*currentLevelWidth] = occupiedPixelB;
					mipmapData[currentIndex+x*4+y*4*currentLevelWidth+1] = occupiedPixelG;
					mipmapData[currentIndex+x*4+y*4*currentLevelWidth+2] = occupiedPixelR;
					mipmapData[currentIndex+x*4+y*4*currentLevelWidth+3] = occupiedPixelA;
				}

			}
		}

		currentIndex += (currentLevelWidth*currentLevelHeight)*4;
	}

	packedImage->unlock();

	irr::core::stringc textureName = "PackedTexture";
	newTexture = driver->addTexture(textureName, packedImage, mipmapData);
	textureName += globalPackedTextureCount;
	globalPackedTextureCount++;
	delete [] occupied;
	for (int x = 0; x < textures.size(); x++) {
		textureImages[x]->drop();
	}
	delete [] textureImages;
	packedImage->drop();
	return newTexture;
}

// Combines mesh scene nodes into one large mesh (does apply transformations)
irr::scene::IMesh* CMeshCombiner::combineMeshes(irr::scene::ISceneManager* smgr, irr::core::array<irr::scene::IMeshSceneNode*> nodes, const irr::core::stringc & meshNameInCache, bool clearNodesAfterUse, bool useHardwareMappingHint) {

	irr::video::IVideoDriver* driver = smgr->getVideoDriver();
	irr::scene::SMesh* newMesh = new irr::scene::SMesh();

	irr::scene::SMeshBuffer** meshBuffer;

	irr::core::array<irr::video::S3DVertex> *vertices;
	irr::core::array<irr::u16> *indices;

	irr::u32 currentIndex = 0;
	irr::u8 meshBufferOn = 0;

	irr::u32 indexTest = 0;
	irr::u8 meshBuffersNeeded = 1;
	irr::u16 indexMax = -1;

	irr::core::array<irr::video::ITexture*> textures;
	for (int x = 0; x < nodes.size(); x++) {
		nodes[x]->updateAbsolutePosition();
		for (int y = 0; y < nodes[x]->getMesh()->getMeshBufferCount(); y++) {
			indexTest += nodes[x]->getMesh()->getMeshBuffer(y)->getVertexCount();
			bool newMb = false;
			if (indexTest >= indexMax) {
				meshBuffersNeeded++;
				indexTest = 0;
				y--;
				newMb = true;
			}
			if (!newMb) {
				irr::video::ITexture* tex = nodes[x]->getMaterial(y).getTexture(0);
				if (tex != 0) {
					bool copy = false;
					for (int z = 0; z < textures.size(); z++) {
						if (tex == textures[z])
							copy = true;
					}
					if (!copy)
						textures.push_back(tex);
				}
			}
		}
	}

	irr::core::array<irr::core::position2di> newTexPositions;
	irr::video::ITexture* masterTexture = packTextures(driver,textures,newTexPositions);

	meshBuffer = new irr::scene::SMeshBuffer*[meshBuffersNeeded];
	vertices = new irr::core::array<irr::video::S3DVertex>[meshBuffersNeeded];
	indices = new irr::core::array<irr::u16>[meshBuffersNeeded];
	irr::video::SMaterial newMaterial = nodes[0]->getMesh()->getMeshBuffer(0)->getMaterial();
	newMaterial.setTexture(0,masterTexture);
	newMaterial.TextureLayer[0].LODBias = -8;
	newMaterial.TextureLayer[0].TrilinearFilter = true;

	float convertX = 1.0f/(float)masterTexture->getSize().Width;
	float convertY = 1.0f/(float)masterTexture->getSize().Height;

	for (int x = 0; x < nodes.size(); x++) {
		for (int y = 0; y < nodes[x]->getMesh()->getMeshBufferCount(); y++) {

			int texID = -1;
			for (int z = 0; z < textures.size(); z++) {
				if (nodes[x]->getMaterial(y).getTexture(0) == textures[z]) {
					texID = z;
					break;
				}
			}

			irr::scene::IMeshBuffer* currentMeshBuffer = nodes[x]->getMesh()->getMeshBuffer(y);
			irr::video::S3DVertex* meshBufferVertices = (irr::video::S3DVertex*)currentMeshBuffer->getVertices();

			if (currentIndex + currentMeshBuffer->getVertexCount() >= indexMax) {
				currentIndex = 0;
				meshBufferOn++;
			}

			for (int u = 0; u < currentMeshBuffer->getVertexCount(); u++) {
				irr::video::S3DVertex vert(meshBufferVertices[u].Pos,meshBufferVertices[u].Normal,meshBufferVertices[u].Color,meshBufferVertices[u].TCoords);
				if (texID >= 0) {
					float realTexX = vert.TCoords.X*textures[texID]->getSize().Width;
					float realTexY = vert.TCoords.Y*textures[texID]->getSize().Height;
					realTexX += newTexPositions[texID].X;
					realTexY += newTexPositions[texID].Y;
					irr::f32 uCoord = realTexX/(float)masterTexture->getSize().Width;
					irr::f32 vCoord = realTexY/(float)masterTexture->getSize().Height;
					vert.TCoords.X = uCoord;
					vert.TCoords.Y = vCoord;

				}
				irr::core::vector3df newPos = vert.Pos;
				irr::core::vector3df newNormal = vert.Normal;
				nodes[x]->getAbsoluteTransformation().transformVect(newPos);
				nodes[x]->getAbsoluteTransformation().rotateVect(newNormal);
				newNormal = newNormal.normalize();
				vert.Pos = newPos;
				vert.Normal = newNormal;
				vertices[meshBufferOn].push_back(vert);
			}

			for (int u = 0; u < currentMeshBuffer->getIndexCount(); u++) {
				irr::u16 index = currentMeshBuffer->getIndices()[u]+currentIndex;
				indices[meshBufferOn].push_back(index);
			}

			currentIndex += currentMeshBuffer->getVertexCount();
		}
	}

	for (int x = 0; x < meshBuffersNeeded; x++) {
		meshBuffer[x] = new irr::scene::SMeshBuffer();
		meshBuffer[x]->append(vertices[x].pointer(),vertices[x].size(),indices[x].pointer(),indices[x].size());
		meshBuffer[x]->Material = newMaterial;
		meshBuffer[x]->recalculateBoundingBox();
		newMesh->addMeshBuffer(meshBuffer[x]);
	}

	if (clearNodesAfterUse) {
		for (int x = 0; x < nodes.size(); x++) {
			nodes[x]->remove();
		}
		nodes.clear();
	}
	newMesh->recalculateBoundingBox();
	if (useHardwareMappingHint)
		newMesh->setHardwareMappingHint(irr::scene::EHM_STATIC);
	delete [] meshBuffer;
	vertices->clear();
	indices->clear();
	delete [] vertices;
	delete [] indices;
    smgr->getMeshCache()->addMesh(irr::io::path(meshNameInCache.c_str()),(irr::scene::IAnimatedMesh*)newMesh);
    newMesh->drop();
	return newMesh;
}

// Set tolerance for which the size of the texture atlas should be double to safely fit all textures
void CMeshCombiner::setSizingTolerance(irr::f32 tolerance) {
	mSizingTolerance = tolerance;
}

// Get sizing tolerance
irr::f32 CMeshCombiner::getSizingTolerance() {
	return mSizingTolerance;
}

// Set texture padding technique. ETPT_EXPAND copies and expands pixels on edges, while ETPT_TILE blends with the opposite edge
void CMeshCombiner::setTexturePaddingTechnique(TEXTURE_PADDING_TECHNIQUE technique) {
	mTexturePaddingTechnique = technique;
}

// Get method of texture padding
TEXTURE_PADDING_TECHNIQUE CMeshCombiner::getTexturePaddingTechnique() {
	return mTexturePaddingTechnique;
}

