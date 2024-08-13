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









const enum class src_IPC_MESSAGE : int
{
	/*ERROR messages*/

	NO_MESSAGE = 0xE00,


	/*OUTPUT messages*/

	OUTPUT_DISCONNECT = 0xA00,

	OUTPUT_READY = 0xA01,

	TRANSFERING_DATA = 0xA02,

	DATA_MESSAGE = 0xA03,

	TRANSFERING_DONE = 0xA04,


	/*INPUT messages*/

	INPUT_DISCONNECT = 0xB00,

	INPUT_READY = 0xB01,

	RECEIVED_DATA = 0xB02

};



const enum class src_IPC_PROTOCOL_TYPES : int
{

	ENTREC = 0xF01

};



namespace srcIPCMessage
{

	/*OUTPUT messages*/



	struct OutputDisconnect_t
	{

		static zmsg_t* New() {

			src_IPC_MESSAGE msgType = src_IPC_MESSAGE::OUTPUT_DISCONNECT;

			zframe_t* msgType_frame = zframe_new(&msgType, sizeof(msgType));
			zmsg_t* outputDisconnectMsg = zmsg_new();

			zmsg_append(outputDisconnectMsg, &msgType_frame);


			zframe_destroy(&msgType_frame);

			return outputDisconnectMsg;
		}


	};



	struct OutputReady_t
	{

		static zmsg_t* New() {

			src_IPC_MESSAGE msgType = src_IPC_MESSAGE::OUTPUT_READY;

			zframe_t* msgType_frame = zframe_new(&msgType, sizeof(msgType));
			zmsg_t* outputReadyMsg = zmsg_new();

			zmsg_append(outputReadyMsg, &msgType_frame);


			zframe_destroy(&msgType_frame);

			return outputReadyMsg;
		}


	};



	struct TransferingData_t
	{

		static zmsg_t* New(int tickCount, zframe_t* metaData_frame) {

			src_IPC_MESSAGE msgType = src_IPC_MESSAGE::TRANSFERING_DATA;

			zmsg_t* transferringDataMsg = zmsg_new();

			zframe_t* msgType_frame = zframe_new(&msgType, sizeof(msgType));

			zframe_t* tickCount_frame = zframe_new(&tickCount, sizeof(tickCount));


			zmsg_append(transferringDataMsg, &msgType_frame);
			zmsg_append(transferringDataMsg, &tickCount_frame);
			zmsg_append(transferringDataMsg, &metaData_frame);

			zframe_destroy(&msgType_frame);
			zframe_destroy(&tickCount_frame);
			zframe_destroy(&metaData_frame);

			return transferringDataMsg;
		}

	};



	struct DataMessage_t
	{

		static zmsg_t* New(int tickCount, zframe_t* data_frame) {


			zmsg_t* dataMessage = zmsg_new();

			src_IPC_MESSAGE msgType = src_IPC_MESSAGE::DATA_MESSAGE;


			zframe_t* msgType_frame = zframe_new(&msgType, sizeof(msgType));

			zframe_t* tickCount_frame = zframe_new(&tickCount, sizeof(tickCount));


			zmsg_append(dataMessage, &msgType_frame);
			zmsg_append(dataMessage, &tickCount_frame);
			zmsg_append(dataMessage, &data_frame);

			zframe_destroy(&msgType_frame);
			zframe_destroy(&tickCount_frame);
			zframe_destroy(&data_frame);

			return dataMessage;

		}
	};



	struct TransferingDone_t
	{

		static zmsg_t* New(int tickCount) {

			src_IPC_MESSAGE msgType = src_IPC_MESSAGE::TRANSFERING_DONE;

			zframe_t* msgType_frame = zframe_new(&msgType, sizeof(msgType));

			zframe_t* tickCount_frame = zframe_new(&tickCount, sizeof(tickCount));

			zmsg_t* transferingDoneMsg = zmsg_new();

			zmsg_append(transferingDoneMsg, &msgType_frame);
			zmsg_append(transferingDoneMsg, &tickCount_frame);


			zframe_destroy(&msgType_frame);
			zframe_destroy(&tickCount_frame);

			return transferingDoneMsg;
		}


	};





	/*INPUT messages*/



	struct InputDisconnect_t
	{

		static zmsg_t* New(zframe_t* peerRoutingID_frame) {

			src_IPC_MESSAGE msgType = src_IPC_MESSAGE::INPUT_DISCONNECT;

			zframe_t* msgType_frame = zframe_new(&msgType, sizeof(msgType));
			zmsg_t* inputDisconnectMsg = zmsg_new();

			zmsg_prepend(inputDisconnectMsg, &peerRoutingID_frame);
			zmsg_append(inputDisconnectMsg, &msgType_frame);

			zframe_destroy(&msgType_frame);

			return inputDisconnectMsg;
		}
	};



	struct InputReady_t
	{

		static zmsg_t* New(zframe_t* peerRoutingID_frame) {

			src_IPC_MESSAGE msgType = src_IPC_MESSAGE::INPUT_READY;

			zframe_t* msgType_frame = zframe_new(&msgType, sizeof(msgType));
			zmsg_t* inputReadyMsg = zmsg_new();

			zmsg_prepend(inputReadyMsg, &peerRoutingID_frame);
			zmsg_append(inputReadyMsg, &msgType_frame);

			zframe_destroy(&msgType_frame);

			return inputReadyMsg;
		}
	};



	struct ReceivedData_t
	{

		static zmsg_t* New(int tickCount, zframe_t* peerRoutingID_frame) {

			src_IPC_MESSAGE msgType = src_IPC_MESSAGE::RECEIVED_DATA;

			zmsg_t* receivedDataMsg = zmsg_new();


			zframe_t* msgType_frame = zframe_new(&msgType, sizeof(msgType));

			zframe_t* tickCount_frame = zframe_new(&tickCount, sizeof(tickCount));

			zmsg_append(receivedDataMsg, &peerRoutingID_frame);
			zmsg_append(receivedDataMsg, &msgType_frame);
			zmsg_append(receivedDataMsg, &tickCount_frame);


			zframe_destroy(&msgType_frame);
			zframe_destroy(&tickCount_frame);


			return receivedDataMsg;
		}
	};


}






