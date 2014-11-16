// -*- C++ -*-
// Module:  Log4CPLUS
// File:    property.h
// Created: 2/2002
// Author:  Tad E. Smith
//
//
// Copyright 2002-2013 Tad E. Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** @file */

#ifndef LOG4CPLUS_HELPERS_PROPERTY_HEADER_
#define LOG4CPLUS_HELPERS_PROPERTY_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/streams.h>
#include <log4cplus/tstring.h>
#include <map>
#include <vector>


namespace log4cplus {
    namespace helpers {

        //! \sa log4cplus::PropertyConfigurator
        class LOG4CPLUS_EXPORT Properties {
        public:
            enum PFlags
            {
                // These encoding related options occupy 2 bits of the flags
                // and are mutually exclusive. These flags are synchronized
                // with PCFlags in PropertyConfigurator.

                fEncodingShift      = 3
                , fEncodingMask     = 0x3
                , fUnspecEncoding   = (0 << fEncodingShift)
#if defined (LOG4CPLUS_HAVE_CODECVT_UTF8_FACET) && defined (UNICODE)
                , fUTF8             = (1 << fEncodingShift)
#endif
#if (defined (LOG4CPLUS_HAVE_CODECVT_UTF16_FACET) || defined (_WIN32)) \
    && defined (UNICODE)
                , fUTF16            = (2 << fEncodingShift)
#endif
#if defined (LOG4CPLUS_HAVE_CODECVT_UTF32_FACET) && defined (UNICODE)
                , fUTF32            = (3 << fEncodingShift)
#endif
            };

            Properties();
            explicit Properties(log4cplus::tistream& input);
            explicit Properties(const log4cplus::tstring& inputFile, unsigned flags = 0);
            virtual ~Properties();

          // constants
            static const tchar PROPERTIES_COMMENT_CHAR;

          // methods
            /**
             * Tests to see if <code>key</code> can be found in this map.
             */
            bool exists(const log4cplus::tstring& key) const;
            bool exists(tchar const * key) const;

            /**
             * Returns the number of entries in this map.
             */
            std::size_t size() const
            {
                return data.size();
            }

            /**
             * Searches for the property with the specified key in this property
             * list. If the key is not found in this property list, the default
             * property list, and its defaults, recursively, are then checked. 
             * The method returns <code>null</code> if the property is not found.
             */
            log4cplus::tstring const & getProperty(const log4cplus::tstring& key) const;
            log4cplus::tstring const & getProperty(tchar const * key) const;

            /**
             * Searches for the property with the specified key in this property
             * list. If the key is not found in this property list, the default
             * property list, and its defaults, recursively, are then checked. 
             * The method returns the default value argument if the property is 
             * not found.
             */
            log4cplus::tstring getProperty(const log4cplus::tstring& key,
                                           const log4cplus::tstring& defaultVal) const;

            /**
             * Returns all the keys in this property list.
             */
            std::vector<log4cplus::tstring> propertyNames() const;

            /**
             * Inserts <code>value</code> into this map indexed by <code>key</code>.
             */
            void setProperty(const log4cplus::tstring& key, const log4cplus::tstring& value);

            /**
             * Removed the property index by <code>key</code> from this map.
             */
            bool removeProperty(const log4cplus::tstring& key);

            /**
             * Returns a subset of the "properties" whose keys start with
             * "prefix".  The returned "properties" have "prefix" trimmed from
             * their keys.
             */
            Properties getPropertySubset(const log4cplus::tstring& prefix) const;

            bool getInt (int & val, log4cplus::tstring const & key) const;
            bool getUInt (unsigned & val, log4cplus::tstring const & key) const;
            bool getLong (long & val, log4cplus::tstring const & key) const;
            bool getULong (unsigned long & val, log4cplus::tstring const & key) const;
            bool getBool (bool & val, log4cplus::tstring const & key) const;

        protected:
          // Types
            typedef std::map<log4cplus::tstring, log4cplus::tstring> StringMap;

          // Methods
            void init(log4cplus::tistream& input);

          // Data
            StringMap data;
            unsigned flags;

        private:
            template <typename StringType>
            log4cplus::tstring const & get_property_worker (
                StringType const & key) const;

            template <typename ValType>
            bool get_type_val_worker (ValType & val,
                log4cplus::tstring const & key) const;
        };
    } // end namespace helpers

}


#endif // LOG4CPLUS_HELPERS_PROPERTY_HEADER_

