//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"

#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4003)

#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#pragma warning(pop)

#include "czmq_pointcamera_shared.h"







CzmqPointCamera::CzmqPointCamera(CBaseHandle hEntity)
{

#ifdef CLIENT_DLL
	CBaseEntity* pEntity = cl_entitylist->GetBaseEntityFromHandle(hEntity);

	cl_entitylist->AddListenerEntity(this);
#else
	CBaseEntity* pEntity = gEntList.GetBaseEntity(hEntity);

	gEntList.AddListenerEntity(this);
#endif // CLIENT_DLL

	// we have to do this to avoid taking a reference to GetDebugName() or GetModelName()

	const char* modelname = modelinfo->GetModelName(pEntity->GetModel());

	int ent_name_len = strlen(pEntity->GetDebugName());
	int ent_modelname_len = strlen(modelname);

	char* ent_name_proxystr = new char[ent_name_len + 1];
	char* ent_modelname_proxystr = new char[ent_modelname_len + 1];

	strcpy_s(ent_name_proxystr, ent_name_len + 1, pEntity->GetDebugName());
	strcpy_s(ent_modelname_proxystr, ent_modelname_len + 1, modelname);


	m_ent_name		 = ent_name_proxystr;
	m_ent_model		 = ent_modelname_proxystr;
	m_ent_id		 = hEntity.GetSerialNumber();
	mh_parent_entity = hEntity;
	m_ent_type		 = int(ENTREC_TYPES::POINT_CAMERA);


	if (strcmp(pEntity->GetClassname(), "player") == 0) {

		CBasePlayer* pPlayerEntity = dynamic_cast<CBasePlayer*>(pEntity);

		m_bIsPlayerCamera = true;


		DevMsg(3, "KleinWorks: Camera of player named \"%s\" initialized.\n", pPlayerEntity->GetPlayerName());
	}
	else {

		m_bIsPlayerCamera = false;


		DevMsg(3, "KleinWorks: Point Camera named \"%s\" initialized.", pEntity->GetDebugName());
	}

}


CzmqPointCamera::~CzmqPointCamera()
{
	CzmqBaseEntity::~CzmqBaseEntity();
}





rapidjson::Value CzmqPointCamera::GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value entData_js = rapidjson::Value(rapidjson::kObjectType);

	CBaseEntity* pPointCameraEntity;

#ifdef CLIENT_DLL
	if (m_bIsPlayerCamera) 
		pPointCameraEntity = dynamic_cast<CBasePlayer*>(cl_entitylist->GetBaseEntityFromHandle(mh_parent_entity));
	else
		pPointCameraEntity = dynamic_cast<CPointCamera*>(cl_entitylist->GetBaseEntityFromHandle(mh_parent_entity));
#else
	if (m_bIsPlayerCamera)
		pPointCameraEntity = dynamic_cast<CBasePlayer*>(gEntList.GetBaseEntity(mh_parent_entity));
	else
		pPointCameraEntity = dynamic_cast<CPointCamera*>(gEntList.GetBaseEntity(mh_parent_entity));

#endif // CLIENT_DLL


	// avoid taking a reference to the ID string by copying it
	int	  id_strlen = strlen(CFmtStr("%d", m_ent_id)) + 1;

	char* ent_id = new char[id_strlen];
	strcpy_s(ent_id, id_strlen, CFmtStr("%d", m_ent_id).String());

	entData_js.AddMember("ent_id", rapidjson::StringRef(ent_id), allocator);


	rapidjson::Value ent_pos_js = rapidjson::Value(rapidjson::kObjectType);
	rapidjson::Value ent_rot_js = rapidjson::Value(rapidjson::kObjectType);

	rapidjson::Value ent_origin_js = rapidjson::Value(rapidjson::kObjectType);
	rapidjson::Value ent_angles_js = rapidjson::Value(rapidjson::kObjectType);


	ent_origin_js.AddMember("x", pPointCameraEntity->EyePosition().x, allocator);
	ent_origin_js.AddMember("y", pPointCameraEntity->EyePosition().y, allocator);
	ent_origin_js.AddMember("z", pPointCameraEntity->EyePosition().z, allocator);

	ent_angles_js.AddMember("x", pPointCameraEntity->EyeAngles().x, allocator);
	ent_angles_js.AddMember("y", pPointCameraEntity->EyeAngles().y, allocator);
	ent_angles_js.AddMember("z", pPointCameraEntity->EyeAngles().z, allocator);

	ent_pos_js.AddMember("0", ent_origin_js, allocator);
	ent_rot_js.AddMember("0", ent_angles_js, allocator);

	entData_js.AddMember("ent_pos", ent_pos_js, allocator);
	entData_js.AddMember("ent_rot", ent_rot_js, allocator);


	return entData_js;
}

