#pragma once
#include "dsl/ParserBase.h"
#include "dsl/AST.h"
#include <memory>

class FabricParser : public ParserBase {
public:
    FabricParser(Lexer& lex, llvm::SourceMgr& SM)
        : ParserBase(lex, SM) {}

    std::unique_ptr<FabricDefNode> parse();

private:
    // NetNode       parseNet();
    // CDCNode       parseCDC();
    // ClockDomain   parseClockDomain();
    // BusMatrix     parseBusMatrix();
};