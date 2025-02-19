/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CBORG_HEADER_H__
#define __CBORG_HEADER_H__

#include <stdint.h>

#include "cborg/CborBase.h"

// clang-format off

class CborgHeader
{
public:
    CborgHeader() {}

    void decode(const uint8_t* head, uint32_t maxLength)
    {
        // reset variables
        tag = 0xFF;
        majorType = CborBase::TypeUnassigned;
        minorType = CborBase::TypeUnknown;
        length = 0;
        value = 0;

        if (maxLength < 1)
        {
            return;
        }

        // null pointer check
        if (head)
        {
            // read types
            majorType = head[0] >> 5;
            minorType = head[0] & 31;

            // use types to read value with proper length
            if (minorType < 24)
            {
                value = minorType;

                length = 1;
            }
            else if (minorType == 24)
            {
                if (maxLength < 2)
                {
                    return;
                }

                value = head[1];

                length = 2;
            }
            else if (minorType == 25) // 2 bytes
            {
                if (maxLength < 3)
                {
                    return;
                }

                value = ((uint16_t) head[1] << 8) | head[2];

                length = 3;
            }
            else if (minorType == 26) // 4 bytes
            {
                if (maxLength < 5)
                {
                    return;
                }

                value = ((uint32_t) head[1] << 24)
                      | ((uint32_t) head[2] << 16)
                      | ((uint32_t) head[3] << 8)
                      |             head[4];

                length = 5;
            }
            else if (minorType == 27) // 8 bytes
            {
                if (maxLength < 9)
                {
                    return;
                }

                value = ((uint64_t) head[1] << 56)
                      | ((uint64_t) head[2] << 48)
                      | ((uint64_t) head[3] << 40)
                      | ((uint64_t) head[4] << 32)
                      | ((uint64_t) head[5] << 24)
                      | ((uint64_t) head[6] << 16)
                      | ((uint64_t) head[7] << 8)
                      |             head[8];

                length = 9;
            }
            else if (minorType == CborBase::TypeIndefinite)
            {
                value = 31;
                length = 1;
            }

            // the first type was a semantic tag, read the next header
            if (majorType == CborBase::TypeTag)
            {
                if (maxLength < uint32_t(length) + 1)
                {
                    return;
                }

                // store previous value as the tag
                tag = value;

                // read next type
                majorType = head[length] >> 5;
                minorType = head[length] & 31;

                // read value based on length in type
                if (minorType < 24)
                {
                    value = minorType;

                    length += 1;
                }
                else if (minorType == 24)
                {
                    if (maxLength < uint32_t(length) + 2)
                    {
                        return;
                    }

                    value = head[length + 1];

                    length += 2;
                }
                else if (minorType == 25)
                {
                    if (maxLength < uint32_t(length) + 3)
                    {
                        return;
                    }

                    value = ((uint16_t) head[length + 1] << 8) | head[length + 2];

                    length += 3;
                }
                else if (minorType == 26)
                {
                    if (maxLength < uint32_t(length) + 5)
                    {
                        return;
                    }

                    value = ((uint32_t) head[length + 1] << 24)
                          | ((uint32_t) head[length + 2] << 16)
                          | ((uint32_t) head[length + 3] << 8)
                          |             head[length + 4];
                    length += 5;
                }
                else if (minorType == 27)
                {
                    if (maxLength < uint32_t(length) + 9)
                    {
                        return;
                    }

                    value = ((uint64_t) head[length + 1] << 56)
                          | ((uint64_t) head[length + 2] << 48)
                          | ((uint64_t) head[length + 3] << 40)
                          | ((uint64_t) head[length + 4] << 32)
                          | ((uint64_t) head[length + 5] << 24)
                          | ((uint64_t) head[length + 6] << 16)
                          | ((uint64_t) head[length + 7] << 8)
                          |             head[length + 8];
                    length += 9;
                }
                else if (minorType == CborBase::TypeIndefinite)
                {
                    value = 31;
                    length += 1;
                }
            }
        }
    }


    uint32_t getTag() const
    {
        return tag;
    }

    uint8_t getMajorType() const
    {
        return majorType;
    }

    uint8_t getMinorType() const
    {
        return minorType;
    }

    uint32_t getValue() const
    {
        return value;
    }

    uint8_t getLength() const
    {
        return length;
    }

private:
    uint32_t tag;
    uint8_t majorType;
    uint8_t minorType;
    uint8_t length;
    uint32_t value;
};

#endif // __CBOR_HEADER_H__

// clang-format on
