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

#include <wchar.h>

#define VLD_OPT_AGGREGATE_DUPLICATES    0x0001 //   If set, aggregate duplicate leaks in the leak report.
#define VLD_OPT_MODULE_LIST_INCLUDE     0x0002 //   If set, modules in the module list are included, all others are excluded.
#define VLD_OPT_REPORT_TO_DEBUGGER      0x0004 //   If set, the memory leak report is sent to the debugger.
#define VLD_OPT_REPORT_TO_FILE          0x0008 //   If set, the memory leak report is sent to a file.
#define VLD_OPT_SAFE_STACK_WALK         0x0010 //   If set, the stack is walked using the "safe" method (StackWalk64).
#define VLD_OPT_SELF_TEST               0x0020 //   If set, perform a self-test to verify memory leak self-checking.
#define VLD_OPT_SLOW_DEBUGGER_DUMP      0x0040 //   If set, inserts a slight delay between sending output to the debugger.
#define VLD_OPT_START_DISABLED          0x0080 //   If set, memory leak detection will initially disabled.
#define VLD_OPT_TRACE_INTERNAL_FRAMES   0x0100 //   If set, include useless frames (e.g. internal to VLD) in call stacks.
#define VLD_OPT_UNICODE_REPORT          0x0200 //   If set, the leak report will be encoded UTF-16 instead of ASCII.
#define VLD_OPT_VLDOFF                  0x0400 //   If set, VLD will be completely deactivated. It will not attach to any modules.
#define VLD_OPT_REPORT_TO_STDOUT        0x0800 //   If set, the memory leak report is sent to stdout.
#define VLD_OPT_SKIP_HEAPFREE_LEAKS     0x1000 //   If set, VLD skip HeapFree memory leaks.
#define VLD_OPT_VALIDATE_HEAPFREE       0x2000 //   If set, VLD verifies and reports heap consistency for HeapFree calls.

#define VLD_RPTHOOK_INSTALL  0
#define VLD_RPTHOOK_REMOVE   1

typedef int (__cdecl * VLD_REPORT_HOOK)(int reportType, wchar_t *message, int *returnValue);
