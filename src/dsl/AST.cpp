//
#include "dsl/AST.h"

//DUmp for debugging
void IPDefNode::dump() const {
  std::printf("\n===IPDefNode===\n");
  std::printf("compatible : %s\n", compatible.c_str());
  std::printf("\nClocks (%zu):\n", clocks.size());
  for (auto& itr : clocks)
    std::printf("%s,Freq=%llu,Domain=%s\n",itr.name.c_str(), itr.frequency, itr.domain.c_str());
  std::printf("\nregmap (%zu):\n",regmaps.size());
  for (auto& r : regmaps) {
    std::printf("\t[0x%04X]:%s,access=%s,reset=0x%08X\n", r.offset, r.label.c_str(), r.access.c_str(), r.resetValue);
    for (auto& f : r.fields)
      std::printf("\t\t[%2d:%2d]:%s,\"%s\"\n",f.bitHi, f.bitLo, f.name.c_str(), f.desc.c_str());
  }
}