#pragma once
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Type.h"

class EncoderBase {
  protected :
    llvm::Module& M;
    llvm::LLVMContext &ctx;
    explicit EncoderBase (llvm::Module& mod) : M(mod), ctx(mod.getContext()) {}
    llvm::MDString* mdStr(const std::string& s);
    llvm::Metadata* asI32(int32_t v);
    llvm::Metadata* asI64(int64_t v);
};