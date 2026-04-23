#pragma once
#include "dsl/Token.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Twine.h" 

class Lexer {
    llvm::SourceMgr& SrcMgr;
    const char* CurPtr; //Current position
    const char* End;    //End of buffer
    unsigned    BufferID; //SourceMgr buffer ID
  public:
    Lexer(llvm::SourceMgr& SM, unsigned bufID);
    Token lex(); //Return Next token
    Token peek(); //Check next token without consume it
    Token ahead(int );
  private:
    Token lexIdentifierOrKeyword();
    Token lexString();
    Token lexNumber();
    Token lexAtAddress();
    Token lexSlashOrRootNode();

    void skipWhitespaceAndComments();
    //SourceMgr based error printing
    void emitError(llvm::SMLoc loc, const llvm::Twine& msg);
    llvm::SMLoc curLoc() const {
      return llvm::SMLoc::getFromPointer(CurPtr);
    }
};