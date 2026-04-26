#include "ir/IPDefEncoder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

void IPDefEncoder::encode(const IPDefNode& ip) {
  encodeIPDef(ip);
  encodeClocks(ip.clocks);
  encodeRegmaps(ip.regmaps);
  encodePorts(ip.ports);
  encodeAccessorStubs(ip.regmaps);
}

//IP name encoding
void IPDefEncoder::encodeIPDef(const IPDefNode& ip) {
  auto* NMD = M.getOrInsertNamedMetadata("ip.def");
  llvm::Metadata* fields[] = {
    mdStr(ip.compatible),
  };
  NMD->addOperand(llvm::MDNode::get(ctx,fields));
}

//Clock encoding
void IPDefEncoder::encodeClocks(const std::vector<ClockPortNode>& clocks) {
  if(clocks.empty()) return;
  auto* NMD = M.getOrInsertNamedMetadata("ip.clocks");
  for(auto& clk : clocks) {
    llvm::Metadata* fields[] = {
      mdStr(clk.name),
      asI64((int64_t)clk.frequency),
      mdStr(clk.domain),
    };
    NMD->addOperand(llvm::MDNode::get(ctx, fields));
  }
}

//Register Map encoding
void IPDefEncoder::encodeRegmaps( const std::vector<RegmapNode>& regmaps) {
  if(regmaps.empty()) return;
  auto* NMD = M.getOrInsertNamedMetadata("ip.regmaps");
  for (auto& reg:regmaps) {
    //Encoding field list firat
    llvm::SmallVector<llvm::Metadata*> fieldMDs;
    for (auto&f:reg.fields) {
      llvm::Metadata* _fields[] = {
        mdStr(f.name),
        asI32(f.bitLo),
        asI32(f.bitHi),
        mdStr(f.desc),
      };
      fieldMDs.push_back(llvm::MDNode::get(ctx, _fields));
    }
    auto* fieldList = llvm::MDNode::get(ctx, fieldMDs);

    //Regmap node
    llvm::Metadata* fields[] = {
      mdStr(reg.label),
      asI32((int32_t)reg.offset),
      asI32((int32_t)reg.size),
      mdStr(reg.access),
      asI32((int32_t)reg.resetValue),
      fieldList,
    };
    NMD->addOperand(llvm::MDNode::get(ctx,fields));
  }
}

void IPDefEncoder::encodePorts (const std::vector<PortNode>& ports) {
  if(ports.empty()) return ;
  auto* NMD = M.getOrInsertNamedMetadata("ip.ports");
  for(auto& port:ports) {
    llvm::Metadata* fields[] = {
      mdStr(port.name),
      mdStr(port.portType),
      mdStr(port.direction),
      asI32(port.dataWidth),
      asI32(port.addrWidth),
    };
    NMD->addOperand(llvm::MDNode::get(ctx,fields));
  }
}

//SW Interface
void IPDefEncoder::encodeAccessorStubs(const std::vector<RegmapNode>& regmaps) {
  auto* i32Ty = llvm::Type::getInt32Ty(ctx);
  auto* voidTy = llvm::Type::getVoidTy(ctx);
  auto* ptrTy = llvm::PointerType::get(ctx,0);
  for(auto& reg:regmaps) {
    //read_LABEL(ptr base) -> i32
    //RO,RW
    auto* readTy = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
    auto* readFn = llvm::Function::Create(readTy, llvm::Function::ExternalLinkage, "read_" + reg.label, M);
    readFn->setMetadata("reg.offset", llvm::MDNode::get(ctx,{asI32((int32_t)reg.offset)}));
    readFn->setMetadata("reg.access",llvm::MDNode::get(ctx,{mdStr(reg.access)}));
    //write_LABEL(prt base, i32 val) -> void
    //skip RO
    if(reg.access == "RO") continue;
    auto* writeTy = llvm::FunctionType::get(voidTy, {ptrTy, i32Ty}, false);
    auto* writeFn = llvm::Function::Create(writeTy,llvm::Function::ExternalLinkage,"write_"+reg.label,M);
    writeFn->setMetadata("reg.offset", llvm::MDNode::get(ctx,{asI32((int32_t)reg.offset)}));
    writeFn->setMetadata("reg.access",llvm::MDNode::get(ctx,{mdStr(reg.access)}));
  }
}