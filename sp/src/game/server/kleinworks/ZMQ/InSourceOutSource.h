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





namespace srcIPC
{

	class InSourceOutSource
	{
	public:
		InSourceOutSource(int INPUTportnumber, int OUTPUTportnumber);
		InSourceOutSource();
		virtual ~InSourceOutSource();



		/*======Member-Variables======*/
	protected:

		zsock_t*   m_sockt_INPUT;

		zsock_t*   m_sockt_OUTPUT;

		zpoller_t* m_poller;

		//  The address that INPUT is bound to.
		const char* m_INPUT_socktaddr;
		//  The address that OUTPUT is connected to.
		const char* m_OUTPUT_socktaddr;

		//  The routing ID of INPUT's peer.
		byte* m_peer_routingID;


		//  Run every tick, functions like OutputReady or Transfer/ReceiveData
		//  need to loop multiple times to function.
		//  If INPUT needs a function looped, it will automatically set this variable
		//  to a pointer of the function it needs looped.
		std::function<int(bool)> m_INPUT_funcloop;

		//  Run every tick, functions like OutputReady or Transfer/ReceiveData
		//  need to loop multiple times to function.
		//  If OUTPUT needs a function looped, it will automatically set this variable
		//  to a pointer of the function it needs looped.
		std::function<int(bool)> m_OUTPUT_funcloop;
	public:
		//  Only affects INPUT, if the peer is done transfering data to us this
		//  variable will be true.
		bool m_peerIsDoneTransfering;

		//  Only affects OUTPUT, represents the last tick that our peer confirmed
		//  they received.
		int m_lastTickReceivedByPeer;

		//  Only affects INPUT, represents the final tick sent by our peer, this is
		//  important for checking if we have received our peer's entire data transfer.
		int m_lastTickReceivedFromPeer;


	public:

		//  Represents the current tick that INPUT has received, this increments by one
		//  for every DATA_MESSAGE received by INPUT, and resets back to 0 once data transfer
		//  is done.
		int m_INPUT_tick_count;

		//  Represents how many DATA_MESSAGEs OUTPUT has sent, this increments by one
		//  for every DATA_MESSAGE sent, and resets back to 0 once data transfer is done.
		int m_OUTPUT_tick_count;

		//  Set to 1 by default, this is how many miliseconds our sockets will wait when
		//  polling for incoming messages.
		int m_pollerTimeout;

		//  Set to 50 by default, this is how many ticks of difference are allowed between
		//  peer DATA_RECEIVED confirmation messages and current OUTPUT tick count.
		int m_drop_out_tolerance;

		//  Only affects INPUT, is set to True if INPUT is currently receiving data from a peer.
		bool m_isReceivingInput;

		//  Only affects OUTPUT, is set to True if OUTPUT is currently sending data to a peer.
		bool m_isSendingOutput;

		//  Only affects OUTPUT, is set to True if OUTPUT is done sending data to it's peer.
		bool m_isDoneTransfering;


		//  This is the data that OUTPUT will send during TransferData, TransferData will set
		//  this to NULL after it finishes sending it, which means it must be updated every tick
		//  or before TransferData is called or TransferData will halt.
		//  Gets destroyed once a data transfer finishes.
		zframe_t*   m_sending_data_buffer;

		//  This is the metadata of the data OUTPUT will be sending during TransferData, has information
		//  about what the data is and how the peer is intended to use it. 
		//	Must be set to a value before calling TransferData, or else TransferData will halt.
		//  Gets set to NULL once a data transfer finishes.
		const char* m_sending_metadata;


		//  This represents the data received by INPUT during a data transfer, gets set to a new value
		//  every time ReceiveData is called, so it must be stored/used every tick or before ReceiveData is called.
		//  Gets destroyed once a data transfer finishes.
		zframe_t* m_received_data_buffer;

		//  This represents the metadata received by INPUT during a data transfer, only gets set to a new value
		//	at the start of new data transfers, so it must be used/stored before a new data transfer is initiated.
		//  Gets destroyed once a data transfer finishes.
		zframe_t* m_received_metadata_buffer;



		/*======Member-Functions======*/
	public:
		//  Calls m_OUTPUT_funcloop and m_INPUT_funcloop.
		//  If a called funcloop returns 0, it is assumed to have finished and that funcloop is set to NULL,
		//  if a called funcloop returns -1 it is assumed to still be working and that funcloop retains its value.
		int RunFuncLoop();


		int OutputReady(bool isInFuncLoop = false);

		int InputReady(bool isInFuncLoop = false);


		int TransferData(bool isInFuncLoop = false);

		int ReceiveData(bool isInFuncLoop = false);



		int ConnectSockets(int INPUTportnumber, int OUTPUTportnumber);

		int DisconnectSockets();

	protected:


		std::tuple<src_IPC_MESSAGE, zmsg_t*> PollSocketForMessages(zsock_t* socket);




	};


}







