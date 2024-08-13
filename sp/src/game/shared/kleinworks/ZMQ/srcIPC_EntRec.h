//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "cbase.h"

#include "SourceIPC.h"
#include "InSourceOutSource.h"


namespace srcIPC {


	class EntRec : public srcIPC::InSourceOutSource
	{
	public:
		using srcIPC::InSourceOutSource::InSourceOutSource;

		EntRec(int INPUTportnumber, int OUTPUTportnumber);
		EntRec();


		/*======Member-Variables======*/






		/*======Member-Functions======*/
	public:

		rapidjson::Document FromBytesToJSON(byte* bpData);

		zframe_t*           FromJSONToFrame(rapidjson::Document data_js);




	};

}