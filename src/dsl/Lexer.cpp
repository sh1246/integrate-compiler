#include "dsl/Lexer.h"

bool loadFile(const std::string& path, llvm::SourceMgr& SrcMgr) {
  //Load File into memory buffer
  auto bufOrErr = llvm::MemoryBuffer::getFile(path);
  if(!bufOrErr) {
    llvm::errs() << "Cannot open file: "<< path << "\n";
    return false;
  }

  //Register to SourceMgr : Later use it when calculate error location
  SrcMgr.AddNewSourceBuffer(std::move(*bufOrErr), llvm::SMLoc());
  return true;
}

Lexer::Lexer(llvm::SourceMgr& SM, unsigned bufID)
    : SrcMgr(SM), BufferID(bufID) {
    llvm::StringRef buf = SM.getMemoryBuffer(bufID)->getBuffer();
    CurPtr = buf.begin();
    End    = buf.end();
}

Token Lexer::peek() {
  const char* savePtr = CurPtr;
  Token tok = lex();
  CurPtr = savePtr;
  return tok;
}
Token Lexer::ahead(int offset) {
  const char* savePtr = CurPtr;
  Token tok;
  for (int i = 0; i <= offset; ++i)
    tok = lex();
  CurPtr = savePtr;
  return tok;
}

//This function get token from input file and classify it.
Token Lexer::lex() {
  skipWhitespaceAndComments();

  if(CurPtr >= End)
    return Token{TokenKind::EndOfFile, {}, curLoc()};
  char c = *CurPtr;

  // @ addressing : @0x00
  if (c == '@') return lexAtAddress();
  // For same root node e.g., /ip-definition/
  if (c == '/') return lexSlashOrRootNode();
  //String
  if (c == '"') return lexString();
  //Number (including hex format)
  if (std::isdigit(c)) return lexNumber();
  //Identifier
  if (std::isalpha(c) || c == '_') return lexIdentifierOrKeyword();
  // Arrow
  if (c == '-' && *(CurPtr+1) == '>') {
      CurPtr += 2;
      return Token{TokenKind::Arrow, "->", curLoc()};
  }
  if (c == '<' && *(CurPtr+1) == '-') {
      CurPtr += 2;
      return Token{TokenKind::BackArrow, "<-", curLoc()};
  }
  //Single char symbol
  TokenKind k = TokenKind::Unknown;
  switch(c) {
    case '{': k = TokenKind::LBrace;    break;
    case '}': k = TokenKind::RBrace;    break;
    case ';': k = TokenKind::Semicolon; break;
    case '=': k = TokenKind::Equals;    break;
    case ',': k = TokenKind::Comma;     break;
    case '<': k = TokenKind::LAngle;    break;
    case '>': k = TokenKind::RAngle;    break;
  }
  llvm::StringRef text(CurPtr, 1);
  ++CurPtr;
  return Token{k, text, curLoc()};
}

void Lexer::skipWhitespaceAndComments() {
  while (CurPtr < End) {
    //Pure white space
    if (std::isspace(*CurPtr)) { ++CurPtr; continue;}
    // Line comment
    if (*CurPtr == '/' && *(CurPtr+1) == '/') {
      while (CurPtr < End && *CurPtr != '\n') ++CurPtr;
      continue;
    }
    //Block comment
    if (*CurPtr == '/' && *(CurPtr+1) == '*') {
      CurPtr += 2;
      while (CurPtr < End && !(*CurPtr == '*' && *(CurPtr+1) == '/'))
        ++CurPtr;
      CurPtr += 2;
      continue;
    }
    break;
  }
}

Token Lexer::lexIdentifierOrKeyword() {
  const char* start = CurPtr;
  //Consume the identifier
  //include '-' e.g., bus-protocol, data-width, reset-value
  while(CurPtr < End && (std::isalnum(*CurPtr) || *CurPtr == '_' || *CurPtr == '-'))
    ++CurPtr;
  llvm::StringRef text(start, CurPtr - start);
  llvm::SMLoc loc = llvm::SMLoc::getFromPointer(start);

  return Token{TokenKind::Identifier, text, loc};
}

Token Lexer::lexString() {
  //Consume open '"'
  const char* start = CurPtr;
  ++CurPtr;
  while(CurPtr < End && *CurPtr != '"') {
    //Reach EOF without close "
    if (*CurPtr == '\n') {
      emitError(llvm::SMLoc::getFromPointer(start),
                "unterminated string literal");
      break;
    }
    ++CurPtr;
  }

  if (CurPtr < End)
    ++CurPtr; //Consume closed "
  //Text include " " pair
  //Parser will remove it.
  llvm::StringRef text(start,CurPtr-start);
  return Token{TokenKind::String, text, llvm::SMLoc::getFromPointer(start)};
}

Token Lexer::lexNumber() {
  const char* start = CurPtr;

  //0x hex format
  if (*CurPtr == '0' && CurPtr+1 < End && (*(CurPtr+1) == 'x' || *(CurPtr+1) == 'X')) {
    CurPtr += 2;
    while (CurPtr < End && std::isxdigit(*CurPtr))
      ++CurPtr;
  } else {
    //Decimal
    while (CurPtr < End && std::isdigit(*CurPtr))
    ++CurPtr;
  }
  llvm::StringRef text(start, CurPtr - start);
  return Token{TokenKind::Integer, text, llvm::SMLoc::getFromPointer(start)};
}

Token Lexer::lexAtAddress() {
    const char* start = CurPtr;
    ++CurPtr;   // consume @

    // Allow @0x00, @0x4000 format
    if (CurPtr < End && *CurPtr == '0' &&
        CurPtr+1 < End && (*(CurPtr+1) == 'x' || *(CurPtr+1) == 'X')) {
        CurPtr += 2;
        while (CurPtr < End && std::isxdigit(*CurPtr))
            ++CurPtr;
    } else {
        emitError(llvm::SMLoc::getFromPointer(start),
                  "expected hex address after '@' (e.g. @0x00)");
    }

    llvm::StringRef text(start, CurPtr - start);  // include ",",@
    return Token{TokenKind::AtAddress, text,
                 llvm::SMLoc::getFromPointer(start)};
}

Token Lexer::lexSlashOrRootNode() {
    const char* start = CurPtr;
    ++CurPtr;   // Consume first '/'

    // /ip-definition/, /fabric-definition/, /soc-definition/
    if (CurPtr < End && (std::isalpha(*CurPtr) || *CurPtr == '_')) {
        while (CurPtr < End && *CurPtr != '/')
            ++CurPtr;

        if (CurPtr >= End) {
            emitError(llvm::SMLoc::getFromPointer(start),
                      "unterminated root node, expected closing '/'");
            return Token{TokenKind::Unknown, llvm::StringRef(start, static_cast<size_t>(CurPtr - start)),
                         llvm::SMLoc::getFromPointer(start)};
        }

        ++CurPtr;   // consume close '/' 
        llvm::StringRef text(start, CurPtr - start);

        // Classify keyword
        TokenKind kind = TokenKind::Unknown;
        if      (text == "/ip-definition/")     kind = TokenKind::Slash_IP;
        else if (text == "/fabric-definition/") kind = TokenKind::Slash_Fabric;
        else if (text == "/soc-definition/")    kind = TokenKind::Slash_SoC;
        else {
            emitError(llvm::SMLoc::getFromPointer(start),
                      "unknown root node '" + text + "'");
        }

        return Token{kind, text, llvm::SMLoc::getFromPointer(start)};
    }

    // simple '/' symbol
    return Token{TokenKind::Slash, {start, 1},
                 llvm::SMLoc::getFromPointer(start)};
}

//Clang style error print.
void Lexer::emitError(llvm::SMLoc loc, const llvm::Twine& msg) {
    SrcMgr.PrintMessage(loc, llvm::SourceMgr::DK_Error, msg);
}