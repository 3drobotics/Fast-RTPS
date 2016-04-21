/*************************************************************************
 * Copyright (c) 2014 eProsima. All rights reserved.
 *
 * This copy of eProsima Fast RTPS is licensed to you under the terms described in the
 * FASTRTPS_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

/**
 * @file RTPSParticipantListener.h
 *
 */

#ifndef RTPSPARTICIPANTLISTENER_H_
#define RTPSPARTICIPANTLISTENER_H_

#include "RTPSParticipantDiscoveryInfo.h"

namespace eprosima{
namespace fastrtps{
namespace rtps{

class RTPSParticipant;
class ReaderProxyData;
class WriterProxyData;
struct CDRMessage_t;

/**
* Class RTPSParticipantListener with virtual method that the user can overload to respond to certain events.
* @ingroup RTPS_MODULE
*/
class RTPS_DllAPI RTPSParticipantListener
{
public:
	RTPSParticipantListener(){};
	virtual ~RTPSParticipantListener(){};
	
	/**
	* This method is invoked when a new participant is discovered
	* @param part Discovered participant
	* @param info Discovery information of the participant
	*/
	virtual void onRTPSParticipantDiscovery(RTPSParticipant* /*part*/, RTPSParticipantDiscoveryInfo /*info*/){};

	virtual void onRTPSReaderDiscovery(RTPSParticipant* /*part*/, ReaderProxyData*, CDRMessage_t*){}

	virtual void onRTPSWriterDiscovery(RTPSParticipant* /*part*/, WriterProxyData*, CDRMessage_t*){}
};
}
}
}



#endif /* RTPSPARTICIPANTLISTENER_H_ */
