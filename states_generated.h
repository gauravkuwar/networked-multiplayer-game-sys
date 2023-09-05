// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_STATES_MYNAMESPACE_H_
#define FLATBUFFERS_GENERATED_STATES_MYNAMESPACE_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 23 &&
              FLATBUFFERS_VERSION_MINOR == 5 &&
              FLATBUFFERS_VERSION_REVISION == 26,
             "Non-compatible flatbuffers version included");

namespace MyNamespace {

struct States;
struct StatesBuilder;

struct States FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef StatesBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_PID = 4,
    VT_X = 6,
    VT_Y = 8,
    VT_VX = 10,
    VT_VY = 12
  };
  int32_t pid() const {
    return GetField<int32_t>(VT_PID, 0);
  }
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  float vx() const {
    return GetField<float>(VT_VX, 0.0f);
  }
  float vy() const {
    return GetField<float>(VT_VY, 0.0f);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_PID, 4) &&
           VerifyField<float>(verifier, VT_X, 4) &&
           VerifyField<float>(verifier, VT_Y, 4) &&
           VerifyField<float>(verifier, VT_VX, 4) &&
           VerifyField<float>(verifier, VT_VY, 4) &&
           verifier.EndTable();
  }
};

struct StatesBuilder {
  typedef States Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_pid(int32_t pid) {
    fbb_.AddElement<int32_t>(States::VT_PID, pid, 0);
  }
  void add_x(float x) {
    fbb_.AddElement<float>(States::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(States::VT_Y, y, 0.0f);
  }
  void add_vx(float vx) {
    fbb_.AddElement<float>(States::VT_VX, vx, 0.0f);
  }
  void add_vy(float vy) {
    fbb_.AddElement<float>(States::VT_VY, vy, 0.0f);
  }
  explicit StatesBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<States> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<States>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<States> CreateStates(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t pid = 0,
    float x = 0.0f,
    float y = 0.0f,
    float vx = 0.0f,
    float vy = 0.0f) {
  StatesBuilder builder_(_fbb);
  builder_.add_vy(vy);
  builder_.add_vx(vx);
  builder_.add_y(y);
  builder_.add_x(x);
  builder_.add_pid(pid);
  return builder_.Finish();
}

inline const MyNamespace::States *GetStates(const void *buf) {
  return ::flatbuffers::GetRoot<MyNamespace::States>(buf);
}

inline const MyNamespace::States *GetSizePrefixedStates(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<MyNamespace::States>(buf);
}

inline bool VerifyStatesBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<MyNamespace::States>(nullptr);
}

inline bool VerifySizePrefixedStatesBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<MyNamespace::States>(nullptr);
}

inline void FinishStatesBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<MyNamespace::States> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedStatesBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<MyNamespace::States> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace MyNamespace

#endif  // FLATBUFFERS_GENERATED_STATES_MYNAMESPACE_H_