#pragma once
#include "llvm/IR/Module.h"
#include <string>

class RegHeaderBackend {
  llvm::Module& M;
  public :
    explicit RegHeaderBackend(llvm::Module& mod) : M(mod) {}

    //Output directory
    bool generate(const std::string& ourputDir);
  private:
    //Read IR Metadata
    std::string readCompatible();
    //Code generation for each section
    void emitFileHeader(std::ostream& os, const std::string& compatible, const std::string& guard);
    void emitOffset(std::ostream& os);
    void emitFields(std::ostream& os);
    void emitAccessors(std::ostream& os);
    void emitRegMapStruct(std::ostream& os);
    void emitFileFooter(std::ostream& os, const std::string& guard);
    //Helper
    std::string makeGuard(const std::string& compatible);
    std::string makeStructName(const std::string& compatible);
    uint32_t fieldMask(int lo, int hi);
};