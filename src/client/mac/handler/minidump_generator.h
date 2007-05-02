// Copyright (c) 2006, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// minidump_generator.h:  Create a minidump of the current MacOS process.

#ifndef CLIENT_MAC_GENERATOR_MINIDUMP_GENERATOR_H__
#define CLIENT_MAC_GENERATOR_MINIDUMP_GENERATOR_H__

#include <mach/mach.h>

#include <string>

#include "client/minidump_file_writer.h"
#include "google_breakpad/common/minidump_format.h"

#include "dynamic_images.h"

namespace google_breakpad {

using std::string;

// Creates a minidump file of the current process.  If there is exception data,
// use SetExceptionInformation() to add this to the minidump.  The minidump
// file is generated by the Write() function.
// Usage:
// MinidumpGenerator minidump();
// minidump.Write("/tmp/minidump");
//
class MinidumpGenerator {
 public:
  MinidumpGenerator();
  MinidumpGenerator(mach_port_t crashing_task, mach_port_t handler_thread);

  ~MinidumpGenerator();

  // Return <dir>/<unique_name>.dmp
  // Sets |unique_name| (if requested) to the unique name for the minidump
  static string UniqueNameInDirectory(const string &dir, string *unique_name);

  // Write out the minidump into |path|
  // All of the components of |path| must exist and be writable
  // Return true if successful, false otherwise
  bool Write(const char *path);

  // Specify some exception information, if applicable
  void SetExceptionInformation(int type, int code, mach_port_t thread_name) {
    exception_type_ = type;
    exception_code_ = code;
    exception_thread_ = thread_name;
  }

  // Gather system information.  This should be call at least once before using
  // the MinidumpGenerator class.
  static void GatherSystemInformation();

 private:
    typedef bool (MinidumpGenerator::*WriteStreamFN)(MDRawDirectory *);

  // Stream writers
  bool WriteThreadListStream(MDRawDirectory *thread_list_stream);
  bool WriteExceptionStream(MDRawDirectory *exception_stream);
  bool WriteSystemInfoStream(MDRawDirectory *system_info_stream);
  bool WriteModuleListStream(MDRawDirectory *module_list_stream);
  bool WriteMiscInfoStream(MDRawDirectory *misc_info_stream);
  bool WriteBreakpadInfoStream(MDRawDirectory *breakpad_info_stream);

  // Helpers
  u_int64_t CurrentPCForStack(thread_state_data_t state);
  bool WriteStackFromStartAddress(vm_address_t start_addr,
                                  MDMemoryDescriptor *stack_location);
  bool WriteStack(thread_state_data_t state,
                  MDMemoryDescriptor *stack_location);
  bool WriteContext(thread_state_data_t state,
                    MDLocationDescriptor *register_location);
  bool WriteThreadStream(mach_port_t thread_id, MDRawThread *thread);
  bool WriteCVRecord(MDRawModule *module, int cpu_type, 
                     const char *module_path);
  bool WriteModuleStream(unsigned int index, MDRawModule *module);
  size_t CalculateStackSize(vm_address_t start_addr);
  int  FindExecutableModule();

  // disallow copy ctor and operator=
  explicit MinidumpGenerator(const MinidumpGenerator &);
  void operator=(const MinidumpGenerator &);

  // Use this writer to put the data to disk
  MinidumpFileWriter writer_;

  // Exception information
  int exception_type_;
  int exception_code_;
  mach_port_t exception_thread_;
  mach_port_t crashing_task_;
  mach_port_t handler_thread_;
  
  // System information
  static char build_string_[16];
  static int os_major_version_;
  static int os_minor_version_;
  static int os_build_number_;
  
  // Information about dynamically loaded code
  DynamicImages *dynamic_images_;
};

}  // namespace google_breakpad

#endif  // CLIENT_MAC_GENERATOR_MINIDUMP_GENERATOR_H__
