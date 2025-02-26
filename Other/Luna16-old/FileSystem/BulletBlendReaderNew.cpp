#include "IrrBlendNew.h"
#include "BulletIrrlichtUtils.h"
#include "btBulletDynamicsCommon.h"
#include "autogenerated/blender.h"
#include "bMain.h"
#include "bBlenderFile.h"

#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h"
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"


BulletBlendReaderNew::BulletBlendReaderNew(class btDynamicsWorld* destinationWorld)
:m_blendFile(0),
m_destinationWorld(destinationWorld)
{
}

BulletBlendReaderNew::~BulletBlendReaderNew()
{
	if (m_blendFile)
	{
		delete m_blendFile;
	}
}

	///if you already have a file pointer, call readFile
int		BulletBlendReaderNew::readFile(char* memoryBuffer, int fileLen, int verboseDumpAllTypes )
{
	if (m_blendFile)
	{
		delete m_blendFile;
	}

	m_blendFile = new bParse::bBlenderFile(memoryBuffer, fileLen);
	bool ok = (m_blendFile->getFlags()& bParse::FD_OK)!=0;
	if (ok)
		m_blendFile->parse(verboseDumpAllTypes);

	if (verboseDumpAllTypes)
	{
		m_blendFile->getMain()->dumpChunks(m_blendFile->getFileDNA());
	}
	return ok;
}



void	BulletBlendReaderNew::convertAllObjects(int verboseDumpAllBlocks)
{
	btAssert(m_blendFile);

	bParse::bMain* mainPtr = m_blendFile->getMain();

	// bMain contains sorted void* pointers.

	// Iterate through the scene.
	bParse::bListBasePtr *sceneBase = mainPtr->getScene();
	int numScenes = sceneBase->size();
	printf("File contains %i scene(s)\n", numScenes);

	bParse::bListBasePtr* objBase = mainPtr->getObject();
	int numObj = objBase->size();
	printf("File contains %i object(s)\n", numObj);

	bParse::bListBasePtr* camBase = mainPtr->getCamera();
	int numCam = camBase->size();
	printf("File contains %i camera(s)\n", numCam);


	Blender::FileGlobal* glob = (Blender::FileGlobal*)m_blendFile->getFileGlobal();


//#define EXTRACT_ALL_SCENES 1
#ifdef EXTRACT_ALL_SCENES
	for (int sce = 0; sce<numScenes; sce++)
	{
		// Get the scene structure.
		Blender::Scene *scene = (Blender::Scene*) sceneBase->at(sce);
#else
	{
		Blender::Scene* scene = (Blender::Scene*) glob->curscene;
#endif

		if (!scene)
		{
			printf("Warning: no default scene, perhaps an old Blender version? Taking first scene.\n");
			if (numScenes)
			{
				scene = (Blender::Scene*) sceneBase->at(0);
			}
		}
		// Loop all objects in the scene.
		Blender::Base *base = scene ? (Blender::Base*)scene->base.first : 0;

		if (!base)
		{
			printf("Warning: no scene\n");
		}


		while (base)
		{
			if (base->object)
			{
				Blender::Object *ob = base->object;
				///only process objects in active scene layer
				if (ob->lay & scene->lay)
				{
					if (verboseDumpAllBlocks)
						printf("Current object : %s\n", ob->id.name);

					switch (ob->type)
					{
					//OB_MESH
					case 1:
						{
							if (verboseDumpAllBlocks)
							{
								printf("\tObject is a mesh\n");
							}

							if (ob->data)
							{
								Blender::Mesh *me = (Blender::Mesh*)ob->data;
								if (verboseDumpAllBlocks)
								{
									printf("\t\tFound mesh data for %s\n", me->id.name);
									printf("\t\tTotal verts %i, faces %i\n", me->totvert, me->totface);
								}

								btCollisionObject* bulletObject = createBulletObject(ob);

							}
						}break;

					//OB_LAMP
					case 10:
						{

							if (verboseDumpAllBlocks)
							{
								printf ("\tObject is a lamp\n");
							}

							if (ob->data)
							{
								Blender::Lamp *la = (Blender::Lamp*)ob->data;
								if (verboseDumpAllBlocks)
								{
									printf("\t\tFound lamp data for %s\n", la->id.name);
									printf("\t\tRGB, %f,%f,%f\n", la->r, la->g, la->b);
								}

								addLight(ob);
							}

						}break;
					//OB_CAMERA
					case 11:
						{
							if (verboseDumpAllBlocks)
							{
								printf ("\tObject is a camera\n");
							}

							if (ob->data)
							{
								Blender::Camera *cam = (Blender::Camera*)ob->data;
								if (verboseDumpAllBlocks)
								{
									printf("\t\tFound camera data for %s\n", cam->id.name);
									printf("\t\t(lens, near, far) %f,%f,%f\n", cam->lens, cam->clipsta, cam->clipend);
								}

								addCamera(ob);
							}
						}break;
					}
				}
			}
			base = (Blender::Base*)base->next;
		}
	}

	createParentChildHierarchy();

	convertConstraints();

	if (verboseDumpAllBlocks)
	{
		mainPtr->dumpChunks(m_blendFile->getFileDNA());
	}

}



btCollisionObject* BulletBlendReaderNew::createBulletObject(Blender::Object* object)
{
	Blender::Mesh *me = (Blender::Mesh*)object->data;

	//let's try to create some static collision shapes from the triangle meshes
	if (me && me->mface)
	{
		btTriangleMesh* meshInterface = new btTriangleMesh();

		btVector3 minVert(1e30f,1e3f,1e30f);
		btVector3 maxVert(-1e30f,-1e30f,-1e30f);
		for (int t=0;t<me->totface;t++)
		{


			int v[4] = {me->mface[t].v1,me->mface[t].v2,me->mface[t].v3,me->mface[t].v4};

			btVector3 vtx0(me->mvert[v[0]].co.x,me->mvert[v[0]].co.y,me->mvert[v[0]].co.z);
			minVert.setMin(vtx0); maxVert.setMax(vtx0);
			btVector3 vtx1(me->mvert[v[1]].co.x,me->mvert[v[1]].co.y,me->mvert[v[1]].co.z);
			minVert.setMin(vtx1); maxVert.setMax(vtx1);
			btVector3 vtx2(me->mvert[v[2]].co.x,me->mvert[v[2]].co.y,me->mvert[v[2]].co.z);
			minVert.setMin(vtx2); maxVert.setMax(vtx2);
			meshInterface ->addTriangle(vtx0,vtx1,vtx2);

			if (v[3])
			{
				btVector3 vtx3(me->mvert[v[3]].co.x,me->mvert[v[3]].co.y,me->mvert[v[3]].co.z);
				minVert.setMin(vtx3); maxVert.setMax(vtx3);
				meshInterface ->addTriangle(vtx0,vtx3,vtx2);
			}
		}
		if (!meshInterface->getNumTriangles())
			return 0;

/* boundtype */
#define OB_BOUND_BOX		0
#define OB_BOUND_SPHERE		1
#define OB_BOUND_CYLINDER	2
#define OB_BOUND_CONE		3
#define OB_BOUND_POLYH		4
#define OB_BOUND_POLYT		5
#define OB_BOUND_DYN_MESH   6

/* ob->gameflag */
#define OB_DYNAMIC		1
//#define OB_CHILD		2
//#define OB_ACTOR		4
//#define OB_INERTIA_LOCK_X	8
//#define OB_INERTIA_LOCK_Y	16
//#define OB_INERTIA_LOCK_Z	32
//#define OB_DO_FH			64
//#define OB_ROT_FH			128
//#define OB_ANISOTROPIC_FRICTION 256
//#define OB_GHOST			512
#define OB_RIGID_BODY		1024
//#define OB_BOUNDS		2048

//#define OB_COLLISION_RESPONSE	4096//??
//#define OB_COLLISION	65536
//#define OB_SOFT_BODY	0x20000

		btVector3 localPos = (minVert+maxVert)*0.5f;
		btVector3 localSize= (maxVert-minVert)*0.5f;


		btTransform	worldTrans;
		worldTrans.setIdentity();
		worldTrans.setOrigin(btVector3(object->loc.x,object->loc.y,object->loc.z));
		//		blenderobject->loc[0]+blenderobject->dloc[0]//??

		worldTrans.getBasis().setEulerZYX(object->rot.x,object->rot.y,object->rot.z);
		btVector3 scale(object->size.x,object->size.y,object->size.z);

		if ( (object->gameflag & OB_RIGID_BODY) || (object->gameflag & OB_DYNAMIC))
		{
			//m_destinationWorld->addRigidBody(
			btCollisionShape* colShape = 0;

			switch (object->boundtype)
			{
			case OB_BOUND_SPHERE:
				{
					btScalar radius = localSize[localSize.maxAxis()];
					colShape = new btSphereShape(radius);
					break;
				};
			case OB_BOUND_BOX:
				{
					colShape = new btBoxShape(localSize);
					break;
				}
			case OB_BOUND_CYLINDER:
				{
					colShape = new btCylinderShapeZ(localSize);
					break;
				}
			case OB_BOUND_CONE:
				{
					btScalar radius = btMax(localSize[0], localSize[1]);
					btScalar height = 2.f*localSize[2];
					colShape = new btConeShapeZ(radius,height);
					break;
				}
			case OB_BOUND_POLYT:

				{
					//better to approximate it, using btShapeHull
					colShape = new btConvexTriangleMeshShape(meshInterface);
					break;
				}
			case OB_BOUND_POLYH:
			case OB_BOUND_DYN_MESH:
				{
					btGImpactMeshShape* gimpact = new btGImpactMeshShape(meshInterface);
					gimpact->postUpdate();
					colShape = gimpact;
					break;
				}

			default:
				{

				}
			};

			if (colShape)
			{
				colShape->setLocalScaling(scale);
				btVector3 inertia;
				colShape->calculateLocalInertia(object->mass,inertia);
				btRigidBody* body = new btRigidBody(object->mass,0,colShape,inertia);
				if (!(object->gameflag & OB_RIGID_BODY))
				{
					body->setAngularFactor(0.f);
				}
				body->setWorldTransform(worldTrans);
				m_destinationWorld->addRigidBody(body);
				//body->setActivationState(DISABLE_DEACTIVATION);

				void* gfxObject = createGraphicsObject(object,body);
				body->setUserPointer(gfxObject);

				return body;
			}

		} else
		{

			btCollisionShape* colShape =0;
			if (meshInterface->getNumTriangles()>0)
			{
				btBvhTriangleMeshShape* childShape = new btBvhTriangleMeshShape(meshInterface,true);

				if (scale[0]!=1. || scale[1]!=1. || scale[2]!=1.)
				{
					colShape = new btScaledBvhTriangleMeshShape(childShape,scale);
				} else
				{
					colShape = childShape;
				}

				btVector3 inertia(0,0,0);
				btRigidBody* colObj = new btRigidBody(0.f,0,colShape,inertia);
				colObj->setWorldTransform(worldTrans);
				colObj->setCollisionShape(colShape);

				m_destinationWorld->addRigidBody(colObj);

				void* gfxObject = createGraphicsObject(object,colObj);
				colObj->setUserPointer(gfxObject);

				return colObj;

			}

		}
	}

	return 0;
}




#define CONSTRAINT_TYPE_RIGIDBODYJOINT 17
#define CONSTRAINT_RB_BALL		1
#define CONSTRAINT_RB_HINGE		2
#define CONSTRAINT_RB_CONETWIST 4
#define CONSTRAINT_RB_VEHICLE	11
#define CONSTRAINT_RB_GENERIC6DOF 12

///flags
#define CONSTRAINT_DISABLE_LINKED_COLLISION 0x80

///slow search, add acceleration structure if necessary
btCollisionObject* BulletBlendReaderNew::findCollisionObject(Blender::Object* ob)
{
	int i;
	for (i=0;i<m_destinationWorld->getNumCollisionObjects();i++)
	{
		btCollisionObject* colObj = m_destinationWorld->getCollisionObjectArray()[i];
		IrrlichtMeshContainer* imc = (IrrlichtMeshContainer*)colObj->getUserPointer();
		if (!imc)
			continue;
		Blender::Object* obj2 = (Blender::Object*)imc->m_userPointer;
		if (ob == obj2)
			return colObj;
	}
	return 0;
}

void	BulletBlendReaderNew::convertConstraints()
{

	int i;

	for (i=0;i<this->m_destinationWorld->getNumCollisionObjects();i++)
	{
		btCollisionObject* colObj = m_destinationWorld->getCollisionObjectArray()[i];
		IrrlichtMeshContainer* imc = (IrrlichtMeshContainer*)colObj->getUserPointer();
		if (!imc)
			continue;
		Blender::Object* obj = (Blender::Object*)imc->m_userPointer;
		if (!obj)
			continue;

		Blender::bConstraint* constraint = (Blender::bConstraint*)obj->constraints.first;

		while (constraint)
		{
			if (constraint->type == CONSTRAINT_TYPE_RIGIDBODYJOINT)
			{
				Blender::bRigidBodyJointConstraint* rbConstraint = (Blender::bRigidBodyJointConstraint*)constraint->data;
				btVector3 pivotInA(rbConstraint->pivX,rbConstraint->pivY,rbConstraint->pivZ);
				btVector3 pivotInB(0,0,0);
				btRigidBody* rbA = btRigidBody::upcast(colObj);
				btRigidBody* rbB = 0;
				int flag = rbConstraint->flag;

				rbConstraint->tar;

				btCollisionObject* colObjB = (btCollisionObject*)findCollisionObject(rbConstraint->tar);
				if (colObjB)
				{
					rbB = btRigidBody::upcast(colObjB);
					pivotInB = rbB->getCenterOfMassTransform().inverse()(rbA->getCenterOfMassTransform()(pivotInA));
				}
				bool disableCollisionBetweenLinkedBodies = (flag & CONSTRAINT_DISABLE_LINKED_COLLISION) != 0;

				float radsPerDeg = 6.283185307179586232f / 360.f;

				//localConstraintFrameBasis
				btMatrix3x3 localCFrame;
				localCFrame.setEulerZYX(radsPerDeg*rbConstraint->axX,radsPerDeg*rbConstraint->axY,radsPerDeg*rbConstraint->axZ);
				btVector3 axisInA = localCFrame.getColumn(0);
				btVector3 axis1 = localCFrame.getColumn(1);
				btVector3 axis2 = localCFrame.getColumn(2);
				bool angularOnly = false;

				/* important: these defines need to match up with PHY_DynamicTypes headerfile */
				switch (rbConstraint->type)
				{

				case CONSTRAINT_RB_BALL:
					{
						btPoint2PointConstraint* p2p = 0;

						if (rbB)
						{
							p2p = new btPoint2PointConstraint(*rbA,*rbB,pivotInA,pivotInB);
						} else
						{
							p2p = new btPoint2PointConstraint(*rbA,pivotInA);
						}

						m_destinationWorld->addConstraint(p2p);

						break;
					}
				case CONSTRAINT_RB_HINGE:
					{
						btHingeConstraint* hinge = 0;
						if (rbB)
						{
							btVector3 axisInB = rbB ?
							(rbB->getCenterOfMassTransform().getBasis().inverse()*(rbA->getCenterOfMassTransform().getBasis() * axisInA)) :
							rbA->getCenterOfMassTransform().getBasis() * axisInA;

							hinge = new btHingeConstraint(*rbA,*rbB,pivotInA,pivotInB,axisInA,axisInB);


						} else
						{
							hinge = new btHingeConstraint(*rbA,	pivotInA,axisInA);

						}
						hinge->setAngularOnly(angularOnly);

						//m_constraints.push_back(hinge);
						m_destinationWorld->addConstraint(hinge,disableCollisionBetweenLinkedBodies);
						//hinge->setUserConstraintId(gConstraintUid++);
						//hinge->setUserConstraintType(type);
						break;
					}
				case CONSTRAINT_RB_CONETWIST:
					{
						break;
					}
				case CONSTRAINT_RB_VEHICLE:
					{
						break;
					}
				case CONSTRAINT_RB_GENERIC6DOF:
					{

						btTransform frameInA;
						btTransform frameInB;
						if (axis1.length() == 0.0)
						{
							btPlaneSpace1( axisInA, axis1, axis2 );
						}
						frameInA.getBasis().setValue( axisInA.x(), axis1.x(), axis2.x(),
													  axisInA.y(), axis1.y(), axis2.y(),
													  axisInA.z(), axis1.z(), axis2.z() );
						frameInA.setOrigin( pivotInA );
						btTransform inv = rbB->getCenterOfMassTransform().inverse();
						btTransform globalFrameA = rbA->getCenterOfMassTransform() * frameInA;
						frameInB = inv  * globalFrameA;
						bool useReferenceFrameA = true;
						btGeneric6DofSpringConstraint* genericConstraint = new btGeneric6DofSpringConstraint(*rbA,*rbB,	frameInA,frameInB,useReferenceFrameA);
						m_destinationWorld->addConstraint(genericConstraint,disableCollisionBetweenLinkedBodies);

						//if it is a generic 6DOF constraint, set all the limits accordingly
						int dof;
						int dofbit=1;
						for (dof=0;dof<6;dof++)
						{
							if (flag & dofbit)
							{
								///this access is a bloated, will probably make special cases for common arrays
								btScalar minLimit = rbConstraint->minLimit[dof];
								btScalar maxLimit = rbConstraint->maxLimit[dof];
								genericConstraint->setLimit(dof,minLimit,maxLimit);
							} else
							{
								//minLimit > maxLimit means free(disabled limit) for this degree of freedom
								genericConstraint->setLimit(dof,1,-1);
							}
							dofbit<<=1;
						}
						break;
					}

				default:
					{
						printf("unsupported rigid body constraint type\n");
					}
				}
			}
			constraint = constraint->next;
		}
	}
}

