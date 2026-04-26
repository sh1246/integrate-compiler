#pragma once
#include "ir/EncoderBase.h"
#include "dsl/AST.h"

class IPDefEncoder : public EncoderBase {
public:
    explicit IPDefEncoder(llvm::Module& mod)
        : EncoderBase(mod) {}

    void encode(const IPDefNode& ip);

private:
    void encodeIPDef(const IPDefNode& ip);
    void encodeClocks(const std::vector<ClockPortNode>& clocks);
    void encodeRegmaps(const std::vector<RegmapNode>& regmaps);
    void encodePorts(const std::vector<PortNode>& ports);
    void encodeAccessorStubs(const std::vector<RegmapNode>& regmaps);
};