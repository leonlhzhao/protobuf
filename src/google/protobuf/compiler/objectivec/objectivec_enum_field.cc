// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// https://developers.google.com/protocol-buffers/
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

#include <map>
#include <string>

#include <google/protobuf/compiler/objectivec/objectivec_enum_field.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/compiler/objectivec/objectivec_helpers.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/stubs/strutil.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace objectivec {

namespace {
void SetEnumVariables(const FieldDescriptor* descriptor,
                      map<string, string>* variables) {
  string type = EnumName(descriptor->enum_type());
  (*variables)["storage_type"] = type;
  // TODO(thomasvl): Make inclusion of descriptor compile time and output
  // both of these. Note: Extensions currently have to have the EnumDescription.
  (*variables)["enum_verifier"] = type + "_IsValidValue";
  (*variables)["enum_desc_func"] = type + "_EnumDescriptor";

  const Descriptor* msg_descriptor = descriptor->containing_type();
  (*variables)["owning_message_class"] = ClassName(msg_descriptor);
}
}  // namespace

EnumFieldGenerator::EnumFieldGenerator(const FieldDescriptor* descriptor)
    : SingleFieldGenerator(descriptor) {
  SetEnumVariables(descriptor, &variables_);
}

EnumFieldGenerator::~EnumFieldGenerator() {}

void EnumFieldGenerator::GenerateFieldDescriptionTypeSpecific(
    io::Printer* printer) const {
  // TODO(thomasvl): Output the CPP check to use descFunc or validator based
  // on final compile.
  printer->Print(
      variables_,
      "  .typeSpecific.enumDescFunc = $enum_desc_func$,\n");
}

void EnumFieldGenerator::GenerateCFunctionDeclarations(
    io::Printer* printer) const {
  if (!HasPreservingUnknownEnumSemantics(descriptor_->file())) return;

  printer->Print(
      variables_,
      "int32_t $owning_message_class$_$capitalized_name$_RawValue($owning_message_class$ *message);\n"
      "void Set$owning_message_class$_$capitalized_name$_RawValue($owning_message_class$ *message, int32_t value);\n"
      "\n");
}

void EnumFieldGenerator::GenerateCFunctionImplementations(
    io::Printer* printer) const {
  if (!HasPreservingUnknownEnumSemantics(descriptor_->file())) return;

  printer->Print(
      variables_,
      "int32_t $owning_message_class$_$capitalized_name$_RawValue($owning_message_class$ *message) {\n"
      "  GPBDescriptor *descriptor = [$owning_message_class$ descriptor];\n"
      "  GPBFieldDescriptor *field = [descriptor fieldWithNumber:$field_number_name$];\n"
      "  return GPBGetInt32IvarWithField(message, field);\n"
      "}\n"
      "\n"
      "void Set$owning_message_class$_$capitalized_name$_RawValue($owning_message_class$ *message, int32_t value) {\n"
      "  GPBDescriptor *descriptor = [$owning_message_class$ descriptor];\n"
      "  GPBFieldDescriptor *field = [descriptor fieldWithNumber:$field_number_name$];\n"
      "  GPBSetInt32IvarWithFieldInternal(message, field, value, descriptor.file.syntax);\n"
      "}\n"
      "\n");
}

RepeatedEnumFieldGenerator::RepeatedEnumFieldGenerator(
    const FieldDescriptor* descriptor)
    : RepeatedFieldGenerator(descriptor) {
  SetEnumVariables(descriptor, &variables_);
  variables_["array_storage_type"] = "GPBEnumArray";
}

RepeatedEnumFieldGenerator::~RepeatedEnumFieldGenerator() {}

void RepeatedEnumFieldGenerator::GenerateFieldDescriptionTypeSpecific(
    io::Printer* printer) const {
  // TODO(thomasvl): Output the CPP check to use descFunc or validator based
  // on final compile.
  printer->Print(
      variables_,
      "  .typeSpecific.enumDescFunc = $enum_desc_func$,\n");
}

}  // namespace objectivec
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
