/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
/*
Voronoi fracture and shatter code and demo copyright (c) 2011 Alain Ducharme
  - Reset scene (press spacebar) to generate new random voronoi shattered cuboids
  - Check console for total time required to: compute and mesh all 3D shards, calculate volumes and centers of mass and create rigid bodies
  - Modify VORONOIPOINTS define below to change number of potential voronoi shards
  - Note that demo's visual cracks between voronoi shards are NOT present in the internally generated voronoi mesh!
*/

//Number of random voronoi points to generate for shattering
#define VORONOIPOINTS 100

//maximum number of objects (and allow user to shoot additional boxes)
#define MAX_PROXIES (2048)

#include "BasicDemo.h"
#include "GlutStuff.h"
///btBulletDynamicsCommon.h is the main Bullet include file, contains most common include files.
#include "btBulletDynamicsCommon.h"

#include <stdio.h> //printf debugging


void BasicDemo::getVerticesInsidePlanes(const btAlignedObjectArray<btVector3>& planes, btAlignedObjectArray<btVector3>& verticesOut, std::set<int>& planeIndicesOut)
{
	// Based on btGeometryUtil.cpp (Gino van den Bergen / Erwin Coumans)
	verticesOut.resize(0);
	planeIndicesOut.clear();
	const int numPlanes = planes.size();
	int i, j, k, l;
	for (i=0;i<numPlanes;i++)
	{
		const btVector3& N1 = planes[i];
		for (j=i+1;j<numPlanes;j++)
		{
			const btVector3& N2 = planes[j];
			btVector3 n1n2 = N1.cross(N2);
			if (n1n2.length2() > btScalar(0.0001))
			{
				for (k=j+1;k<numPlanes;k++)
				{
					const btVector3& N3 = planes[k];
					btVector3 n2n3 = N2.cross(N3);
					btVector3 n3n1 = N3.cross(N1);
					if ((n2n3.length2() > btScalar(0.0001)) && (n3n1.length2() > btScalar(0.0001) ))
					{
						btScalar quotient = (N1.dot(n2n3));
						if (btFabs(quotient) > btScalar(0.0001))
						{
							btVector3 potentialVertex = (n2n3 * N1[3] + n3n1 * N2[3] + n1n2 * N3[3]) * (btScalar(-1.) / quotient);
							for (l=0; l<numPlanes; l++)
							{
								const btVector3& NP = planes[l];
								if (btScalar(NP.dot(potentialVertex))+btScalar(NP[3]) > btScalar(0.000001))
									break;
							}
							if (l == numPlanes)
							{
								// vertex (three plane intersection) inside all planes
								verticesOut.push_back(potentialVertex);
								planeIndicesOut.insert(i);
								planeIndicesOut.insert(j);
								planeIndicesOut.insert(k);
							}
						}
					}
				}
			}
		}
	}
}

static btVector3 curVoronoiPoint; // Here for btAlignedObjectArray.quickSort pointCmp scope

static bool pointCmp(const btVector3& p1, const btVector3& p2) {
	return ((p1-curVoronoiPoint).length2() < (p2-curVoronoiPoint).length2());
}

void BasicDemo::voronoiBBShatter(const btAlignedObjectArray<btVector3>& points, const btVector3& bbmin, const btVector3& bbmax, const btQuaternion& bbq, const btVector3& bbt, btScalar matDensity) {
	// points define voronoi cells in world space (avoid duplicates)
	// bbmin & bbmax = bounding box min and max in local space
	// bbq & bbt = bounding box quaternion rotation and translation
	// matDensity = Material density for voronoi shard mass calculation
	btVector3 bbvx = quatRotate(bbq, btVector3(1.0, 0.0, 0.0));
	btVector3 bbvy = quatRotate(bbq, btVector3(0.0, 1.0, 0.0));
	btVector3 bbvz = quatRotate(bbq, btVector3(0.0, 0.0, 1.0));
	btQuaternion bbiq = bbq.inverse();
	btConvexHullComputer* convexHC = new btConvexHullComputer();
	btAlignedObjectArray<btVector3> vertices;
	btVector3 rbb, nrbb;
	btScalar nlength, maxDistance, distance;
	btAlignedObjectArray<btVector3> sortedVoronoiPoints;
	sortedVoronoiPoints.copyFromArray(points);
	btVector3 normal, plane;
	btAlignedObjectArray<btVector3> planes;
	std::set<int> planeIndices;
	std::set<int>::iterator planeIndicesIter;
	int numplaneIndices;
	int cellnum = 0;
	int i, j, k;

	int numpoints = points.size();
	for (i=0; i < numpoints ;i++) {
		curVoronoiPoint = points[i];
		btVector3 icp = quatRotate(bbiq, curVoronoiPoint - bbt);
		rbb = icp - bbmax;
		nrbb = bbmin - icp;
		planes.resize(6);
		planes[0] = bbvx; planes[0][3] = rbb.x();
		planes[1] = bbvy; planes[1][3] = rbb.y();
		planes[2] = bbvz; planes[2][3] = rbb.z();
		planes[3] = -bbvx; planes[3][3] = nrbb.x();
		planes[4] = -bbvy; planes[4][3] = nrbb.y();
		planes[5] = -bbvz; planes[5][3] = nrbb.z();
		maxDistance = SIMD_INFINITY;
		sortedVoronoiPoints.quickSort(pointCmp);
		for (j=1; j < numpoints; j++) {
			normal = sortedVoronoiPoints[j] - curVoronoiPoint;
			nlength = normal.length();
			if (nlength > maxDistance)
				break;
			plane = normal.normalized();
			plane[3] = -nlength / btScalar(2.);
			planes.push_back(plane);
			getVerticesInsidePlanes(planes, vertices, planeIndices);
			if (vertices.size() == 0)
				break;
			numplaneIndices = planeIndices.size();
			if (numplaneIndices != planes.size()) {
				planeIndicesIter = planeIndices.begin();
				for (k=0; k < numplaneIndices; k++) {
					if (k != *planeIndicesIter)
						planes[k] = planes[*planeIndicesIter];
					planeIndicesIter++;
				}
				planes.resize(numplaneIndices);
			}
			maxDistance = vertices[0].length();
			for (k=1; k < vertices.size(); k++) {
				distance = vertices[k].length();
				if (maxDistance < distance)
					maxDistance = distance;
			}
			maxDistance *= btScalar(2.);
		}
		if (vertices.size() == 0)
			continue;

		// Clean-up voronoi convex shard vertices and generate edges & faces
		convexHC->compute(&vertices[0].getX(), sizeof(btVector3), vertices.size(),0.0,0.0);

		// At this point we have a complete 3D voronoi shard mesh contained in convexHC

		// Calculate volume and center of mass (Stan Melax volume integration)
		int numFaces = convexHC->faces.size();
		int v0, v1, v2; // Triangle vertices
		btScalar volume = btScalar(0.);
		btVector3 com(0., 0., 0.);
		for (j=0; j < numFaces; j++) {
			const btConvexHullComputer::Edge* edge = &convexHC->edges[convexHC->faces[j]];
			v0 = edge->getSourceVertex();
			v1 = edge->getTargetVertex();
			edge = edge->getNextEdgeOfFace();
			v2 = edge->getTargetVertex();
			while (v2 != v0) {
				// Counter-clockwise triangulated voronoi shard mesh faces (v0-v1-v2) and edges here...
				btScalar vol = convexHC->vertices[v0].triple(convexHC->vertices[v1], convexHC->vertices[v2]);
				volume += vol;
				com += vol * (convexHC->vertices[v0] + convexHC->vertices[v1] + convexHC->vertices[v2]);
				edge = edge->getNextEdgeOfFace();
				v1 = v2;
				v2 = edge->getTargetVertex();
			}
		}
		com /= volume * btScalar(4.);
		volume /= btScalar(6.);

		// Shift all vertices relative to center of mass
		int numVerts = convexHC->vertices.size();
		for (j=0; j < numVerts; j++)
		{
			convexHC->vertices[j] -= com;
		}

		// Note:
		// At this point convex hulls contained in convexHC should be accurate (line up flush with other pieces, no cracks),
		// ...however Bullet Physics rigid bodies demo visualizations appear to produce some visible cracks.
		// Use the mesh in convexHC for visual display or to perform boolean operations with.

		// Create Bullet Physics rigid body shards
		btCollisionShape* shardShape = new btConvexHullShape(&(convexHC->vertices[0].getX()), convexHC->vertices.size());
		shardShape->setMargin(0.); // for this demo; note convexHC has optional margin parameter for this
		m_collisionShapes.push_back(shardShape);
		btTransform shardTransform;
		shardTransform.setIdentity();
		shardTransform.setOrigin(curVoronoiPoint + com); // Shard's adjusted location
		btDefaultMotionState* shardMotionState = new btDefaultMotionState(shardTransform);
		btScalar shardMass(volume * matDensity);
		btVector3 shardInertia(0.,0.,0.);
		shardShape->calculateLocalInertia(shardMass, shardInertia);
		btRigidBody::btRigidBodyConstructionInfo shardRBInfo(shardMass, shardMotionState, shardShape, shardInertia);
		btRigidBody* shardBody = new btRigidBody(shardRBInfo);
		m_dynamicsWorld->addRigidBody(shardBody);

		cellnum ++;

	}
	printf("Generated %d voronoi btRigidBody shards\n", cellnum);
}

void BasicDemo::voronoiConvexHullShatter(const btAlignedObjectArray<btVector3>& points, const btAlignedObjectArray<btVector3>& verts, const btQuaternion& bbq, const btVector3& bbt, btScalar matDensity) {
	// points define voronoi cells in world space (avoid duplicates)
	// verts = source (convex hull) mesh vertices in local space
	// bbq & bbt = source (convex hull) mesh quaternion rotation and translation
	// matDensity = Material density for voronoi shard mass calculation
	btConvexHullComputer* convexHC = new btConvexHullComputer();
	btAlignedObjectArray<btVector3> vertices, chverts;
	btVector3 rbb, nrbb;
	btScalar nlength, maxDistance, distance;
	btAlignedObjectArray<btVector3> sortedVoronoiPoints;
	sortedVoronoiPoints.copyFromArray(points);
	btVector3 normal, plane;
	btAlignedObjectArray<btVector3> planes, convexPlanes;
	std::set<int> planeIndices;
	std::set<int>::iterator planeIndicesIter;
	int numplaneIndices;
	int cellnum = 0;
	int i, j, k;

	// Convert verts to world space and get convexPlanes
	int numverts = verts.size();
	chverts.resize(verts.size());
	for (i=0; i < numverts ;i++) {
		chverts[i] = quatRotate(bbq, verts[i]) + bbt;
	}
	//btGeometryUtil::getPlaneEquationsFromVertices(chverts, convexPlanes);
	// Using convexHullComputer faster than getPlaneEquationsFromVertices for large meshes...
	convexHC->compute(&chverts[0].getX(), sizeof(btVector3), numverts, 0.0, 0.0);
	int numFaces = convexHC->faces.size();
	int v0, v1, v2; // vertices
	for (i=0; i < numFaces; i++) {
		const btConvexHullComputer::Edge* edge = &convexHC->edges[convexHC->faces[i]];
		v0 = edge->getSourceVertex();
		v1 = edge->getTargetVertex();
		edge = edge->getNextEdgeOfFace();
		v2 = edge->getTargetVertex();
		plane = (convexHC->vertices[v1]-convexHC->vertices[v0]).cross(convexHC->vertices[v2]-convexHC->vertices[v0]).normalize();
		plane[3] = -plane.dot(convexHC->vertices[v0]);
		convexPlanes.push_back(plane);
	}
	const int numconvexPlanes = convexPlanes.size();

	int numpoints = points.size();
	for (i=0; i < numpoints ;i++) {
		curVoronoiPoint = points[i];
		planes.copyFromArray(convexPlanes);
		for (j=0; j < numconvexPlanes ;j++) {
			planes[j][3] += planes[j].dot(curVoronoiPoint);
		}
		maxDistance = SIMD_INFINITY;
		sortedVoronoiPoints.quickSort(pointCmp);
		for (j=1; j < numpoints; j++) {
			normal = sortedVoronoiPoints[j] - curVoronoiPoint;
			nlength = normal.length();
			if (nlength > maxDistance)
				break;
			plane = normal.normalized();
			plane[3] = -nlength / btScalar(2.);
			planes.push_back(plane);
			getVerticesInsidePlanes(planes, vertices, planeIndices);
			if (vertices.size() == 0)
				break;
			numplaneIndices = planeIndices.size();
			if (numplaneIndices != planes.size()) {
				planeIndicesIter = planeIndices.begin();
				for (k=0; k < numplaneIndices; k++) {
					if (k != *planeIndicesIter)
						planes[k] = planes[*planeIndicesIter];
					planeIndicesIter++;
				}
				planes.resize(numplaneIndices);
			}
			maxDistance = vertices[0].length();
			for (k=1; k < vertices.size(); k++) {
				distance = vertices[k].length();
				if (maxDistance < distance)
					maxDistance = distance;
			}
			maxDistance *= btScalar(2.);
		}
		if (vertices.size() == 0)
			continue;

		// Clean-up voronoi convex shard vertices and generate edges & faces
		convexHC->compute(&vertices[0].getX(), sizeof(btVector3), vertices.size(),0.0,0.0);

		// At this point we have a complete 3D voronoi shard mesh contained in convexHC

		// Calculate volume and center of mass (Stan Melax volume integration)
		numFaces = convexHC->faces.size();
		btScalar volume = btScalar(0.);
		btVector3 com(0., 0., 0.);
		for (j=0; j < numFaces; j++) {
			const btConvexHullComputer::Edge* edge = &convexHC->edges[convexHC->faces[j]];
			v0 = edge->getSourceVertex();
			v1 = edge->getTargetVertex();
			edge = edge->getNextEdgeOfFace();
			v2 = edge->getTargetVertex();
			while (v2 != v0) {
				// Counter-clockwise triangulated voronoi shard mesh faces (v0-v1-v2) and edges here...
				btScalar vol = convexHC->vertices[v0].triple(convexHC->vertices[v1], convexHC->vertices[v2]);
				volume += vol;
				com += vol * (convexHC->vertices[v0] + convexHC->vertices[v1] + convexHC->vertices[v2]);
				edge = edge->getNextEdgeOfFace();
				v1 = v2;
				v2 = edge->getTargetVertex();
			}
		}
		com /= volume * btScalar(4.);
		volume /= btScalar(6.);

		// Shift all vertices relative to center of mass
		int numVerts = convexHC->vertices.size();
		for (j=0; j < numVerts; j++)
		{
			convexHC->vertices[j] -= com;
		}

		// Note:
		// At this point convex hulls contained in convexHC should be accurate (line up flush with other pieces, no cracks),
		// ...however Bullet Physics rigid bodies demo visualizations appear to produce some visible cracks.
		// Use the mesh in convexHC for visual display or to perform boolean operations with.

		// Create Bullet Physics rigid body shards
		btCollisionShape* shardShape = new btConvexHullShape(&(convexHC->vertices[0].getX()), convexHC->vertices.size());
		shardShape->setMargin(0.); // for this demo; note convexHC has optional margin parameter for this
		m_collisionShapes.push_back(shardShape);
		btTransform shardTransform;
		shardTransform.setIdentity();
		shardTransform.setOrigin(curVoronoiPoint + com); // Shard's adjusted location
		btDefaultMotionState* shardMotionState = new btDefaultMotionState(shardTransform);
		btScalar shardMass(volume * matDensity);
		btVector3 shardInertia(0.,0.,0.);
		shardShape->calculateLocalInertia(shardMass, shardInertia);
		btRigidBody::btRigidBodyConstructionInfo shardRBInfo(shardMass, shardMotionState, shardShape, shardInertia);
		btRigidBody* shardBody = new btRigidBody(shardRBInfo);
		m_dynamicsWorld->addRigidBody(shardBody);

		cellnum ++;

	}
	printf("Generated %d voronoi btRigidBody shards\n", cellnum);
}

void BasicDemo::clientMoveAndDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	//simple dynamics world doesn't handle fixed-time-stepping
	float ms = getDeltaTimeMicroseconds();
	
	///step the simulation
	if (m_dynamicsWorld)
	{
		m_dynamicsWorld->stepSimulation(ms / 1000000.f);
		//optional but useful: debug drawing
		m_dynamicsWorld->debugDrawWorld();
	}
		
	renderme(); 

	glFlush();

	swapBuffers();
}


void BasicDemo::displayCallback(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	renderme();

	//optional but useful: debug drawing to detect problems
	if (m_dynamicsWorld)
		m_dynamicsWorld->debugDrawWorld();

	glFlush();
	swapBuffers();
}


void	BasicDemo::initPhysics()
{
	setTexturing(true);
	setShadows(true);

	setCameraDistance(btScalar(20.));

	///collision configuration contains default setup for memory, collision setup
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	//m_collisionConfiguration->setConvexConvexMultipointIterations();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);

	m_broadphase = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	m_solver = sol;

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
	
	m_dynamicsWorld->setGravity(btVector3(0,-10,0));

	///create a few basic rigid bodies
	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));
//	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),50);
	
	m_collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,-50,0));

	//We can also use DemoApplication::localCreateRigidBody, but for clarity it is provided here:
	{
		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0,0,0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass,localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		m_dynamicsWorld->addRigidBody(body);
	}

	// ==> Voronoi Shatter Basic Demo: Random Cuboid

	// Random size cuboid (defined by bounding box max and min)
	btVector3 bbmax(btScalar(rand() / btScalar(RAND_MAX)) * 6. +0.5, btScalar(rand() / btScalar(RAND_MAX)) * 6. +0.5, btScalar(rand() / btScalar(RAND_MAX)) * 6. +0.5);
	btVector3 bbmin = -bbmax;
	// Place it 10 units above ground
	btVector3 bbt(0,10,0);
	// Use an arbitrary material density for shards (should be consitent/relative with/to rest of RBs in world)
	btScalar matDensity = 100.;
	// Using random rotation
	btQuaternion bbq(btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.,btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.,btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.,btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.);
	bbq.normalize();
	// Generate random points for voronoi cells
	btAlignedObjectArray<btVector3> points;
	btVector3 point;
	btVector3 diff = bbmax - bbmin;
	for (int i=0; i < VORONOIPOINTS; i++) {
		// Place points within box area (points are in world coordinates)
		point = quatRotate(bbq, btVector3(btScalar(rand() / btScalar(RAND_MAX)) * diff.x() -diff.x()/2., btScalar(rand() / btScalar(RAND_MAX)) * diff.y() -diff.y()/2., btScalar(rand() / btScalar(RAND_MAX)) * diff.z() -diff.z()/2.)) + bbt;
		points.push_back(point);
	}
	m_perfmTimer.reset();
	voronoiBBShatter(points, bbmin, bbmax, bbq, bbt, matDensity);
	printf("Total Time: %f seconds\n", m_perfmTimer.getTimeMilliseconds()/1000.);

}
void	BasicDemo::clientResetScene()
{
	exitPhysics();
	initPhysics();
}
	

void	BasicDemo::exitPhysics()
{

	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}
	m_collisionShapes.clear();

	delete m_dynamicsWorld;
	
	delete m_solver;
	
	delete m_broadphase;
	
	delete m_dispatcher;

	delete m_collisionConfiguration;

}
