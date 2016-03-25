// -*- C++ -*-
// Module:  Log4CPLUS
// File:    objectregistry.h
// Created: 3/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2013 Tad E. Smith
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

#ifndef LOG4CPLUS_SPI_OBJECT_REGISTRY_HEADER_
#define LOG4CPLUS_SPI_OBJECT_REGISTRY_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/tstring.h>
#include <log4cplus/thread/syncprims.h>
#include <map>
#include <memory>
#include <vector>


namespace log4cplus {
    namespace spi {

        /**
         * This is the base class used to implement the functionality required
         * by the ObjectRegistry template class.
         */
        class LOG4CPLUS_EXPORT ObjectRegistryBase {
        public:
          // public methods
            /**
             * Tests to see whether or not an object is bound in the
             * registry as <code>name</code>.
             */
            bool exists(const log4cplus::tstring& name) const;

            /**
             * Returns the names of all registered objects.
             */
            std::vector<log4cplus::tstring> getAllNames() const;

        protected:
          // Ctor and Dtor
            ObjectRegistryBase();
            virtual ~ObjectRegistryBase();

          // protected methods
            /**
             * Used to enter an object into the registry.  (The registry now
             * owns <code>object</code>.)
             */
            bool putVal(const log4cplus::tstring& name, void* object);

            /**
             * Used to retrieve an object from the registry.  (The registry
             * owns the returned pointer.)
             */
            void* getVal(const log4cplus::tstring& name) const;

            /**
             * Deletes <code>object</code>.
             */
            virtual void deleteObject(void *object) const = 0;

            /**
             * Deletes all objects from this registry.
             */
            virtual void clear();

          // Types
            typedef std::map<log4cplus::tstring, void*> ObjectMap;

          // Data
            thread::Mutex mutex;
            ObjectMap data;

        private:
            ObjectRegistryBase (ObjectRegistryBase const &);
            ObjectRegistryBase & operator = (ObjectRegistryBase const &);
        };

    }
}


#endif // LOG4CPLUS_SPI_OBJECT_REGISTRY_HEADER_

