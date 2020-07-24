/*
  Copyright (c) 2019-present, Trail of Bits, Inc.
  All rights reserved.

  This source code is licensed in accordance with the terms specified in
  the LICENSE file found in the root directory of this source tree.
*/

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

#include <ebpfpub/ibufferstorage.h>

#include <tob/ebpf/perfeventarray.h>

namespace tob::ebpfpub {
class IFunctionTracer {
public:
  struct Parameter final {
    enum class Type { Integer, IntegerPtr, Buffer, String, Argv };
    enum class Mode { In, InOut, Out };

    using SizeVariant = std::variant<std::size_t, std::string>;
    using OptionalSizeVariant = std::optional<SizeVariant>;

    std::string name;
    Type type{Type::Integer};
    Mode mode{Mode::In};
    OptionalSizeVariant opt_size_var;
  };

  struct Event final {
    struct Header final {
      std::uint64_t timestamp{0U};
      pid_t thread_id{0};
      pid_t process_id{0};
      uid_t user_id{0};
      gid_t group_id{0};
      std::uint64_t cgroup_id{0U};
      std::uint64_t exit_code{0U};
      bool probe_error{false};
    };

    struct Field final {
      using Buffer = std::vector<std::uint8_t>;
      using Argv = std::vector<std::string>;

      std::string name;
      bool in{true};
      std::variant<std::uint64_t, Buffer, std::string, Argv> data_var;
    };

    using FieldMap = std::unordered_map<std::string, Field>;

    std::uint64_t identifier{0U};
    std::string name;

    Header header;

    FieldMap in_field_map;
    FieldMap out_field_map;
  };

  using EventList = std::vector<Event>;
  using ParameterList = std::vector<Parameter>;
  using Ref = std::unique_ptr<IFunctionTracer>;

  static StringErrorOr<Ref> createFromSyscallTracepoint(
      const std::string &name, IBufferStorage &buffer_storage,
      ebpf::PerfEventArray &perf_event_array, std::size_t event_map_size);

  static StringErrorOr<Ref> createFromSyscallTracepoint(
      const std::string &name, const ParameterList &parameter_list,
      IBufferStorage &buffer_storage, ebpf::PerfEventArray &perf_event_array,
      std::size_t event_map_size);

  static StringErrorOr<IFunctionTracer::Ref>
  createFromKprobe(const std::string &name, const ParameterList &parameter_list,
                   IBufferStorage &buffer_storage,
                   ebpf::PerfEventArray &perf_event_array,
                   std::size_t event_map_size);

  static StringErrorOr<IFunctionTracer::Ref> createFromUprobe(
      const std::string &name, const std::string &path,
      const ParameterList &parameter_list, IBufferStorage &buffer_storage,
      ebpf::PerfEventArray &perf_event_array, std::size_t event_map_size);

  IFunctionTracer() = default;
  virtual ~IFunctionTracer() = default;

  virtual const std::string &name() const = 0;
  virtual std::uint64_t eventIdentifier() const = 0;

  virtual std::string ir() const = 0;

  IFunctionTracer(const IFunctionTracer &) = delete;
  IFunctionTracer &operator=(const IFunctionTracer &) = delete;
};
} // namespace tob::ebpfpub
