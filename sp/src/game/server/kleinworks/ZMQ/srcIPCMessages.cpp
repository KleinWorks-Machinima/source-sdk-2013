//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"



#include "srcIPCMessages.h"

using namespace srcIPCMessage;






/*OUTPUT messages*/


struct OutputReady_t
{

	OutputReady_t::OutputReady_t() {

	}

	zmsg_t* New() {

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

	TransferingData_t::TransferingData_t() {

	}

	zmsg_t* New(int tickCount, zframe_t* metaData_frame) {

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

	DataMessage_t::DataMessage_t() {

	}

	zmsg_t* New(int tickCount, zchunk_t* data) {

		zmsg_t* dataMessage = zmsg_new();

		src_IPC_MESSAGE msgType = src_IPC_MESSAGE::DATA_MESSAGE;


		zframe_t* msgType_frame = zframe_new(&msgType, sizeof(msgType));

		zframe_t* tickCount_frame = zframe_new(&tickCount, sizeof(tickCount));

		zframe_t* data_frame = zframe_new(data, zchunk_size(data));


		zmsg_append(dataMessage, &msgType_frame);
		zmsg_append(dataMessage, &tickCount_frame);
		zmsg_append(dataMessage, &data_frame);

		zframe_destroy(&msgType_frame);
		zframe_destroy(&tickCount_frame);
		zframe_destroy(&data_frame);

		zchunk_destroy(&data);

		return dataMessage;

	}
};


/*INPUT messages*/


struct InputReady_t
{
	InputReady_t::InputReady_t() {

	}

	zmsg_t* New(zframe_t* peerRoutingID_frame) {

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
	ReceivedData_t::ReceivedData_t() {

	}

	zmsg_t* New(int tickCount, zframe_t* peerRoutingID_frame) {

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











