#pragma once

#include <memory>
#include <irrTypes.h>
#include <vector3d.h>
#include <SColor.h>
#include <SMaterial.h>
#include <IEventReceiver.h>
#include <irrlicht.h>
#include <irrBullet.h>

class CExampleFramework : public irr::IEventReceiver {

    public:
        CExampleFramework();

        ~CExampleFramework();

        virtual bool OnEvent(const irr::SEvent& event) = 0;

        virtual void runExample() = 0;


	void createTerrain();
	IRigidBody* const addCube(const irr::core::vector3df &pos, const irr::core::vector3df &scale, irr::f32 mass, irr::core::stringc textureFile="crate.jpg") const;
        IRigidBody* const shootCube(const irr::core::vector3df &scale, irr::f32 mass, irr::core::stringc textureFile="rockwall.jpg") const;
        IRigidBody* const shootSphere(const irr::core::vector3df &scale, irr::f32 mass) const;
        IRigidBody* const createTank(const irr::core::stringw& file, const irr::core::stringw& collFile,
				     const irr::core::vector3df &pos, const irr::f32 mass) const;

        irr::scene::IParticleSystemSceneNode* const createParticleSystem(const irr::core::vector3df& dir, irr::f32 min, irr::f32 max,
									 const irr::video::SColor& darkest, const irr::video::SColor& brightest, irr::f32 minAge, irr::f32 maxAge, irr::f32 angle,
									 const irr::core::dimension2df& minSize, const irr::core::dimension2df maxSize, const irr::core::stringc& textureFile,
									 irr::f32 lifeTime, bool gravity, irr::video::E_MATERIAL_TYPE materialType, bool fadeOut) const;

        IRigidBody* const createGround() const;

        bool debugDraw;
        bool drawProperties;
        bool drawWireFrame;

    protected:
        std::shared_ptr<irr::IrrlichtDevice> device;
	std::shared_ptr<irrBulletWorld> world;

        irr::scene::ICameraSceneNode *camera;
};
