/*
  Copyright (c) 2019-present, Trail of Bits, Inc.
  All rights reserved.

  This source code is licensed in accordance with the terms specified in
  the LICENSE file found in the root directory of this source tree.
*/

#pragma once

#include <vector>

#include <ebpfpub/ibufferstorage.h>

#include <linux/bpf.h>

#include <tob/ebpf/bpfmap.h>

namespace tob::ebpfpub {
class BufferStorage final : public IBufferStorage {
public:
  virtual ~BufferStorage();

  virtual std::size_t memoryUsage() const override;
  virtual std::size_t bufferSize() const override;
  virtual std::size_t bufferCount() const override;

  virtual int bufferMap() const override;
  virtual int indexMap() const override;

  virtual ebpf::BPFMapErrorCode getBuffer(std::vector<std::uint8_t> &value,
                                          std::uint64_t index) override;

  BufferStorage(const BufferStorage &) = delete;
  BufferStorage &operator=(const BufferStorage &) = delete;

protected:
  BufferStorage(std::size_t buffer_size, std::size_t buffer_count);

private:
  struct PrivateData;
  std::unique_ptr<PrivateData> d;

  friend class IBufferStorage;
};
} // namespace tob::ebpfpub
