/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This generated file is licensed to you under the terms described in the
 * _LICENSE file included in this  distribution.
 *
 *************************************************************************
 * 
 * @file FilteringExample.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifndef _FilteringExample_H_
#define _FilteringExample_H_

// TODO Poner en el contexto.

#include <stdint.h>
#include <array>
#include <string>
#include <vector>

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif
#else
#define eProsima_user_DllExport
#endif

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(FilteringExample_SOURCE)
#define FilteringExample_DllAPI __declspec( dllexport )
#else
#define FilteringExample_DllAPI __declspec( dllimport )
#endif // FilteringExample_SOURCE
#else
#define FilteringExample_DllAPI
#endif
#else
#define FilteringExample_DllAPI
#endif // _WIN32

namespace eprosima
{
    namespace fastcdr
    {
        class Cdr;
    }
}

/*!
 * @brief This class represents the structure FilteringExample defined by the user in the IDL file.
 * @ingroup FILTERINGEXAMPLE
 */
class FilteringExample
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport FilteringExample();
    
    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~FilteringExample();
    
    /*!
     * @brief Copy constructor.
     * @param x Reference to the object FilteringExample that will be copied.
     */
    eProsima_user_DllExport FilteringExample(const FilteringExample &x);
    
    /*!
     * @brief Move constructor.
     * @param x Reference to the object FilteringExample that will be copied.
     */
    eProsima_user_DllExport FilteringExample(FilteringExample &&x);
    
    /*!
     * @brief Copy assignment.
     * @param x Reference to the object FilteringExample that will be copied.
     */
    eProsima_user_DllExport FilteringExample& operator=(const FilteringExample &x);
    
    /*!
     * @brief Move assignment.
     * @param x Reference to the object FilteringExample that will be copied.
     */
    eProsima_user_DllExport FilteringExample& operator=(FilteringExample &&x);
    
    /*!
     * @brief This function sets a value in member sampleNumber
     * @param _sampleNumber New value for member sampleNumber
     */
    inline eProsima_user_DllExport void sampleNumber(int32_t _sampleNumber)
    {
        m_sampleNumber = _sampleNumber;
    }

    /*!
     * @brief This function returns the value of member sampleNumber
     * @return Value of member sampleNumber
     */
    inline eProsima_user_DllExport int32_t sampleNumber() const
    {
        return m_sampleNumber;
    }

    /*!
     * @brief This function returns a reference to member sampleNumber
     * @return Reference to member sampleNumber
     */
    inline eProsima_user_DllExport int32_t& sampleNumber()
    {
        return m_sampleNumber;
    }
    
    /*!
     * @brief This function returns the maximum serialized size of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getMaxCdrSerializedSize(size_t current_alignment = 0);


    /*!
     * @brief This function serializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serialize(eprosima::fastcdr::Cdr &cdr) const;

    /*!
     * @brief This function deserializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void deserialize(eprosima::fastcdr::Cdr &cdr);



    /*!
     * @brief This function returns the maximum serialized size of the Key of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(size_t current_alignment = 0);

    /*!
     * @brief This function tells you if the Key has been defined for this type
     */
    eProsima_user_DllExport static bool isKeyDefined();

    /*!
     * @brief This function serializes the key members of an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serializeKey(eprosima::fastcdr::Cdr &cdr) const;
    
private:
    int32_t m_sampleNumber;
};

#endif // _FilteringExample_H_