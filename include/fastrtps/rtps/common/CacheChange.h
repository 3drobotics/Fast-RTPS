/*************************************************************************
 * Copyright (c) 2014 eProsima. All rights reserved.
 *
 * This copy of eProsima Fast RTPS is licensed to you under the terms described in the
 * FASTRTPS_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

/**
 * @file CacheChange.h
 */

#ifndef CACHECHANGE_H_
#define CACHECHANGE_H_

#include "Types.h"
#include "WriteParams.h"
#include "SerializedPayload.h"
#include "Time_t.h"
#include "InstanceHandle.h"

#include <vector>

namespace eprosima
{
    namespace fastrtps
    {
        namespace rtps
        {
            /**
             * @enum ChangeKind_t, different types of CacheChange_t.
             * @ingroup COMMON_MODULE
             */
#if defined(_WIN32)
            enum RTPS_DllAPI ChangeKind_t{
#else
            enum ChangeKind_t{
#endif
                ALIVE,                //!< ALIVE
                NOT_ALIVE_DISPOSED,   //!< NOT_ALIVE_DISPOSED
                NOT_ALIVE_UNREGISTERED,//!< NOT_ALIVE_UNREGISTERED
                NOT_ALIVE_DISPOSED_UNREGISTERED //!<NOT_ALIVE_DISPOSED_UNREGISTERED
            };

            enum ChangeFragmentStatus_t
            {
                NOT_PRESENT = 0,
                PRESENT = 1
            };

            /**
             * Structure CacheChange_t, contains information on a specific CacheChange.
             * @ingroup COMMON_MODULE
             */
            struct RTPS_DllAPI CacheChange_t
            {
                //!Kind of change, default value ALIVE.
                ChangeKind_t kind;
                //!GUID_t of the writer that generated this change.
                GUID_t writerGUID;
                //!Handle of the data associated wiht this change.
                InstanceHandle_t instanceHandle;
                //!SequenceNumber of the change
                SequenceNumber_t sequenceNumber;
                //!Serialized Payload associated with the change.
                SerializedPayload_t serializedPayload;
                //!Indicates if the cache has been read (only used in READERS)
                bool isRead;
                //!Source TimeStamp (only used in Readers)
                Time_t sourceTimestamp;

                WriteParams write_params;
                bool is_untyped_;

                /*!
                 * @brief Default constructor.
                 * Creates an empty CacheChange_t.
                 */
                CacheChange_t():
                    kind(ALIVE),
                    isRead(false),
                    is_untyped_(true),
                    dataFragments_(new std::vector<uint32_t>()),
                    fragment_size_(0)
                {
                }

                /**
                 * Constructor with payload size
                 * @param payload_size Serialized payload size
                 */
                // TODO Check pass uint32_t to serializedPayload that needs int16_t.
                CacheChange_t(uint32_t payload_size, bool is_untyped = false):
                    kind(ALIVE),
                    serializedPayload(payload_size),
                    isRead(false),
                    is_untyped_(is_untyped),
                    dataFragments_(new std::vector<uint32_t>()),
                    fragment_size_(0)
                {
                }

                /*!
                 * Copy a different change into this one. All the elements are copied, included the data, allocating new memory.
                 * @param[in] ch_ptr Pointer to the change.
                 * @return True if correct.
                 */
                bool copy(CacheChange_t* ch_ptr)
                {
                    kind = ch_ptr->kind;
                    writerGUID = ch_ptr->writerGUID;
                    instanceHandle = ch_ptr->instanceHandle;
                    sequenceNumber = ch_ptr->sequenceNumber;
                    sourceTimestamp = ch_ptr->sourceTimestamp;
                    write_params = ch_ptr->write_params;

                    bool ret = serializedPayload.copy(&ch_ptr->serializedPayload, (ch_ptr->is_untyped_ ? false : true));

                    setFragmentSize(ch_ptr->fragment_size_);
                    dataFragments_->assign(ch_ptr->dataFragments_->begin(), ch_ptr->dataFragments_->end());

                    return ret;
                }

                void copy_not_memcpy(CacheChange_t* ch_ptr)
                {
                    kind = ch_ptr->kind;
                    writerGUID = ch_ptr->writerGUID;
                    instanceHandle = ch_ptr->instanceHandle;
                    sequenceNumber = ch_ptr->sequenceNumber;
                    sourceTimestamp = ch_ptr->sourceTimestamp;
                    write_params = ch_ptr->write_params;

                    // Copy certain values from serializedPayload
                    serializedPayload.encapsulation = ch_ptr->serializedPayload.encapsulation;
                }

                ~CacheChange_t()
                {
                    if (dataFragments_)
                        delete dataFragments_;
                }

                uint32_t getFragmentCount() const
                { 
                    return (uint32_t)dataFragments_->size();
                }

                std::vector<uint32_t>* getDataFragments() { return dataFragments_; }

                uint16_t getFragmentSize() const { return fragment_size_; }

                void setFragmentSize(uint16_t fragment_size)
                {
                    this->fragment_size_ = fragment_size;

                    if (fragment_size == 0) {
                        dataFragments_->clear();
                    } 
                    else
                    {
                        //TODO Mirar si cuando se compatibilice con RTI funciona el calculo, porque ellos
                        //en el sampleSize incluyen el padding.
                        uint32_t size = (serializedPayload.length + fragment_size - 1) / fragment_size;
                        dataFragments_->assign(size, ChangeFragmentStatus_t::NOT_PRESENT);
                    }
                }


                private:

                // Data fragments
                std::vector<uint32_t>* dataFragments_;

                // Fragment size
                uint16_t fragment_size_;
            };

#ifndef DOXYGEN_SHOULD_SKIP_THIS_PUBLIC

            /**
             * Enum ChangeForReaderStatus_t, possible states for a CacheChange_t in a ReaderProxy.
             *  @ingroup COMMON_MODULE
             */
            enum ChangeForReaderStatus_t{
                UNSENT = 0,        //!< UNSENT
                UNACKNOWLEDGED = 1,//!< UNACKNOWLEDGED
                REQUESTED = 2,     //!< REQUESTED
                ACKNOWLEDGED = 3,  //!< ACKNOWLEDGED
                UNDERWAY = 4       //!< UNDERWAY
            };

            /**
             * Enum ChangeFromWriterStatus_t, possible states for a CacheChange_t in a WriterProxy.
             *  @ingroup COMMON_MODULE
             */
            enum ChangeFromWriterStatus_t{
                UNKNOWN = 0,
                MISSING = 1,
                //REQUESTED_WITH_NACK,
                RECEIVED = 2,
                LOST = 3
            };

            /**
             * Struct ChangeForReader_t used to represent the state of a specific change with respect to a specific reader, as well as its relevance.
             *  @ingroup COMMON_MODULE
             */
            class ChangeForReader_t
            {
                friend struct ChangeForReaderCmp;

                public:

                ChangeForReader_t() : status_(UNSENT), is_relevant_(true),
                change_(nullptr)
                {
                }

                ChangeForReader_t(const ChangeForReader_t& ch) : status_(ch.status_),
                is_relevant_(ch.is_relevant_), seq_num_(ch.seq_num_), change_(ch.change_)
                {
                }

                ChangeForReader_t(const CacheChange_t* change) : status_(UNSENT),
                is_relevant_(true), seq_num_(change->sequenceNumber), change_(change)
                {
                }

                ChangeForReader_t(const SequenceNumber_t& seq_num) : status_(UNSENT),
                is_relevant_(true), seq_num_(seq_num), change_(nullptr)
                {
                }

                ~ChangeForReader_t(){}

                ChangeForReader_t& operator=(const ChangeForReader_t& ch)
                {
                    status_ = ch.status_;
                    is_relevant_ = ch.is_relevant_;
                    seq_num_ = ch.seq_num_;
                    change_ = ch.change_;
                    return *this;
                }

                /**
                 * Get the cache change
                 * @return Cache change
                 */
                const CacheChange_t* getChange() const
                {
                    return change_;
                }

                void setStatus(const ChangeForReaderStatus_t status)
                {
                    status_ = status;
                }

                ChangeForReaderStatus_t getStatus() const
                {
                    return status_;
                }

                void setRelevance(const bool relevance)
                {
                    is_relevant_ = relevance;
                }

                bool isRelevant() const
                {
                    return is_relevant_;
                }

                const SequenceNumber_t getSequenceNumber() const
                {
                    return seq_num_;
                }

                //! Set change as not valid
                void notValid()
                {
                    is_relevant_ = false;
                    change_ = nullptr;
                }

                //! Set change as valid
                bool isValid() const
                {
                    return change_ != nullptr;
                }

                private:

                //!Status
                ChangeForReaderStatus_t status_;

                //!Boolean specifying if this change is relevant
                bool is_relevant_;

                //!Sequence number
                SequenceNumber_t seq_num_;

                const CacheChange_t* change_;
            };

            struct ChangeForReaderCmp
            {
                bool operator()(const ChangeForReader_t& a, const ChangeForReader_t& b) const
                {
                    return a.seq_num_ < b.seq_num_;
                }
            };

            /**
             * Struct ChangeFromWriter_t used to indicate the state of a specific change with respect to a specific writer, as well as its relevance.
             *  @ingroup COMMON_MODULE
             */
            class ChangeFromWriter_t
            {
                friend struct ChangeFromWriterCmp;

                public:

                ChangeFromWriter_t() : status_(UNKNOWN), is_relevant_(true)
                {

                }

                ChangeFromWriter_t(const ChangeFromWriter_t& ch) : status_(ch.status_),
                is_relevant_(ch.is_relevant_), seq_num_(ch.seq_num_)
                {
                }

                ChangeFromWriter_t(const SequenceNumber_t& seq_num) : status_(UNKNOWN),
                is_relevant_(true), seq_num_(seq_num)
                {
                }

                ~ChangeFromWriter_t(){};

                ChangeFromWriter_t& operator=(const ChangeFromWriter_t& ch)
                {
                    status_ = ch.status_;
                    is_relevant_ = ch.is_relevant_;
                    seq_num_ = ch.seq_num_;
                    return *this;
                }

                void setStatus(const ChangeFromWriterStatus_t status)
                {
                    status_ = status;
                }

                ChangeFromWriterStatus_t getStatus() const
                {
                    return status_;
                }

                void setRelevance(const bool relevance)
                {
                    is_relevant_ = relevance;
                }

                bool isRelevant() const
                {
                    return is_relevant_;
                }

                const SequenceNumber_t getSequenceNumber() const
                {
                    return seq_num_;
                }

                //! Set change as not valid
                void notValid()
                {
                    is_relevant_ = false;
                }

                private:

                //! Status
                ChangeFromWriterStatus_t status_;

                //! Boolean specifying if this change is relevant
                bool is_relevant_;

                //! Sequence number
                SequenceNumber_t seq_num_;
            };

            struct ChangeFromWriterCmp
            {
                bool operator()(const ChangeFromWriter_t& a, const ChangeFromWriter_t& b) const
                {
                    return a.seq_num_ < b.seq_num_;
                }
            };
#endif
        }
    }
}
#endif /* CACHECHANGE_H_ */
