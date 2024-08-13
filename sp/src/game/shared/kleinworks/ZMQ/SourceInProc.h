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




class SourceInProc
{
public:
	SourceInProc(const char* INPUTaddr, const char* OUTPUTaddr);
	SourceInProc();
	~SourceInProc();



	/*======Member-Variables======*/
protected:

	zsock_t*   m_sockt_INPUT;

	zsock_t*   m_sockt_OUTPUT;

	zpoller_t* m_poller;

	const char* m_INPUT_socktaddr;
	const char* m_OUTPUT_socktaddr;

	int m_INPUT_tick_count;
	int m_OUTPUT_tick_count;

	byte* m_peer_routingID;

	bool m_isReceivingInput;
	bool m_isSendingOutput;

	std::function<int(bool)> m_INPUT_funcloop;

	std::function<int(bool)> m_OUTPUT_funcloop;

public:


	zframe_t*   m_sending_data_buffer;

	const char* m_sending_metadata;


	zframe_t* m_received_data_buffer;

	zframe_t* m_received_metadata_buffer;



	/*======Member-Functions======*/

	int RunFuncLoop();


	int OutputReady(bool isInFuncLoop = false);

	int InputReady(bool isInFuncLoop = false);


	int TransferData(bool isInFuncLoop = false);

	int ReceiveData(bool isInFuncLoop = false);


};