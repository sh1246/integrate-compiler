#pragma once
#include "dsl/Lexer.h"
#include "llvm/Support/SourceMgr.h"

class ParserBase {
  protected :
    Lexer& lexer;
    llvm::SourceMgr& SrcMgr;
    Token CurTok;
    ParserBase(Lexer& lex, llvm::SourceMgr& SM)
    : lexer(lex), SrcMgr(SM) {
      advance();
    }
    void        advance();
    bool        expect(TokenKind k, const llvm::Twine& msg);
    bool        consume(TokenKind k);
    bool        check(TokenKind k) const;
    std::string parseStringValue();
    uint32_t    parseAngledInt();
    void emitError(llvm::SMLoc loc, const llvm::Twine& msg);
};