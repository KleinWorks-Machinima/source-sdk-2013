//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"


#include "srcIPC_EntRec.h"

using namespace srcIPC;





EntRec::EntRec(int INPUTportnumber, int OUTPUTportnumber) : srcIPC::InSourceOutSource(INPUTportnumber, OUTPUTportnumber)
{
}


EntRec::EntRec() : srcIPC::InSourceOutSource()
{
}















rapidjson::Document EntRec::FromBytesToJSON(byte* bpData)
{
	rapidjson::Document data_js;

	const char* data = reinterpret_cast<const char*>(bpData);

	data_js.Parse(data);

	return data_js;

}




zframe_t*  EntRec::FromJSONToFrame(rapidjson::Document data_js)
{

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	data_js.Accept(writer);

	const char* data = buffer.GetString();
	zframe_t* data_frame = zframe_new(data, buffer.GetSize());

	return data_frame;
}


