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
	CBaseEntity* pEntity = cl_entitylist->GetBaseEntity(hEntity.GetEntryIndex());

	cl_entitylist->AddListenerEntity(this);
#else
	CBaseEntity* pEntity = gEntList.GetBaseEntity(hEntity);

	gEntList.AddListenerEntity(this);
#endif // CLIENT_DLL


	m_ent_name		 = "Camera";
	m_ent_model		 = "None";
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
		pPointCameraEntity = dynamic_cast<CBasePlayer*>(cl_entitylist->GetBaseEntity(mh_parent_entity.GetEntryIndex()));
	else
		pPointCameraEntity = dynamic_cast<CPointCamera*>(cl_entitylist->GetBaseEntity(mh_parent_entity.GetEntryIndex()));
#else
	if (m_bIsPlayerCamera)
		pPointCameraEntity = dynamic_cast<CBasePlayer*>(gEntList.GetBaseEntity(mh_parent_entity));
	else
		pPointCameraEntity = dynamic_cast<CPointCamera*>(gEntList.GetBaseEntity(mh_parent_entity));

#endif // CLIENT_DLL

	QAngle eyeAngles = pPointCameraEntity->EyeAngles();

	entData_js.AddMember("ent_id", m_ent_id, allocator);


	rapidjson::Value ent_pos_js = rapidjson::Value(rapidjson::kObjectType);
	rapidjson::Value ent_rot_js = rapidjson::Value(rapidjson::kObjectType);

	rapidjson::Value ent_origin_js = rapidjson::Value(rapidjson::kObjectType);
	rapidjson::Value ent_angles_js = rapidjson::Value(rapidjson::kObjectType);


	ent_origin_js.AddMember("x", pPointCameraEntity->EyePosition().x, allocator);
	ent_origin_js.AddMember("y", pPointCameraEntity->EyePosition().y, allocator);
	ent_origin_js.AddMember("z", pPointCameraEntity->EyePosition().z, allocator);

	// converting it to a quat for transport, w is a junk value
	ent_angles_js.AddMember("w", 1.0,		  allocator);
	ent_angles_js.AddMember("x", eyeAngles.x, allocator);
	ent_angles_js.AddMember("y", eyeAngles.y, allocator);
	ent_angles_js.AddMember("z", eyeAngles.z, allocator);

	ent_pos_js.AddMember("0", ent_origin_js, allocator);
	ent_rot_js.AddMember("0", ent_angles_js, allocator);

	entData_js.AddMember("ent_pos", ent_pos_js, allocator);
	entData_js.AddMember("ent_rot", ent_rot_js, allocator);


	return entData_js;
}

