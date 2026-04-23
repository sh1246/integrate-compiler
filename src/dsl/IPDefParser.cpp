#include "dsl/IPDefParser.h"

// regmap@0x00 { label = "IP_CTRL_REG"; ... };
RegmapNode IPDefParser::parseRegmap() { //Parsing single register
  RegmapNode node;
  node.size = 4;

  // @0x00 pattern
  if (check(TokenKind::AtAddress)) {
    std::string addrStr = CurTok.text.str().substr(1); //Remove @
    node.offset = (uint32_t)std::stoul(addrStr,nullptr,16);
    advance();
  }

  expect(TokenKind::LBrace, "expected '{' after regmap address ");

  while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
    if(!check(TokenKind::Identifier)) {
      emitError(CurTok.loc, "exptected property name");
      advance();
      continue;
    }
    std::string key = CurTok.text.str();
    advance();

    if(key == "fields") { //fields { FIELD_NAME { bits = <0 1>; desc = "example field"; }; } pattern
      expect(TokenKind::LBrace, "expected '{' after 'fields'");
      while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile))
        node.fields.push_back(parseField());
      expect(TokenKind::RBrace, "expected '}'");
      consume(TokenKind::Semicolon);
      continue;
    }
    expect(TokenKind::Equals, "expected '='");
    if (key == "label") node.label = parseStringValue();
    else if(key == "access") node.access = parseStringValue();
    else if(key == "reset-value") node.resetValue = parseAngledInt();
    else if(key == "reg") { //reg = <offset size> pattern
      expect(TokenKind::LAngle, "expected '<'");
      node.offset = (uint32_t)std::stoul(CurTok.text.str(), nullptr, 0);
      advance();
      node.size   = (uint32_t)std::stoul(CurTok.text.str(), nullptr, 0);
      advance();
      expect(TokenKind::RAngle, "expected '>'");
    } else {
      emitError(CurTok.loc, "unknown regmap property '"  + key + "'");
      advance();
    }
    expect(TokenKind::Semicolon, "expected ';'");
  }
  expect(TokenKind::RBrace, "expected '}'");
  consume(TokenKind::Semicolon);
  return node;
}

//FIELD_NAME { bits = <0 1>; desc = "example field"; };
FieldNode IPDefParser::parseField() {
  FieldNode node;
  //Field's name
  if(!check(TokenKind::Identifier)) {
    emitError(CurTok.loc, "expected field name");
    return node;
  }
  node.name = CurTok.text.str();
  advance();
  expect(TokenKind::LBrace, "expected '{' after field name");

  while(!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
    if(!check(TokenKind::Identifier)) {
      emitError(CurTok.loc, "expected property name ");
      advance();
      continue;
    }
    std::string key = CurTok.text.str();
    advance();
    expect(TokenKind::Equals, "expected '=' after property name");

    if(key == "bits") { //bits = <lo hi> TODO:Change lo,hi -> hi,lo
      expect(TokenKind::LAngle, "expected '<'");
      node.bitLo = (int)std::stoul(CurTok.text.str(), nullptr, 0);
      advance();
      node.bitHi = (int)std::stoul(CurTok.text.str(), nullptr, 0);
      advance();
      expect(TokenKind::RAngle, "expect '>'");
    } else if (key == "desc") { //Description field
      node.desc = parseStringValue();
    } else { //Handle about unkwon field
      emitError(CurTok.loc, "unknown field property '" + key + "'");
      advance();
    }
    expect(TokenKind::Semicolon, "expected ';'");
  }
  expect(TokenKind::RBrace, "expected '}'");
  consume(TokenKind::Semicolon);
  return node;
}

// s_axi { type = "AXI4-Lite"; direction = "slave"; };
PortNode IPDefParser::parsePort() {
    PortNode node;

    node.name = CurTok.text.str();
    advance();

    expect(TokenKind::LBrace, "expected '{'");

    while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
        std::string key = CurTok.text.str();
        advance();
        expect(TokenKind::Equals, "expected '='");

        if      (key == "type")       node.portType  = parseStringValue();
        else if (key == "direction")  node.direction = parseStringValue();
        else if (key == "data-width") node.dataWidth = (int)parseAngledInt();
        else if (key == "addr-width")      node.dataWidth = (int)parseAngledInt();
        else {
            emitError(CurTok.loc,
                "unknown port property '" + key + "'");
            advance();
        }
        expect(TokenKind::Semicolon, "expected ';'");
    }

    expect(TokenKind::RBrace, "expected '}'");
    consume(TokenKind::Semicolon);
    return node;
}

// clk_sys { frequency = <100000000>; domain = "syc_clock"; };
ClockPortNode IPDefParser::parseClock() {
    ClockPortNode node;

    node.name = CurTok.text.str();
    advance();

    expect(TokenKind::LBrace, "expected '{'");

    while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
        std::string key = CurTok.text.str();
        advance();
        expect(TokenKind::Equals, "expected '='");

        if (key == "frequency") {
            node.frequency = (uint64_t)parseAngledInt();
        } else if (key == "domain") {
            node.domain = parseStringValue();
        } else {
            emitError(CurTok.loc,
                "unknown clock property '" + key + "'");
            advance();
        }
        expect(TokenKind::Semicolon, "expected ';'");
    }

    expect(TokenKind::RBrace, "expected '}'");
    consume(TokenKind::Semicolon);
    return node;
}

//IPdef Top-levle parser
std::unique_ptr<IPDefNode> IPDefParser::parseIPDef() {
  auto node = std::make_unique<IPDefNode>();
  // /ip-definition/
  if(!expect(TokenKind::Slash_IP, "expected '/ip-definition/'")) return nullptr;
  
  // top level {
  if(!expect(TokenKind::LBrace, "expected '{' after '/ip-definition/'")) return nullptr;

  while (!check(TokenKind::RBrace) &&  !check(TokenKind::EndOfFile)) {
    if(!check(TokenKind::Identifier)) {
      emitError(CurTok.loc, "Expected property or block name");
      advance();
      continue;
    }

    std::string key = CurTok.text.str();
    advance();
    //Block i.e., { without = 
    if(key == "clocks" & check(TokenKind::LBrace)) { //clocks { ... };
      advance(); // consume {
      while(!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile))
        node->clocks.push_back(parseClock());
      expect(TokenKind::RBrace,"expected '}'");
      consume(TokenKind::Semicolon);
      continue;
    }
    if (key == "ports" && check(TokenKind::LBrace)) { //ports { ... };
      advance(); // consume {
      while(!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile))
        node->ports.push_back(parsePort());
      expect(TokenKind::RBrace,"expected '}'");
      consume(TokenKind::Semicolon);
      continue;
    }
    if (key == "regmap" && check(TokenKind::AtAddress)) {
      node->regmaps.push_back(parseRegmap());
      continue;
    }
    //Normal property with =
    expect(TokenKind::Equals, "expected '='");
    if (key == "compatible") node->compatible = parseStringValue();
    else if (key == "data-width") node->dataWidth = (int)parseAngledInt();
    else if (key == "addr-width") node->addrWidth = (int)parseAngledInt();
    else {
      emitError(CurTok.loc, "unkwon ip-definition property '"+key+"'");
      advance();
    }
    expect(TokenKind::Semicolon,"expected ';'");
  }
  expect(TokenKind::RBrace, "expected '}'");
  consume(TokenKind::Semicolon);
  return node;
}