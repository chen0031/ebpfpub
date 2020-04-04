/*
  Copyright (c) 2019-present, Trail of Bits, Inc.
  All rights reserved.

  This source code is licensed in accordance with the terms specified in
  the LICENSE file found in the root directory of this source tree.
*/

#pragma once

#include <unordered_set>

#include <ebpfpub/ibpfprogramwriter.h>
#include <ebpfpub/ibufferreader.h>
#include <ebpfpub/ibufferstorage.h>

namespace tob::ebpfpub {
class IFunctionSerializer {
public:
  struct Event final {
    struct Header final {
      std::uint64_t timestamp{0U};

      pid_t parent_process_id{0U}; // todo
      pid_t thread_id{0};
      pid_t process_id{0};

      uid_t user_id{0};
      gid_t group_id{0};

      std::uint64_t exit_code{0U};
      bool probe_error{false};
    };

    struct Integer final {
      enum class Type { Int8, Int16, Int32, Int64 };

      Type type;
      bool is_signed{false};
      std::uint64_t value{0U};
    };

    using Variant =
        std::variant<std::string, std::vector<std::uint8_t>, Integer>;

    using OptionalVariant = std::optional<Variant>;
    using FieldMap = std::unordered_map<std::string, OptionalVariant>;

    std::string name;
    Header header;
    FieldMap field_map;
  };

  enum class Stage { Enter, Exit };

  using StageList = std::unordered_set<Stage>;
  using EventList = std::vector<Event>;

  using Ref = std::unique_ptr<IFunctionSerializer>;
  using Factory = StringErrorOr<Ref> (*)(IBufferStorage &buffer_storage);

  IFunctionSerializer() = default;
  virtual ~IFunctionSerializer() = default;

  virtual const std::string &getName() const = 0;
  virtual const StageList &stages() const = 0;

  virtual SuccessOrStringError generate(Stage stage,
                                        const ebpf::Structure &enter_structure,
                                        IBPFProgramWriter &bpf_prog_writer) = 0;

  virtual SuccessOrStringError parseEvents(Event &event,
                                           IBufferReader &buffer_reader,
                                           IBufferStorage &buffer_storage) = 0;

  IFunctionSerializer(const IFunctionSerializer &) = delete;
  IFunctionSerializer &operator=(const IFunctionSerializer &) = delete;
};
} // namespace tob::ebpfpub