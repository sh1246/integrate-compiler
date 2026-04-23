#include "dsl/ParserBase.h"
#include "llvm/Support/raw_ostream.h"
void ParserBase::advance() {
  CurTok = lexer.lex();
}
bool ParserBase::check(TokenKind k) const {
  return CurTok.kind == k;
}
bool ParserBase::consume(TokenKind k) {
  if(check(k)) { advance(); return true;}
  return false;
}
bool ParserBase::expect(TokenKind k, const llvm::Twine& msg) {
  if(!check(k)) {
    emitError(CurTok.loc, msg);
    return false;
  }
  advance();
  return true;
}
void ParserBase::emitError(llvm::SMLoc loc, const llvm::Twine& msg) {
    SrcMgr.PrintMessage(loc, llvm::SourceMgr::DK_Error, msg);
}
std::string ParserBase::parseStringValue() {
  if (!check(TokenKind::String)) {
    emitError(CurTok.loc, "expected string literal");
    return "";
  }
  //Remove "" (e.g., "foo" -> foo)
  llvm::StringRef text = CurTok.text;
  std::string val = text.substr(1, text.size() - 2).str();
  advance();
  return val; 
}
uint32_t ParserBase::parseAngledInt() {
  //<Int>
  if(!expect(TokenKind::LAngle, "expected '<'")) return 0;

  if(!check(TokenKind::Integer)) {
    emitError(CurTok.loc, "expected integer inside '<>'");
    return 0;
  }

  uint32_t val = (uint32_t)std::stoul(CurTok.text.str(), nullptr, 0);
  advance();
  expect(TokenKind::RAngle, "expected '>'");
  return val;
}