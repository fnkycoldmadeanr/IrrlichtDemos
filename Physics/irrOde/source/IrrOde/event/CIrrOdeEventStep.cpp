  #include <event/CIrrOdeEventStep.h>

namespace irr {
namespace ode {

CIrrOdeEventStep::CIrrOdeEventStep(u32 iStepNo, s32 ID) : IIrrOdeEvent() {
  this->ID=ID;
  m_iStepNo=iStepNo;
  m_iFrameNo=0;
}

CIrrOdeEventStep::CIrrOdeEventStep(CSerializer *pData, s32 ID) {
  this->ID=ID;
  pData->resetBufferPos();
  u16 iCode=pData->getU16();
  if (iCode==eIrrOdeEventStep) {
    m_iStepNo=pData->getU32();
  }
  m_iFrameNo=0;
}

s32 CIrrOdeEventStep::getID() {
  return ID;
}

u16 CIrrOdeEventStep::getType() {
  return eIrrOdeEventStep;
}

CSerializer *CIrrOdeEventStep::serialize() {
  if (m_pSerializer==NULL) {
    m_pSerializer=new CSerializer();
    m_pSerializer->addU16(eIrrOdeEventStep);
    m_pSerializer->addU32(m_iStepNo);
  }

  return m_pSerializer;
}

const c8 *CIrrOdeEventStep::toString() {
  sprintf(m_sString,"CIrrOdeEventStep: stepno=%i",m_iStepNo);
  return m_sString;
}

} //namespace ode
} //namespace irr
