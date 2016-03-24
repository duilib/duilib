/*
http://code.google.com/p/ticpp/
Copyright (c) 2006 Ryan Pusztai, Ryan Mulder

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

 /*
 * THIS FILE WAS ALTERED BY Matt Janisz, 12. October 2012.
 *
 * - added ticppapi.h include and TICPP_API dll-interface to support building DLL using VS200X
 */

#ifdef TIXML_USE_TICPP

#ifndef TICPPRC_INCLUDED
#define TICPPRC_INCLUDED

#include "ticppapi.h"
#include <vector>

// Forward declare ticpp::Node, so it can be made a friend of TiCppRC
namespace ticpp
{
	class TICPP_API Base;
}

// Forward declare TiCppRCImp so TiCppRC can hold a pointer to it
class TICPP_API TiCppRCImp;

/**
Base class for reference counting functionality
*/
class TICPP_API TiCppRC
{
	// Allow ticpp::Node to directly modify reference count
	friend class ticpp::Base;

private:

	TiCppRCImp* m_tiRC; /**< Pointer to reference counter */

public:

	/**
	Constructor
	Spawns new reference counter with a pointer to this
	*/
	TiCppRC();

	/**
	Destructor
	Nullifies the pointer to this held by the reference counter
	Decrements reference count
	*/
	virtual ~TiCppRC();
	
	std::vector< ticpp::Base* > m_spawnedWrappers; /**< Remember all wrappers that we've created with 'new' - ( e.g. NodeFactory, FirstChildElement, etc. )*/

	/**
	Delete all container objects we've spawned with 'new'.
	*/
	void DeleteSpawnedWrappers();
};

class TICPP_API TiCppRCImp
{
private:

	int m_count;		/**< Holds reference count to me, and to the node I point to */

	TiCppRC* m_tiCppRC;	/**< Holds pointer to an object inheriting TiCppRC */

public:

	/**
	Initializes m_tiCppRC pointer, and set reference count to 1
	*/
	TiCppRCImp( TiCppRC* tiCppRC );

	/**
	Allows the TiCppRC object to set the pointer to itself ( m_tiCppRc ) to NULL when the TiCppRC object is deleted
	*/
	void Nullify();

	/**
	Increment Reference Count
	*/
	void IncRef();

	/**
	Decrement Reference Count
	*/
	void DecRef();

	/**
	Set Reference Count to 1 - dangerous! - Use only if you are sure of the consequences
	*/
	void InitRef();

	/**
	Get internal pointer to the TiCppRC object - not reference counted, use at your own risk
	*/
	TiCppRC* Get();

	/**
	Returns state of internal pointer - will be null if the object was deleted
	*/
	bool IsNull();
};

#endif // TICPP_INCLUDED

#endif // TIXML_USE_TICPP
