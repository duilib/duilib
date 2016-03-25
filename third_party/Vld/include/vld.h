////////////////////////////////////////////////////////////////////////////////
//
//  Visual Leak Detector - Import Library Header
//  Copyright (c) 2005-2014 VLD Team
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//  See COPYING.txt for the full terms of the GNU Lesser General Public License.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vld_def.h"

typedef int            VLD_BOOL;
typedef unsigned int   VLD_UINT;
typedef size_t         VLD_SIZET;
typedef void*          VLD_HMODULE;

#if defined _DEBUG || defined VLD_FORCE_ENABLE

#pragma comment(lib, "vld.lib")

// Force a symbolic reference to the global VisualLeakDetector class object from
// the DLL. This ensures that the DLL is loaded and linked with the program,
// even if no code otherwise imports any of the DLL's exports.
#pragma comment(linker, "/include:__imp_?g_vld@@3VVisualLeakDetector@@A")

////////////////////////////////////////////////////////////////////////////////
//
//  Visual Leak Detector APIs
//

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// VLDDisable - Disables Visual Leak Detector's memory leak detection at
//   runtime. If memory leak detection is already disabled, then calling this
//   function has no effect.
//
//  Note: In multithreaded programs, this function operates on a per-thread
//    basis. In other words, if you call this function from one thread, then
//    memory leak detection is only disabled for that thread. If memory leak
//    detection is enabled for other threads, then it will remain enabled for
//    those other threads. It was designed to work this way to insulate you,
//    the programmer, from having to ensure thread synchronization when calling
//    VLDEnable() and VLDDisable(). Without this, calling these two functions
//    unsynchronized could result in unpredictable and unintended behavior.
//    But this also means that if you want to disable memory leak detection
//    process-wide, then you need to call this function from every thread in
//    the process.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDDisable ();

// VLDEnable - Enables Visual Leak Detector's memory leak detection at runtime.
//   If memory leak detection is already enabled, which it is by default, then
//   calling this function has no effect.
//
//  Note: In multithreaded programs, this function operates on a per-thread
//    basis. In other words, if you call this function from one thread, then
//    memory leak detection is only enabled for that thread. If memory leak
//    detection is disabled for other threads, then it will remain disabled for
//    those other threads. It was designed to work this way to insulate you,
//    the programmer, from having to ensure thread synchronization when calling
//    VLDEnable() and VLDDisable(). Without this, calling these two functions
//    unsynchronized could result in unpredictable and unintended behavior.
//    But this also means that if you want to enable memory leak detection
//    process-wide, then you need to call this function from every thread in
//    the process.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDEnable ();

// VLDRestore - Restore Visual Leak Detector's previous state.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDRestore ();

// VLDGlobalDisable - Disables Visual Leak Detector's memory leak detection at
//   runtime in all threads. If memory leak detection is already disabled, 
//   then calling this function has no effect.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDGlobalDisable ();

// VLDGlobalEnable - Enables Visual Leak Detector's memory leak detection 
//   at runtime in all threads. If memory leak detection is already enabled, 
//   which it is by default, then calling this function has no effect.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDGlobalEnable ();

// VLDReportLeaks - Report leaks up to the execution point.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) VLD_UINT VLDReportLeaks ();

// VLDReportThreadLeaks - Report thread leaks up to the execution point.
//
// threadId: thread Id.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) VLD_UINT VLDReportThreadLeaks (VLD_UINT threadId);

// VLDGetLeaksCount - Return memory leaks count to the execution point.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) VLD_UINT VLDGetLeaksCount ();

// VLDGetThreadLeaksCount - Return thread memory leaks count to the execution point.
//
// threadId: thread Id.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) VLD_UINT VLDGetThreadLeaksCount (VLD_UINT threadId);

// VLDMarkAllLeaksAsReported - Mark all leaks as reported.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDMarkAllLeaksAsReported ();

// VLDMarkThreadLeaksAsReported - Mark thread leaks as reported.
//
// threadId: thread Id.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDMarkThreadLeaksAsReported (VLD_UINT threadId);


// VLDRefreshModules - Look for recently loaded DLLs and patch them if necessary.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDRefreshModules();


// VLDEnableModule - Enable Memory leak checking on the specified module.
//
// module: module handle.
//
//  Return Value:
//
//    None.
//

__declspec(dllimport) void VLDEnableModule(VLD_HMODULE module);


// VLDDisableModule - Disable Memory leak checking on the specified module.
//
// module: module handle.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDDisableModule(VLD_HMODULE module);

// VLDGetOptions - Return all current options.
//
//  Return Value:
//
//    Mask of current options.
//
__declspec(dllimport) VLD_UINT VLDGetOptions();

// VLDGetReportFilename - Return current report filename.
//
// filename: current report filename (max characters - MAX_PATH).
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDGetReportFilename(wchar_t *filename);

// VLDSetOptions - Update the report options via function call rather than INI file.
//
// option_mask: Only the following flags are checked
// VLD_OPT_AGGREGATE_DUPLICATES
// VLD_OPT_MODULE_LIST_INCLUDE
// VLD_OPT_SAFE_STACK_WALK
// VLD_OPT_SLOW_DEBUGGER_DUMP
// VLD_OPT_TRACE_INTERNAL_FRAMES
// VLD_OPT_START_DISABLED
// VLD_OPT_SKIP_HEAPFREE_LEAKS
// VLD_OPT_VALIDATE_HEAPFREE
//
// maxDataDump: maximum number of user-data bytes to dump for each leaked block.
//
// maxTraceFrames: maximum number of frames per stack trace for each leaked block.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDSetOptions(VLD_UINT option_mask, VLD_SIZET maxDataDump, VLD_UINT maxTraceFrames);

// VLDSetModulesList - Set list of modules included/excluded in leak detection
// depending on parameter "includeModules".
//
// modules: list of modules to be forcefully included/excluded in leak detection.
//
// includeModules: include or exclude that modules.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDSetModulesList(const wchar_t *modules, VLD_BOOL includeModules);

// VLDGetModulesList - Return current list of included/excluded modules
// depending on flag VLD_OPT_TRACE_INTERNAL_FRAMES.
//
// modules: destination string for list of included/excluded modules (maximum length 512 characters).
//
// size: maximum string size.
//
//  Return Value:
//
//    VLD_BOOL: TRUE if include modules, otherwise FALSE.
//
__declspec(dllimport) VLD_BOOL VLDGetModulesList(wchar_t *modules, VLD_UINT size);

// VLDSetReportOptions - Update the report options via function call rather than INI file.
//
// Only the following flags are checked
// VLD_OPT_REPORT_TO_DEBUGGER
// VLD_OPT_REPORT_TO_FILE
// VLD_OPT_REPORT_TO_STDOUT
// VLD_OPT_UNICODE_REPORT
//
// filename is optional and can be NULL.
//
//  Return Value:
//
//    None.
//
__declspec(dllimport) void VLDSetReportOptions(VLD_UINT option_mask, const wchar_t *filename);

// VLDSetReportHook - Installs or uninstalls a client-defined reporting function by hooking it 
//  into the C run-time debug reporting process (debug version only).
//
// mode: The action to take: VLD_RPTHOOK_INSTALL or VLD_RPTHOOK_REMOVE.
//
// pfnNewHook: Report hook to install or remove.
//
//  Return Value:
//
//    int: 0 if success.
//
__declspec(dllimport) int VLDSetReportHook(int mode,  VLD_REPORT_HOOK pfnNewHook);

// VLDResolveCallstacks - Performs symbol resolution for all saved extent CallStack's that have
// been tracked by Visual Leak Detector. This function is necessary for applications that 
// dynamically load and unload modules, and through which memory leaks might be included.
// If this is NOT called, stack traces may have stack frames with no symbol information. This 
// happens because the symbol API's cannot look up symbols for a binary / module that has been unloaded
// from the process.
//
//  Return Value:
//
//    None.
//
__declspec(dllexport) void VLDResolveCallstacks();

#ifdef __cplusplus
}
#endif // __cplusplus

#else // !_DEBUG

#define VLDEnable()
#define VLDDisable()
#define VLDRestore()
#define VLDReportLeaks() (0)
#define VLDReportThreadLeaks() (0)
#define VLDGetLeaksCount() (0)
#define VLDGetThreadLeaksCount() (0)
#define VLDMarkAllLeaksAsReported()
#define VLDMarkThreadLeaksAsReported(a)
#define VLDRefreshModules()
#define VLDEnableModule(a)
#define VLDDisableModule(b)
#define VLDGetOptions() (0)
#define VLDGetReportFilename(a)
#define VLDSetOptions(a, b, c)
#define VLDSetReportHook(a, b)
#define VLDSetModulesList(a)
#define VLDGetModulesList(a, b) (FALSE)
#define VLDSetReportOptions(a, b)

#endif // _DEBUG
