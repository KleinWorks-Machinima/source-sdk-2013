//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"


#include "InSourceOutSource.h"

using namespace srcIPC;





InSourceOutSource::InSourceOutSource(int INPUTportnumber, int OUTPUTportnumber)
{
	m_sockt_INPUT = zsock_new(ZMQ_ROUTER);
	m_sockt_OUTPUT = zsock_new(ZMQ_DEALER);

	zsock_set_hwm(m_sockt_INPUT, -1);
	zsock_set_hwm(m_sockt_OUTPUT, -1);

	std::string INPUTaddr("tcp://*:");
	INPUTaddr.append(std::to_string((INPUTportnumber)));

	std::string OUTPUTaddr("tcp://localhost:");
	OUTPUTaddr.append(std::to_string((OUTPUTportnumber)));


	// Allocate memory for the destination pointers
	char* proxyINPUTaddr = new char[INPUTaddr.length() + 1];
	char* proxyOUTPUTaddr = new char[OUTPUTaddr.length() + 1];



	// Copy the strings using std::strcpy
	strcpy_s(proxyINPUTaddr, INPUTaddr.length() + 1, INPUTaddr.c_str());
	strcpy_s(proxyOUTPUTaddr, OUTPUTaddr.length() + 1, OUTPUTaddr.c_str());
	


	m_INPUT_socktaddr = proxyINPUTaddr;
	m_OUTPUT_socktaddr = proxyOUTPUTaddr;
	

	int rc;

	rc = zsock_bind(m_sockt_INPUT, INPUTaddr.c_str());
	assert(rc != -1);
	rc = zsock_connect(m_sockt_OUTPUT, OUTPUTaddr.c_str());
	assert(rc != -1);


	m_poller = zpoller_new(NULL);

	rc = zpoller_add(m_poller, m_sockt_INPUT);
	assert(rc != -1);
	rc = zpoller_add(m_poller, m_sockt_OUTPUT);
	assert(rc != -1);

	m_isReceivingInput = false;
	m_isSendingOutput  = false;

	m_INPUT_tick_count  = 0;
	m_OUTPUT_tick_count = 0;

	m_pollerTimeout		 = 1;
	m_drop_out_tolerance = 30;

	m_lastTickReceivedByPeer   = 0;
	m_lastTickReceivedFromPeer = 0;
	

	//printf("KleinWorks: InSourceOutSource initialization completed.\n");

}


InSourceOutSource::InSourceOutSource()
{
	m_sockt_INPUT = zsock_new(ZMQ_ROUTER);
	m_sockt_OUTPUT = zsock_new(ZMQ_DEALER);

	zsock_set_hwm(m_sockt_INPUT, -1);
	zsock_set_hwm(m_sockt_OUTPUT, -1);

	int rc;


	m_poller = zpoller_new(NULL);

	rc = zpoller_add(m_poller, m_sockt_INPUT);
	assert(rc != -1);
	rc = zpoller_add(m_poller, m_sockt_OUTPUT);
	assert(rc != -1);



	m_isReceivingInput = false;
	m_isSendingOutput  = false;

	m_INPUT_tick_count  = 0;
	m_OUTPUT_tick_count = 0;


	m_pollerTimeout	     = 1;
	m_drop_out_tolerance = 30;



	m_lastTickReceivedByPeer   = 0;
	m_lastTickReceivedFromPeer = 0;

	

	//printf("KleinWorks: InSourceOutSource initialization completed.\n");
}


InSourceOutSource::~InSourceOutSource()
{
	/*
	if (m_INPUT_socktaddr != NULL || m_OUTPUT_socktaddr != NULL || m_isReceivingInput != false || m_isSendingOutput != false) {
	DisconnectSockets();
	}


	if (m_sockt_INPUT != nullptr) 
		if (zsock_is(m_sockt_INPUT) == true) {
			zsock_destroy(&m_sockt_INPUT);
			m_sockt_INPUT = nullptr;
		}
	
	if (m_sockt_OUTPUT != nullptr)
		if (zsock_is(m_sockt_OUTPUT) == true) {
			zsock_destroy(&m_sockt_OUTPUT);
			m_sockt_OUTPUT = nullptr;
		}



	if (m_poller != nullptr) {
		zpoller_destroy(&m_poller);
	}
	

	/*
	if (zframe_is(m_sending_data_buffer) == true) {
		zframe_destroy(&m_sending_data_buffer);
	}

	if (zframe_is(m_received_data_buffer) == true) {
		zframe_destroy(&m_received_data_buffer);
	}

	if (zframe_is(m_received_metadata_buffer) == true) {
		zframe_destroy(&m_received_metadata_buffer);
	}
	*/

	//printf("KleinWorks: InSourceOutSource destruction completed.\n");
}








int InSourceOutSource::ConnectSockets(int INPUTportnumber, int OUTPUTportnumber)
{
	if (m_INPUT_socktaddr != NULL || m_OUTPUT_socktaddr != NULL || m_isReceivingInput != false || m_isSendingOutput != false) {
		DisconnectSockets();
	}

	std::string INPUTaddr("tcp://*:");
	INPUTaddr.append(std::to_string((INPUTportnumber)));

	std::string OUTPUTaddr("tcp://localhost:");
	OUTPUTaddr.append(std::to_string((OUTPUTportnumber)));


	int rc;

	rc = zsock_bind(m_sockt_INPUT, INPUTaddr.c_str());
	assert(rc != -1);
	rc = zsock_connect(m_sockt_OUTPUT, OUTPUTaddr.c_str());
	assert(rc != -1);


	return 0;

	DevMsg(3, "KleinWorks_DEBUG: InSourceOutSource ConnectSockets completed.\n");
}



int InSourceOutSource::DisconnectSockets()
{

	int rc;

	if (m_isReceivingInput != false) {

		zframe_t* peerRoutingID = zframe_new(m_peer_routingID, 5);

		zmsg_t* inputDisconnectMessage = srcIPCMessage::InputDisconnect_t::New(peerRoutingID);

		rc = zmsg_send(&inputDisconnectMessage, m_sockt_INPUT);
		assert(rc != -1);

		zclock_sleep(10);
	}

	if (m_isSendingOutput != false) {

		zmsg_t* outputDisconnectMessage = srcIPCMessage::OutputDisconnect_t::New();


		rc = zmsg_send(&outputDisconnectMessage, m_sockt_OUTPUT);
		assert(rc != -1);

		zclock_sleep(10);
	}

	
	
	rc = zsock_unbind(m_sockt_INPUT, zsock_last_endpoint(m_sockt_INPUT));
	assert(rc != -1);



	rc = zsock_disconnect(m_sockt_OUTPUT, zsock_last_endpoint(m_sockt_OUTPUT));
	assert(rc != -1);

	m_isReceivingInput = false;
	m_isSendingOutput  = false;

	m_INPUT_tick_count  = 0;
	m_OUTPUT_tick_count = 0;

	m_isDoneTransfering		= false;
	m_peerIsDoneTransfering = false;

	m_INPUT_socktaddr = NULL;
	m_OUTPUT_socktaddr = NULL;

	DevMsg(3, "KleinWorks_DEBUG: InSourceOutSource DisconnectSockets completed.\n");

	return 0;
}




int InSourceOutSource::RunFuncLoop()
{
	//printf("Beginning funcloop...\n");

	if (m_INPUT_funcloop == NULL && m_OUTPUT_funcloop == NULL)
		return -1;

	if (m_OUTPUT_funcloop != nullptr) {
		if (m_OUTPUT_funcloop(true) == 0)
			m_OUTPUT_funcloop = nullptr;
	}


	if (m_INPUT_funcloop != nullptr) {
		if (m_INPUT_funcloop(true) == 0)
			m_INPUT_funcloop = nullptr;
	}

	//printf("Finished funcloop.\n");



	return 0;
}





int InSourceOutSource::OutputReady(bool isInFuncLoop)
{
	if (!isInFuncLoop) {
		if (m_OUTPUT_funcloop == nullptr)
			m_OUTPUT_funcloop = std::bind(&InSourceOutSource::OutputReady, this, std::placeholders::_1);

		zmsg_t* outputReadyMessage = srcIPCMessage::OutputReady_t::New();

		if (zmsg_send(&outputReadyMessage, m_sockt_OUTPUT) != 0) {
			//printf("\nOutputReady: Error! Failed to send output ready message!\n");
			return -1;
		}
	}



	// Poll every pending message
	std::tuple<src_IPC_MESSAGE, zmsg_t*> messageTuple = PollSocketForMessages(m_sockt_OUTPUT);
	while (messageTuple != std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::NO_MESSAGE, NULL)) {

		switch (std::get<0>(messageTuple)) {

		case src_IPC_MESSAGE::INPUT_READY:

			//printf("OutputReady: Received input ready message!\n");
			m_isSendingOutput = true;
			//printf("OutputReady: Finished readying output!\n");

			zmsg_destroy(&std::get<1>(messageTuple));

			return 0;

		case src_IPC_MESSAGE::INPUT_DISCONNECT:
			//printf("OutputReady: Received INPUT_DISCONNECT message! Aborting...\n");

			m_isSendingOutput = false;
			m_OUTPUT_tick_count = 0;
			m_isDoneTransfering = false;

			zmsg_destroy(&std::get<1>(messageTuple));

			return 0;



		     // ALWAYS report NO_MESSAGEs as errors.
		case src_IPC_MESSAGE::NO_MESSAGE:
			//printf("OutputReady: ERROR! Received NO_MESSAGE message! Something must have went wrong.\n");
			break;


		default:
			break;
		}

		messageTuple = PollSocketForMessages(m_sockt_OUTPUT);
	}

	//printf("OutputReady: Received no messages.\n");
	return -1;

}




int InSourceOutSource::InputReady(bool isInFuncLoop)
{
	if (!isInFuncLoop) {
		if (m_INPUT_funcloop == nullptr)
			m_INPUT_funcloop = std::bind(&InSourceOutSource::InputReady, this, std::placeholders::_1);
	}



	// Poll every pending message
	std::tuple<src_IPC_MESSAGE, zmsg_t*> messageTuple = PollSocketForMessages(m_sockt_INPUT);
	while (messageTuple != std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::NO_MESSAGE, NULL)) {

		switch (std::get<0>(messageTuple)) {

	// if received message is OUTPUT_READY, finish readying input and set the sender as our new peer
		case src_IPC_MESSAGE::OUTPUT_READY: {

			zframe_t* first_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(first_frame);

			//printf("InputReady: Received output ready message!\n");
			m_peer_routingID = zframe_data(first_frame);

			zframe_destroy(&first_frame);
			zmsg_destroy(&std::get<1>(messageTuple));


			zmsg_t* inputReadyMessage = srcIPCMessage::InputReady_t::New(zframe_new(m_peer_routingID, 5));
			assert(inputReadyMessage);

			int rc = zmsg_send(&inputReadyMessage, m_sockt_INPUT);
			assert(rc != -1);



			m_isReceivingInput = true;
			//printf("InputReady: Finished readying input!\n");
			return 0;
		}

		case src_IPC_MESSAGE::OUTPUT_DISCONNECT: {
			//printf("InputReady: Received OUTPUT_DISCONNECT message! Aborting...\n");

			m_isReceivingInput = false;
			m_peerIsDoneTransfering = false;
			m_INPUT_tick_count = 0;

			zmsg_destroy(&std::get<1>(messageTuple));

			return 0;
		}


			 // ALWAYS report NO_MESSAGEs as errors.
		case src_IPC_MESSAGE::NO_MESSAGE: {
			//printf("InputReady: ERROR! Received NO_MESSAGE message! Something must have went wrong.\n");
			break;
		}


		default: {
			break;
		}
		}

		messageTuple = PollSocketForMessages(m_sockt_OUTPUT);
	}

	//printf("InputReady: Received no messages.\n");


	return -1;
}




int InSourceOutSource::TransferData(bool isInFuncLoop)
{
	
	if (m_sending_data_buffer == nullptr) {
		// we should probably end transfering instead of just waiting...
		return -1;
	}

	if (!isInFuncLoop) {
		m_OUTPUT_funcloop = std::bind(&InSourceOutSource::TransferData, this, std::placeholders::_1);
	}






	// Check if we are done transfering, if so end data transfer.
	if (m_isDoneTransfering != false) {
		if (m_isSendingOutput != false) {

			zmsg_t* transferingDoneMessage = srcIPCMessage::TransferingDone_t::New(m_OUTPUT_tick_count);

			assert(zmsg_send(&transferingDoneMessage, m_sockt_OUTPUT) != -1);
			//Msg("TransferData: Sent TRANSFERING_DONE message.\n");

			m_isSendingOutput = false;
		}
	}
	else if (!m_isSendingOutput) {
		if (OutputReady(isInFuncLoop) != 0)
			return -1;

		m_OUTPUT_tick_count = 0;
		m_lastTickReceivedByPeer = 0;

		zmsg_t* transferingDataMessage = srcIPCMessage::TransferingData_t::New(m_OUTPUT_tick_count, zframe_new(m_sending_metadata, strlen(m_sending_metadata)));


		assert(zmsg_send(&transferingDataMessage, m_sockt_OUTPUT) != -1);

		//printf("TransferData: Sent TransferData message!\n");
		return -1;

	}




	
	




	// Poll every pending message
	std::tuple<src_IPC_MESSAGE, zmsg_t*> messageTuple = PollSocketForMessages(m_sockt_OUTPUT);
	while (messageTuple != std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::NO_MESSAGE, NULL)) {

		switch (std::get<0>(messageTuple)) {

		case src_IPC_MESSAGE::INPUT_DISCONNECT: {
			//printf("TransferData: Received INPUT_DISCONNECT message! Aborting...\n");

			m_isSendingOutput = false;
			m_OUTPUT_tick_count = 0;
			m_isDoneTransfering = false;

			zmsg_destroy(&std::get<1>(messageTuple));

			return 0;
		}

		case src_IPC_MESSAGE::RECEIVED_DATA: {
			//Msg("TransferData: Received RECEIVED_DATA message.\n");

			zframe_t* first_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(first_frame);
			zframe_t* second_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(second_frame);


			m_lastTickReceivedByPeer = *reinterpret_cast<int*>(zframe_data(second_frame));


			if (m_isDoneTransfering != false) {

				if (m_lastTickReceivedByPeer != m_OUTPUT_tick_count)
					return -1;

				//Msg("TransferData: Received last RECEIVED_DATA message from peer! Ending data transfer.\n");
				m_isDoneTransfering = false;
				m_OUTPUT_tick_count = 0;

				zmsg_destroy(&std::get<1>(messageTuple));
				zframe_destroy(&first_frame);
				zframe_destroy(&second_frame);

				return 0;
			}

			zmsg_destroy(&std::get<1>(messageTuple));
			zframe_destroy(&first_frame);
			zframe_destroy(&second_frame);

			break;
		}



			 // ALWAYS report NO_MESSAGEs as errors.
		case src_IPC_MESSAGE::NO_MESSAGE: {
			//printf("TransferData: ERROR! Received NO_MESSAGE message! Something must have went wrong.\n");
			break;
		}




		default: {
			zmsg_destroy(&std::get<1>(messageTuple));
			break;
		}
		}

		messageTuple = PollSocketForMessages(m_sockt_OUTPUT);
	}






	if (m_isDoneTransfering)
		return -1;



	m_OUTPUT_tick_count++;

	// Sending data messages
	zmsg_t* dataMessage = srcIPCMessage::DataMessage_t::New(m_OUTPUT_tick_count, m_sending_data_buffer);

	if (zmsg_send(&dataMessage, m_sockt_OUTPUT) != 0) {
		//printf("\nTransferData: Error! Failed to send data message!\n");
		m_OUTPUT_tick_count--;
		return -1;
	}
	//printf("TransferData: Data message sent!\n");


	

	//  if tolerance is -1 then ignore drop-out checks.
	if (m_drop_out_tolerance == -1)
		return -1;


	// if peer hasn't sent any RECEIVED_DATA confirmations within the dropout tolerance, assume peer broke connection.
	if (std::abs(m_lastTickReceivedByPeer - m_OUTPUT_tick_count) >= m_drop_out_tolerance) {

		//printf("\nTransferData: Error! Peer hasn't confirmed that it has received any recent ticks! Peer assumed to have dropped connection! Aborting...\n");

		m_isSendingOutput = false;
		m_OUTPUT_tick_count = 0;
		m_isDoneTransfering = false;

		return 0;
	}

	//printf("TransferData ran.\n");
	return -1;
}




int InSourceOutSource::ReceiveData(bool isInFuncLoop)
{


	if (!isInFuncLoop) {
		m_INPUT_funcloop = std::bind(&InSourceOutSource::ReceiveData, this, std::placeholders::_1);
	}

	if (!m_isReceivingInput) {
		if (InputReady(isInFuncLoop) != 0)
			return -1;
		m_INPUT_tick_count = 0;

	}



	// if we are in the process of ending this data transfer...
	if (m_peerIsDoneTransfering == true) {
		if (m_INPUT_tick_count == m_lastTickReceivedFromPeer) {

			//printf("ReceiveData: All messages received from peer, ending data transfer!\n");
			zmsg_t* lastReceivedDataMessage = srcIPCMessage::ReceivedData_t::New(m_INPUT_tick_count, zframe_new(m_peer_routingID, 5));

			assert(zmsg_send(&lastReceivedDataMessage, m_sockt_INPUT) != -1);

			m_isReceivingInput = false;
			m_peerIsDoneTransfering = false;
			m_INPUT_tick_count = 0;

			return 0;
		}

		//printf("ReceiveData: Haven't received all pending data messages! Can't end data transfer yet!\n");
	}



	// Sending DATA_RECEIVED messages
	if (m_INPUT_tick_count != 0 && m_INPUT_tick_count % (m_drop_out_tolerance / 2) == 0) {

		zmsg_t* receivedDataMessage = srcIPCMessage::ReceivedData_t::New(m_INPUT_tick_count, zframe_new(m_peer_routingID, 5));

		assert(zmsg_send(&receivedDataMessage, m_sockt_INPUT) != -1);
		//printf("ReceiveData: Sent received data message!\n");
	}



	// Poll every pending message
	std::tuple<src_IPC_MESSAGE, zmsg_t*> messageTuple = PollSocketForMessages(m_sockt_INPUT);
	while (messageTuple != std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::NO_MESSAGE, NULL)) {

		switch (std::get<0>(messageTuple)) {

		case src_IPC_MESSAGE::OUTPUT_DISCONNECT: {
			//printf("ReceiveData: Received OUTPUT_DISCONNECT message! Aborting...\n");

			m_isReceivingInput = false;
			m_peerIsDoneTransfering = false;
			m_INPUT_tick_count = 0;

			zmsg_destroy(&std::get<1>(messageTuple));

			return 0;
		}

		case src_IPC_MESSAGE::TRANSFERING_DATA: {
			//printf("ReceiveData: Received metadata message!\n");


			zframe_t* first_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(first_frame);
			zframe_t* second_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(second_frame);
			zframe_t* third_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(third_frame);
			zframe_t* fourth_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(fourth_frame);


			zmsg_destroy(&std::get<1>(messageTuple));
			zframe_destroy(&first_frame);
			zframe_destroy(&second_frame);
			zframe_destroy(&third_frame);

			zframe_destroy(&m_received_metadata_buffer);

			m_received_metadata_buffer = fourth_frame;

			break;
		}

		case src_IPC_MESSAGE::DATA_MESSAGE: {
			//printf("ReceiveData: Received data message!\n");


			zframe_t* first_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(first_frame);
			zframe_t* second_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(second_frame);
			zframe_t* third_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(third_frame);
			zframe_t* fourth_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(fourth_frame);


			zmsg_destroy(&std::get<1>(messageTuple));
			zframe_destroy(&first_frame);
			zframe_destroy(&second_frame);
			zframe_destroy(&third_frame);

			zframe_destroy(&m_received_data_buffer);

			m_received_data_buffer = fourth_frame;


			m_INPUT_tick_count++;

			break;
		}

		case src_IPC_MESSAGE::TRANSFERING_DONE: {
			//printf("ReceiveData: Received TRANSFERING_DONE message! Starting to end data transfer.\n");

			m_peerIsDoneTransfering = true;

			zframe_t* first_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(first_frame);
			zframe_t* second_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(second_frame);
			zframe_t* third_frame = zmsg_pop(std::get<1>(messageTuple));
			assert(third_frame);

			m_lastTickReceivedFromPeer = *reinterpret_cast<int*>(zframe_data(third_frame));


			zmsg_destroy(&std::get<1>(messageTuple));
			zframe_destroy(&first_frame);
			zframe_destroy(&second_frame);
			zframe_destroy(&third_frame);

			return -1;
		}


		     // ALWAYS report NO_MESSAGEs as errors.
		case src_IPC_MESSAGE::NO_MESSAGE: {
			//printf("ReceiveData: ERROR! Received NO_MESSAGE message! Something must have went wrong.\n");
			break;
		}
		


		default: {
			return -1;
		}
		}

		messageTuple = PollSocketForMessages(m_sockt_OUTPUT);
	}

	return -1;
}




std::tuple<src_IPC_MESSAGE, zmsg_t*> InSourceOutSource::PollSocketForMessages(zsock_t* socket)
{
	// if provided socket is null, return null
	if (socket == nullptr && !zsock_is(socket))
		return std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::NO_MESSAGE, NULL);



	// polling the provided socket for messages
	void* polledSocket = zpoller_wait(m_poller, m_pollerTimeout);
	if (polledSocket && polledSocket == socket) {


		zmsg_t* receivedMessage = zmsg_recv(socket);
		assert(receivedMessage);


		// duplicate the message so we can dissect it without damaging the original message
		zmsg_t* duplicateMessage = zmsg_dup(receivedMessage);
		assert(duplicateMessage);

		zframe_t* first_frame = zmsg_pop(duplicateMessage);
		assert(first_frame);


		zmsg_destroy(&duplicateMessage);





		// check for all message types

		src_IPC_MESSAGE first_frame_MessageType = *reinterpret_cast<src_IPC_MESSAGE*>(zframe_data(first_frame));
		zframe_destroy(&first_frame);



		switch (first_frame_MessageType) {

			// first frame can only contain INPUT messages
		case src_IPC_MESSAGE::INPUT_DISCONNECT: {
			return std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::INPUT_DISCONNECT, receivedMessage);
		}

		case src_IPC_MESSAGE::INPUT_READY: {
			return std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::INPUT_READY, receivedMessage);
		}

		case src_IPC_MESSAGE::RECEIVED_DATA: {
			return std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::RECEIVED_DATA, receivedMessage);
		}


		// if the message was of no known message type, or if message isnt an INPUT message
		default: {
			break;
		}
		}



		// now check second frame for OUTPUT messages
		
		zframe_t* second_frame = zmsg_pop(duplicateMessage);
		assert(second_frame);

		src_IPC_MESSAGE second_frame_MessageType = *reinterpret_cast<src_IPC_MESSAGE*>(zframe_data(second_frame));
		zframe_destroy(&second_frame);




		switch (second_frame_MessageType) {

			// second frame can only contain OUTPUT messages
		case src_IPC_MESSAGE::OUTPUT_DISCONNECT: {
			return std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::OUTPUT_DISCONNECT, receivedMessage);
		}

		case src_IPC_MESSAGE::OUTPUT_READY: {
			return std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::OUTPUT_READY, receivedMessage);
		}

		case src_IPC_MESSAGE::TRANSFERING_DATA: {
			return std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::TRANSFERING_DATA, receivedMessage);
		}

		case src_IPC_MESSAGE::DATA_MESSAGE: {
			return std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::DATA_MESSAGE, receivedMessage);
		}

		case src_IPC_MESSAGE::TRANSFERING_DONE: {
			return std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::TRANSFERING_DONE, receivedMessage);
		}


		// if the message was of no known message type
		default: {
			return std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::NO_MESSAGE, NULL);;
		}
		}


	}

	return std::tuple<src_IPC_MESSAGE, zmsg_t*>(src_IPC_MESSAGE::NO_MESSAGE, NULL);
}