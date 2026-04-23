#pragma once
#include "dsl/ParserBase.h"
#include "dsl/AST.h"
#include <memory>

class IPDefParser : public ParserBase {
  public :
    IPDefParser(Lexer& lex, llvm::SourceMgr& SM) : ParserBase(lex,SM) {}
    std::unique_ptr<IPDefNode> parseIPDef();
  private :
    RegmapNode parseRegmap();
    FieldNode parseField();
    PortNode parsePort();
    ClockPortNode parseClock();
};