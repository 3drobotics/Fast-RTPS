/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This generated file is licensed to you under the terms described in the
 * fastcdr_LICENSE file included in this fastcdr distribution.
 *
 *************************************************************************
 * 
 * @file FilteringExampleSubscriber.cpp
 * This file contains the implementation of the subscriber functions.
 *
 * This file was generated by the tool fastcdrgen.
 */

#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/attributes/SubscriberAttributes.h>

#include <fastrtps/Domain.h>

#include "FilteringExampleSubscriber.h"


FilteringExampleSubscriber::FilteringExampleSubscriber() : mp_participant(nullptr), mp_subscriber(nullptr) {}

FilteringExampleSubscriber::~FilteringExampleSubscriber() {	Domain::removeParticipant(mp_participant);}

bool FilteringExampleSubscriber::init(int type)
{
	// Create RTPSParticipant
	
	ParticipantAttributes PParam;
	PParam.rtps.builtin.domainId = 0; //MUST BE THE SAME AS IN THE PUBLISHER
	PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
	PParam.rtps.setName("Participant_subscriber"); //You can put the name you want
	mp_participant = Domain::createParticipant(PParam);
	if(mp_participant == nullptr)
			return false;
	
	//Register the type
	
	Domain::registerType(mp_participant,(TopicDataType*) &myType);		
			
	// Create Subscriber

	SubscriberAttributes Rparam;
	Rparam.topic.topicKind = NO_KEY;
	Rparam.topic.topicDataType = myType.getName(); //Must be registered before the creation of the subscriber
	Rparam.topic.topicName = "FilteringExamplePubSubTopic";
	if (type == 1)
	{
		Rparam.qos.m_partition.push_back("Fast_Partition");
	}
	else //2 = slow
	{
		Rparam.qos.m_partition.push_back("Slow_Partition");
	}

	mp_subscriber = Domain::createSubscriber(mp_participant, Rparam, (SubscriberListener*)&m_listener);
	if (mp_subscriber == nullptr)
		return false;
	return true;
}

void FilteringExampleSubscriber::SubListener::onSubscriptionMatched(Subscriber* sub,MatchingInfo& info)
{
	if (info.status == MATCHED_MATCHING)
	{
		n_matched++;
		cout << "Subscriber matched" << endl;
	}
	else
	{
		n_matched--;
		cout << "Subscriber unmatched" << endl;
	}
}

void FilteringExampleSubscriber::SubListener::onNewDataMessage(Subscriber* sub)
{
		// Take data
		FilteringExample st;
		
		if(sub->takeNextData(&st, &m_info))
		{
			if(m_info.sampleKind == ALIVE)
			{
				// Print your structure data here.
				++n_msg;
				cout << "Sample received, count=" << n_msg << endl;
				cout << " sampleNumber=" << st.sampleNumber() << endl;
			}
		}
}

void FilteringExampleSubscriber::run()
{
	cout << "Waiting for Data, press Enter to stop the Subscriber. "<<endl;
	std::cin.ignore();
	cout << "Shutting down the Subscriber." << endl;
}

