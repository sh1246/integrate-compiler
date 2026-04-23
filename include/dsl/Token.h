#pragma once
#include "llvm/Support/SMLoc.h"
#include "llvm/ADT/StringRef.h"

enum class TokenKind {
  //Defining Token
  LBrace,    // {
  RBrace,    // }
  Semicolon, // ;
  Equals,    // =
  Slash,     // /
  Comma,     // ,
  LAngle,    // <
  RAngle,    // >
  LBracket,  // [
  RBracket,  // ]
  Arrow,     // ->
  BackArrow, // <-

  //Lietral (Include Device tree specific)
  Identifier, //Compatible, regmap, ports .. etc
  String,     // "MyIP,UART-v2"
  Integer,    // 32, 0x4000
  AtAddress,  // @0x00

  //Special case
  Slash_IP,     // /ip-definition/
  Slash_Fabric, // /fabric-definition/
  Slash_SoC,    // /soc-definition/

  EndOfFile,
  Unknown,
};

struct Token {
  TokenKind kind;
  llvm::StringRef text;
  llvm::SMLoc loc; //Location of SourceMgr (for error printing)
  bool is(TokenKind k) const    {return kind == k;}
  bool isNot(TokenKind k) const {return kind != k;}
};