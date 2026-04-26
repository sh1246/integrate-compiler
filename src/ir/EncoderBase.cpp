#include "ir/EncoderBase.h"

llvm::MDString* EncoderBase::mdStr(const std::string& s) {
    return llvm::MDString::get(ctx, s);
}

llvm::Metadata* EncoderBase::asI32(int32_t v) {
    return llvm::ConstantAsMetadata::get(
        llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(ctx), v, true));
}

llvm::Metadata* EncoderBase::asI64(int64_t v) {
    return llvm::ConstantAsMetadata::get(
        llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(ctx), v, true));
}