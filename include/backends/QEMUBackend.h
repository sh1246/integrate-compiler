#pragma once
#include "llvm/IR/Module.h"
#include <string>
#include <vector>
#include <cstdint>

class QEMUBackend {
  llvm::Module &M;
  struct RegInfo {
    std::string name;
    uint32_t offset;
    uint32_t size;
    std::string access; //RW, RO, WO
    uint32_t resetValue;
  };
  public:
    explicit QEMUBackend(llvm::Module& mod) : M(mod) {}
    bool generate(const std::string& outputDir);
  private :
    //Read IR Metadata
    std::string readCompatible();
    std::vector<RegInfo> readRegmaps();
    uint32_t calcMemRegionSize(const std::vector<RegInfo>& regs);
    //Header generation
    bool generateHeader(const std::string& path, const std::string& compatible, const std::vector<RegInfo>& regs);
    void emitHeaderTop(std::ostream& os, const std::string& compatible, const std::string& guard, const std::string& typeName);
    void emitHookDecls(std::ostream& os, const std::string&typeName, const std::vector<RegInfo>& regs);
    void emitSetterDecls(std::ostream& os, const std::string& typeName, const std::vector<RegInfo>& guard);

    //C code generation
    bool generateSource(const std::string& path, const std::string& compatible, const std::vector<RegInfo>& regs);
    void emitIncludes(std::ostream& of, const std::string& typeName);
    void emitStateStruct(std::ostream& os, const std::string& typeName, const std::vector<RegInfo>& regs);
    void emitReadHandler(std::ostream& os, const std::string& typeName, const std::vector<RegInfo>& regs);
    void emitWriteHandler(std::ostream& os, const std::string& typeNAme, const std::vector<RegInfo>& regs);
    void emitReset(std::ostream& os, const std::string& typeName, const std::vector<RegInfo>& regs);
    void emitRealize(std::ostream& os, const std::string& typeName, uint32_t memSize);
    void emitSetters(std::ostream& os, const std::string&typeName, const std::vector<RegInfo>& regs);
    void emitWeakHooks(std::ostream& os, const std::string& compatible, const std::string& typeName);
    
    //Helper
    std::string makeTypeName(const std::string& compatible);
    std::string makeUpperName(const std::string& compatible);
    std::string makeMacroName(const std::string& compatible);
};