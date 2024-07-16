//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"
#include "czmq_baseentity.h"

#include "mathlib/mathlib.h"




CzmqBaseEntity::CzmqBaseEntity(CBaseHandle hEntity)
{
	CBaseEntity* pEntity = gEntList.GetBaseEntity(hEntity);

	mh_parent_entity = hEntity;
	m_ent_type		 = int(ENTREC_TYPES::BASE_ENTITY);

	// we have to do this to avoid taking a reference to GetDebugName() or GetModelName()

	int ent_name_len = strlen(pEntity->GetDebugName());
	int ent_modelname_len = strlen(pEntity->GetModelName().ToCStr());


	char* ent_name_proxystr      = new char[ent_name_len + 1];
	char* ent_modelname_proxystr = new char[ent_modelname_len + 1];


	strcpy_s(ent_name_proxystr, ent_name_len + 1, pEntity->GetDebugName());
	strcpy_s(ent_modelname_proxystr, ent_modelname_len + 1, pEntity->GetModelName().ToCStr());

	
	m_ent_name  = ent_name_proxystr;
	m_ent_model = ent_modelname_proxystr;


	DevMsg(3, "KleinWorks_DEBUG: Entity of class %s initialized.\n", pEntity->GetClassname());

	gEntList.AddListenerEntity(this);
}


CzmqBaseEntity::CzmqBaseEntity()
{
	mh_parent_entity = nullptr;
	m_ent_type       = NULL;
	m_ent_name       = NULL;
	m_ent_model      = NULL;
}




CzmqBaseEntity::~CzmqBaseEntity()
{
	gEntList.RemoveListenerEntity(this);
	
	if (mh_parent_entity != NULL) {
		mh_parent_entity.Term();
		mh_parent_entity = NULL;
	}

}



rapidjson::Value CzmqBaseEntity::GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator)
{

	CBaseEntity* pEntity = gEntList.GetBaseEntity(mh_parent_entity);

	rapidjson::Value entData_js = rapidjson::Value(rapidjson::kObjectType);

	entData_js.AddMember("ent_name", rapidjson::StringRef(m_ent_name), allocator);

	// have to do this little maneuver to avoid taking a reference of VecToString

	Quaternion entQuatAngle;
	
	AngleQuaternion(pEntity->GetAbsAngles(), entQuatAngle);

	// angles MUST be in quaternions, eulers cause a lot of problems
	const char* quatAngleStr = static_cast<const char *>(CFmtStr("(%f, %f, %f, %f)", entQuatAngle.w, entQuatAngle.x, entQuatAngle.y, entQuatAngle.z));



	int absOriginStrlen  = strlen(VecToString(pEntity->GetAbsOrigin()));
	int quatAnglesStrlen = strlen(quatAngleStr);


	char* entAbsOrigin_proxystr = new char[absOriginStrlen + 1];
	char* entQuatAngles_proxystr = new char[quatAnglesStrlen + 1];


	strcpy_s(entAbsOrigin_proxystr, absOriginStrlen + 1, VecToString(pEntity->GetAbsOrigin()));
	strcpy_s(entQuatAngles_proxystr, quatAnglesStrlen + 1, quatAngleStr);

	entData_js.AddMember("VEC_ORIGIN", rapidjson::StringRef(entAbsOrigin_proxystr), allocator);
	entData_js.AddMember("QANGLE_ANGLES", rapidjson::StringRef(entQuatAngles_proxystr), allocator);
	

	return entData_js;
}



bool CzmqBaseEntity::operator == (const CzmqBaseEntity& other) const
{
	if (mh_parent_entity == other.mh_parent_entity)
		return true;
	else
		return false;
}

bool CzmqBaseEntity::operator == (const CzmqBaseEntity other) const
{
	if (mh_parent_entity == other.mh_parent_entity)
		return true;
	else
		return false;
}

bool CzmqBaseEntity::operator == (const CzmqBaseEntity* other) const
{
	if (mh_parent_entity == other->mh_parent_entity)
		return true;
	else
		return false;
}
/*
bool CzmqBaseEntity::operator ==(const std::unique_ptr<CzmqBaseEntity> other) const
{
	if (other.get() == this)
		return true;
	else
	return false;
}
*/
bool CzmqBaseEntity::operator == (const CBaseHandle other) const
{
	if (mh_parent_entity == other)
		return true;
	else
		return false;
}