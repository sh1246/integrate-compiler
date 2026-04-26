//Define AST node for ipdef parser
#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct FieldNode {
  std::string name; //Field name
  //[Hi:Lo]
  int bitLo;
  int bitHi;
  std::string desc; //Description
};

//Single Register format
struct RegmapNode {
  std::string label;
  uint32_t offset;
  uint32_t size;
  std::string access; //"RW or RO or WO"
  uint32_t resetValue;
  std::vector<FieldNode> fields;
};

//Ports
struct PortNode {
  std::string name;
  std::string portType;  //AXI-L, AXIS, AXI, wire
  std::string direction; //in, out, slave, master
  int dataWidth = 0;     //No data width
  int addrWidth = 0;     //Used in AMBA protocol
};

//Clock
struct ClockPortNode {
  std::string name;
  uint64_t frequency = 0;
  std::string domain; //Clock-domain name used in fabric
};

//IP-definitino
struct IPDefNode {
  std::string compatible;   //Unique Identifier of IP e.g, mb, uart-v2
  std::vector<RegmapNode> regmaps;
  std::vector<PortNode>   ports;
  std::vector<ClockPortNode> clocks;
  //Debug
  void dump() const;
};
