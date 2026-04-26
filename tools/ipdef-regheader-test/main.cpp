#include "dsl/Lexer.h"
#include "dsl/IPDefParser.h"
#include "backends/RegHeaderBackend.h"
#include "ir/IPDefEncoder.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"

static const char* tokenKindStr(TokenKind k) {
    switch(k) {
        case TokenKind::LBrace:      return "LBrace";
        case TokenKind::RBrace:      return "RBrace";
        case TokenKind::Semicolon:   return "Semicolon";
        case TokenKind::Equals:      return "Equals";
        case TokenKind::Slash:       return "Slash";
        case TokenKind::Comma:       return "Comma";
        case TokenKind::LAngle:      return "LAngle";
        case TokenKind::RAngle:      return "RAngle";
        case TokenKind::Arrow:       return "Arrow";
        case TokenKind::BackArrow:   return "BackArrow";
        case TokenKind::Identifier:  return "Identifier";
        case TokenKind::String:      return "String";
        case TokenKind::Integer:     return "Integer";
        case TokenKind::AtAddress:   return "AtAddress";
        case TokenKind::Slash_IP:    return "Slash_IP";
        case TokenKind::Slash_Fabric:return "Slash_Fabric";
        case TokenKind::Slash_SoC:   return "Slash_SoC";
        case TokenKind::EndOfFile:   return "EOF";
        default:                     return "Unknown";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        llvm::errs() << "Usage: lexer-test <file.ipdef>\n";
        return 1;
    }

    llvm::SourceMgr SrcMgr;
    auto bufOrErr = llvm::MemoryBuffer::getFile(argv[1]);
    if (!bufOrErr) {
        llvm::errs() << "Cannot open: " << argv[1] << "\n";
        return 1;
    }
    unsigned bufID = SrcMgr.AddNewSourceBuffer(
        std::move(*bufOrErr), llvm::SMLoc());

    Lexer lexer(SrcMgr, bufID);
    IPDefParser ipdef_parser(lexer, SrcMgr);
    auto ipdef = ipdef_parser.parseIPDef();
    //IR generation
    llvm::LLVMContext ctx;
    auto ir_module = std::make_unique<llvm::Module>(ipdef->compatible, ctx);
    IPDefEncoder encoder(*ir_module);
    encoder.encode(*ipdef);
    //IR export
    std::error_code EC;
    llvm::raw_fd_ostream outFile("ipdef.ll",EC, llvm::sys::fs::OF_Text);
    //ir_module->print(llvm::outs(), nullptr);
    ir_module->print(outFile, nullptr);
    outFile.close();

    RegHeaderBackend backend(*ir_module);
    backend.generate(".");



    //Results dump lexer
    llvm::outs() << "=== Tokens for: " << argv[1] << " ===\n";
    int count = 0;
    while (true) {
        Token tok = lexer.lex();
        auto [line, col] = SrcMgr.getLineAndColumn(tok.loc);
        llvm::outs() << llvm::format(
            "[%4d] %-16s  %3d:%-3d  '%s'\n",
            count++,
            tokenKindStr(tok.kind),
            line, col,
            tok.text.str().c_str()
        );
        if (tok.is(TokenKind::EndOfFile)) break;
    }
    llvm::outs() << "=== Total: " << count << " tokens ===\n";
    //Results of ipdef parsing
    if (ipdef)
        ipdef->dump();
    else 
        std::printf("parse failed.\n");


    return 0;
}