#include "desteer/behavior/EvadeBehavior.hpp"

using namespace desteer;
using namespace behavior;
using namespace entity;

using namespace irr;
using namespace core;


EvadeBehavior::EvadeBehavior(IMobileEntity * target)
{
    _target = target;
    _fleeBehavior = new FleeBehavior(vector3df(0,0,0));
}

vector3df EvadeBehavior::Calculate()
{
    vector3df toTarget = _target->Position() - _mob->Position();

    float lookAheadTime = toTarget.getLength() / ( _mob->MaxSpeed() + _target->Velocity().getLength() );

    _fleeBehavior->SetTarget(_target->Position() + (_target->Velocity() * lookAheadTime));
    return _fleeBehavior->Calculate();
}

void EvadeBehavior::SetMobile(entity::IMobileEntity * mob)
{
    _mob = mob;
}

void EvadeBehavior::SetTarget(IMobileEntity *target)
{
    _target = target;
}
