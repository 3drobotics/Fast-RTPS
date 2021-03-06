/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This generated file is licensed to you under the terms described in the
 * _LICENSE file included in this  distribution.
 *
 *************************************************************************
 * 
 * @file Data64kb.cpp
 * This source file contains the definition of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace { char dummy; }
#endif

#include "Data64kb.h"

#include <fastcdr/Cdr.h>

#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>

Data64kb::Data64kb()
{
}

Data64kb::~Data64kb()
{
}

Data64kb::Data64kb(const Data64kb &x)
{
    m_data = x.m_data;
}

Data64kb::Data64kb(Data64kb &&x)
{
    m_data = std::move(x.m_data);
}

Data64kb& Data64kb::operator=(const Data64kb &x)
{
    m_data = x.m_data;
    
    return *this;
}

Data64kb& Data64kb::operator=(Data64kb &&x)
{
    m_data = std::move(x.m_data);
    
    return *this;
}

bool Data64kb::operator==(const Data64kb &x) const
{
    if(m_data == x.m_data)
        return true;

    return false;
}

size_t Data64kb::getMaxCdrSerializedSize(size_t current_alignment)
{
    size_t initial_alignment = current_alignment;
            
    current_alignment += 4 + eprosima::fastcdr::Cdr::alignment(current_alignment, 4);
    current_alignment += (63996 * 1) + eprosima::fastcdr::Cdr::alignment(current_alignment, 1);


    return current_alignment - initial_alignment;
}

void Data64kb::serialize(eprosima::fastcdr::Cdr &scdr) const
{
    if(m_data.size() <= 63996)
    scdr << m_data;
    else
        throw eprosima::fastcdr::exception::BadParamException("data field exceeds the maximum length");
}

void Data64kb::deserialize(eprosima::fastcdr::Cdr &dcdr)
{
    dcdr >> m_data;
}

size_t Data64kb::getKeyMaxCdrSerializedSize(size_t current_alignment)
{
	size_t current_align = current_alignment;
            

    return current_align;
}

bool Data64kb::isKeyDefined()
{
    return false;
}

void Data64kb::serializeKey(eprosima::fastcdr::Cdr& /*scdr*/) const
{
	 
}
