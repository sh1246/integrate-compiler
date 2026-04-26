#pragma once
#include "ir/EncoderBase.h"
#include "dsl/AST.h"

class FabricEncoder : public EncoderBase {
public:
    explicit FabricEncoder(llvm::Module& mod)
        : EncoderBase(mod) {}

    void encode(const FabricDefNode& fabric);

private:
    // TODO:
    // void encodeInstances(...);
    // void encodeNets(...);
    // void encodeCDC(...);
    // void encodeBusMatrix(...);
};