#pragma once
#include "dsl/ParserBase.h"
#include "dsl/AST.h"
#include <memory>

class SoCParser : public ParserBase {
public:
    SoCParser(Lexer& lex, llvm::SourceMgr& SM)
        : ParserBase(lex, SM) {}

    std::unique_ptr<SoCDefNode> parse();

private:
    // FabricInstNode parseFabricInst();
    // MemoryMapNode  parseMemoryMap();
};