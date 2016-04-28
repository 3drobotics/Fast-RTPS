/*************************************************************************
 * Copyright (c) 2014 eProsima. All rights reserved.
 *
 * This copy of eProsima Fast RTPS is licensed to you under the terms described in the
 * FASTRTPS_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

/**
 * @file RTPSMessageGroup.cpp
 *
 */

#include <fastrtps/rtps/messages/RTPSMessageGroup.h>
#include <fastrtps/rtps/messages/RTPSMessageCreator.h>
#include <fastrtps/rtps/writer/RTPSWriter.h>
#include "../participant/RTPSParticipantImpl.h"

#include <fastrtps/utils/RTPSLog.h>

namespace eprosima {
namespace fastrtps{
namespace rtps {

static const char* const CLASS_NAME = "RTPSMessageGroup";


bool sort_changes_group (CacheChange_t* c1,CacheChange_t* c2)
{
	return(c1->sequenceNumber < c2->sequenceNumber);
}

bool sort_SeqNum(const SequenceNumber_t& s1,const SequenceNumber_t& s2)
{
	return(s1 < s2);
}

typedef std::pair<SequenceNumber_t,SequenceNumberSet_t> pair_T;

void RTPSMessageGroup::prepare_SequenceNumberSet(std::vector<SequenceNumber_t>* changesSeqNum,
		std::vector<std::pair<SequenceNumber_t,SequenceNumberSet_t>>* sequences)
{
	//First compute the number of GAP messages we need:

	std::sort(changesSeqNum->begin(),changesSeqNum->end(),sort_SeqNum);
	bool new_pair = true;
	bool seqnumset_init = false;
	uint32_t count = 0;
	for(std::vector<SequenceNumber_t>::iterator it = changesSeqNum->begin();
			it!=changesSeqNum->end();++it)
	{
		if(new_pair)
		{
			SequenceNumberSet_t seqset;
			seqset.base = (*it) + 1; // IN CASE IN THIS SEQNUMSET there is only 1 number.
			pair_T pair(*it,seqset);
			sequences->push_back(pair);
			new_pair = false;
			seqnumset_init = true;
			count = 1;
			continue;
		}
		if((*it - sequences->back().first).low == count) //CONTINUOUS FROM THE START
		{
			++count;
			sequences->back().second.base = (*it)+1;
			continue;
		}
		else
		{
			if(seqnumset_init) //FIRST TIME SINCE it was continuous
			{
				sequences->back().second.base = *(it-1);
				seqnumset_init = false;
			}
            // Try to add, If it fails the diference between *it and base is greater than 255.
			if(sequences->back().second.add((*it)))
				continue;
			else
			{
                // Process again the sequence number in a new pair in next loop.
				--it;
				new_pair = true;
			}
		}
	}
}







bool RTPSMessageGroup::send_Changes_AsGap(RTPSMessageGroup_t* msg_group,
		RTPSWriter* W, std::vector<SequenceNumber_t>* changesSeqNum,
        const GuidPrefix_t& remoteGuidPrefix, const EntityId_t& readerId,
		LocatorList_t* unicast, LocatorList_t* multicast)
{
	const char* const METHOD_NAME = "send_Changes_AsGap";
	//cout << "CHANGES SEQ NUM: "<<changesSeqNum->size()<<endl;
	std::vector<std::pair<SequenceNumber_t,SequenceNumberSet_t>> Sequences;
	RTPSMessageGroup::prepare_SequenceNumberSet(changesSeqNum,&Sequences);
	std::vector<std::pair<SequenceNumber_t,SequenceNumberSet_t>>::iterator seqit = Sequences.begin();

	CDRMessage_t* cdrmsg_submessage = &msg_group->m_rtpsmsg_submessage;
	CDRMessage_t* cdrmsg_header = &msg_group->m_rtpsmsg_header;
	CDRMessage_t* cdrmsg_fullmsg = &msg_group->m_rtpsmsg_fullmsg;

	uint16_t gap_msg_size = 0;
	uint16_t gap_n = 1;
	//FIRST SUBMESSAGE
	CDRMessage::initCDRMsg(cdrmsg_submessage);
	RTPSMessageCreator::addSubmessageGap(cdrmsg_submessage,seqit->first,seqit->second,
			readerId,W->getGuid().entityId);

	gap_msg_size = (uint16_t)cdrmsg_submessage->length;
	if(gap_msg_size+(uint32_t)RTPSMESSAGE_HEADER_SIZE > msg_group->m_rtpsmsg_fullmsg.max_size)
	{
		logError(RTPS_WRITER,"The Gap messages are larger than max size, something is wrong");
		return false;
	}
	bool first = true;
	do
	{
		CDRMessage::initCDRMsg(cdrmsg_fullmsg);
		CDRMessage::appendMsg(cdrmsg_fullmsg,cdrmsg_header);

        // If there is a destinatary, send the submessage INFO_DST.
        if(remoteGuidPrefix != c_GuidPrefix_Unknown)
        {
            RTPSMessageCreator::addSubmessageInfoDST(cdrmsg_fullmsg, remoteGuidPrefix);
        }

		if(first)
		{
			CDRMessage::appendMsg(cdrmsg_fullmsg,cdrmsg_submessage);
			first = false;
		}
		while(cdrmsg_fullmsg->length + gap_msg_size < cdrmsg_fullmsg->max_size
				&& (gap_n + 1) <=(uint16_t)Sequences.size()) //another one fits in the full message
		{
			++gap_n;
			++seqit;
			CDRMessage::initCDRMsg(cdrmsg_submessage);
			RTPSMessageCreator::addSubmessageGap(cdrmsg_submessage,seqit->first,seqit->second,
					readerId,W->getGuid().entityId);
			CDRMessage::appendMsg(cdrmsg_fullmsg,cdrmsg_fullmsg);
		}
		std::vector<Locator_t>::iterator lit;
		for(lit = unicast->begin();lit!=unicast->end();++lit)
			W->getRTPSParticipant()->sendSync(cdrmsg_fullmsg,(*lit));
		for(lit = multicast->begin();lit!=multicast->end();++lit)
			W->getRTPSParticipant()->sendSync(cdrmsg_fullmsg,(*lit));

	}while(gap_n < Sequences.size()); //There is still a message to add
	return true;
}

void RTPSMessageGroup::prepareDataSubM(RTPSWriter* W, CDRMessage_t* submsg, bool expectsInlineQos, const CacheChange_t* change, const EntityId_t& ReaderId)
{
	const char* const METHOD_NAME = "prepareDataSubM";
	ParameterList_t* inlineQos = NULL;
	if(expectsInlineQos)
	{
		//TODOG INLINEQOS
//		if(W->getInlineQos()->m_parameters.size()>0)
//			inlineQos = W->getInlineQos();
	}
	CDRMessage::initCDRMsg(submsg);
	bool added= RTPSMessageCreator::addSubmessageData(submsg, change, W->getAttributes()->topicKind, ReaderId, expectsInlineQos, inlineQos);
	if(!added)
		logError(RTPS_WRITER,"Problem adding DATA submsg to the CDRMessage, buffer too small";);
}


// helper: create empty RTPS message (RTPS header + optional INFO_DST)
static void msg_group_init(RTPSMessageGroup_t *msg_group, const GuidPrefix_t& remoteGuidPrefix)
{
    // fullmsg = empty
    CDRMessage::initCDRMsg(&msg_group->m_rtpsmsg_fullmsg);
    // fullmsg += RTPS header
    CDRMessage::appendMsg(&msg_group->m_rtpsmsg_fullmsg, &msg_group->m_rtpsmsg_header);
    // fullmsg += INFO_DST if needed
    if (remoteGuidPrefix != c_GuidPrefix_Unknown)
        RTPSMessageCreator::addSubmessageInfoDST(&msg_group->m_rtpsmsg_fullmsg, remoteGuidPrefix);
}

// create an RTPS message consisting of several data (and other) submessages
//
// basic flow is:
//
// clear rtps message
// for each cache change
//     create a data submessage from cache change
//     determine whether an info_ts submessage is required (and how big it is)
//     if rtps message is not empty and (data submessage + info_ts) don't fit
//         send rtps message
//         clear rtps message
//     if data submessage and info_ts fit in rtps message
//         append them
//     else
//         data submessage does not fit in an empty rtps message; skip it
// end for
// if rtps message is not empty
//     send rtps message
//
bool RTPSMessageGroup::send_Changes_AsData(RTPSMessageGroup_t* msg_group,
        RTPSWriter* W, std::vector<const CacheChange_t*>* changes,
        const GuidPrefix_t& remoteGuidPrefix, const EntityId_t& ReaderId,
        LocatorList_t& unicast, LocatorList_t& multicast,
        bool expectsInlineQos)
{
    const char* const METHOD_NAME = "send_Changes_AsData";
    logInfo(RTPS_WRITER,"Sending relevant changes as data messages");
    CDRMessage_t* cdrmsg_submessage = &msg_group->m_rtpsmsg_submessage;
    CDRMessage_t* cdrmsg_fullmsg = &msg_group->m_rtpsmsg_fullmsg;

    // true when fullmsg has only the rtps header and (possibly) the INFO_DST submessage;
    // false when any other submessages have been added
    bool fullmsg_empty;

    // true when an INFO_TS with timestamp has been inserted;
    // false before any INFO_TS are inserted, or when one is inserted with the invalid flag set
    bool fullmsg_info_ts;

    msg_group_init(msg_group, remoteGuidPrefix);
    fullmsg_empty = true;
    fullmsg_info_ts = false;

    // how many bytes the INFO_TS for the current submessage (if any) will take
    unsigned info_ts_size;

    // std::vector<const CacheChange_t*>::iterator cit;
    for (auto cit = changes->begin(); cit != changes->end(); cit++)
    {

        // create submessage containing next cache change
        RTPSMessageGroup::prepareDataSubM(W, cdrmsg_submessage, expectsInlineQos, *cit, ReaderId);
        unsigned data_msg_size = cdrmsg_submessage->length;
        Time_t msg_timestamp = (*cit)->sourceTimestamp;
        bool msg_has_timestamp = (msg_timestamp != c_TimeInvalid);

        // Do we need INFO_TS, and if so, how many bytes will it take? We need it if this
        // message has a valid timestamp (assume it is different from any previous message),
        // or if this message does not have a valid timestamp and there is a timestamp set
        // in the message.
        if (msg_has_timestamp)
        {
            // need INFO_TS with submessage's timestamp
            info_ts_size = 12; // XXX
        }
        else
        {
            // submessage does not have a valid timestamp
            if (fullmsg_info_ts)
            {
                // message has a previous INFO_TS; need new INFO_TS with invalid flag set
                info_ts_size = 4; // XXX
            }
            else // !fullmsg_info_ts
            {
                // message does not have a previous INFO_TS, so none is needed
                info_ts_size = 0;
            }
        }

        unsigned fullmsg_room = cdrmsg_fullmsg->max_size - cdrmsg_fullmsg->length;
        bool submsg_fits = ((data_msg_size + info_ts_size) <= fullmsg_room);

        // Send fullmsg first if necessary to make room. If fullmsg is empty and
        // submessage does not fit, error is detected below.
        if (!fullmsg_empty && !submsg_fits)
        {
            // send it!
            // std::vector<Locator_t>::iterator lit
            for (auto lit = unicast.begin(); lit != unicast.end(); ++lit)
                W->getRTPSParticipant()->sendSync(cdrmsg_fullmsg, *lit);
            for (auto lit = multicast.begin(); lit != multicast.end(); ++lit)
                W->getRTPSParticipant()->sendSync(cdrmsg_fullmsg, *lit);
            // clear fullmsg
            msg_group_init(msg_group, remoteGuidPrefix);
            fullmsg_empty = true;
            fullmsg_info_ts = false;
            fullmsg_room = cdrmsg_fullmsg->max_size - cdrmsg_fullmsg->length;
            submsg_fits = ((data_msg_size + info_ts_size) <= fullmsg_room);
        }

        if (submsg_fits)
        {
            // append info_ts
            if (info_ts_size > 0)
            {
                RTPSMessageCreator::addSubmessageInfoTS(cdrmsg_fullmsg, msg_timestamp, false);
                // We either inserted an INFO_TS with a timestamp, or a flag
                // invalidating the timestamp. In either case we need to
                // remember that for the next submessage (if any) so we can
                // know whether to insert or invalidate the timestamp for that
                // submessage.
                if (info_ts_size > 8)
                    fullmsg_info_ts = true;
                else
                    fullmsg_info_ts = false;
            }
            // append cache change
            CDRMessage::appendMsg(cdrmsg_fullmsg, cdrmsg_submessage);
            fullmsg_empty = false;
        }
        else
        {
            // skipping data; does not fit in an empty rtps message
            logError(RTPS_WRITER, "Cache change too big; not sending")
        }

    } // for (auto cit = changes->begin(); cit != changes->end(); cit++)

    // finished processing cache changes

    if (!fullmsg_empty)
    {
        // send it!
        for (auto lit = unicast.begin(); lit != unicast.end(); ++lit)
            W->getRTPSParticipant()->sendSync(cdrmsg_fullmsg, *lit);
        for (auto lit = multicast.begin(); lit != multicast.end(); ++lit)
            W->getRTPSParticipant()->sendSync(cdrmsg_fullmsg, *lit);
    }

    return true;

} // bool RTPSMessageGroup::send_Changes_AsData()

bool RTPSMessageGroup::send_Changes_AsData(RTPSMessageGroup_t* msg_group,
        RTPSWriter* W, std::vector<const CacheChange_t*>* changes,
        const GuidPrefix_t& remoteGuidPrefix, const EntityId_t& ReaderId,
        const Locator_t& loc, bool expectsInlineQos)
{
    LocatorList_t locs1;
    LocatorList_t locs2;
    locs1.push_back(loc);
    return send_Changes_AsData(msg_group, W, changes, remoteGuidPrefix, ReaderId, locs1, locs2, expectsInlineQos);
}

}
} /* namespace rtps */
} /* namespace eprosima */
