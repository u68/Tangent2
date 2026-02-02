import std.stdio;
import std.array;
import std.string;
import std.file : exists, isFile, read, write, FileException;
import std.path : dirName, buildPath;
import std.conv : to;
import std.uni : toLower;
import core.stdc.stdlib : exit;

// Build error codes
enum BuildError {
    SUCCESS = 0,
    FILE_NOT_FOUND = 1,
    INVALID_INPUT = 2,
    PARSE_ERROR = 3,
    ASSEMBLY_ERROR = 4,
    OUTPUT_ERROR = 5,
    UNKNOWN_ERROR = 99
}

struct BuildResult {
    BuildError errorCode;
    string errorMessage;
    string[] warnings;
    bool success() const { return errorCode == BuildError.SUCCESS; }
}

struct SourceLine {
    string text;
    string filePath;
    size_t lineNumber;
}

struct ImmValue {
    bool isRef;
    string label;
    long value;
    int widthBytes; // 1 or 2
}

struct AsmStats {
    long stackDepth;
    long maxStackDepth;
    long maxStoreAddr;
    bool hasStoreAddr;
}

enum ubyte OP_MOV8_REG_REG = 0;
enum ubyte OP_MOV16_REG_REG = OP_MOV8_REG_REG + 1;
enum ubyte OP_MOV16_REG_IMM = OP_MOV16_REG_REG + 1;
enum ubyte OP_LOAD8_REG_MREG = OP_MOV16_REG_IMM + 1;
enum ubyte OP_LOAD8_REG_MIMM = OP_LOAD8_REG_MREG + 1;
enum ubyte OP_LOAD16_REG_MREG = OP_LOAD8_REG_MIMM + 1;
enum ubyte OP_LOAD16_REG_MIMM = OP_LOAD16_REG_MREG + 1;
enum ubyte OP_STORE8_MREG_REG = OP_LOAD16_REG_MIMM + 1;
enum ubyte OP_STORE8_MREG_IMM = OP_STORE8_MREG_REG + 1;
enum ubyte OP_STORE8_MIMM_REG = OP_STORE8_MREG_IMM + 1;
enum ubyte OP_STORE8_MIMM_IMM = OP_STORE8_MIMM_REG + 1;
enum ubyte OP_STORE16_MREG_REG = OP_STORE8_MIMM_IMM + 1;
enum ubyte OP_STORE16_MREG_IMM = OP_STORE16_MREG_REG + 1;
enum ubyte OP_STORE16_MIMM_REG = OP_STORE16_MREG_IMM + 1;
enum ubyte OP_STORE16_MIMM_IMM = OP_STORE16_MIMM_REG + 1;

enum ubyte OP_PUSH8_REG = OP_STORE16_MIMM_IMM + 1;
enum ubyte OP_PUSH8_IMM = OP_PUSH8_REG + 1;
enum ubyte OP_PUSH16_REG = OP_PUSH8_IMM + 1;
enum ubyte OP_PUSH16_IMM = OP_PUSH16_REG + 1;
enum ubyte OP_POP8_REG = OP_PUSH16_IMM + 1;
enum ubyte OP_POP16_REG = OP_POP8_REG + 1;
enum ubyte OP_PUSH_LR = OP_POP16_REG + 1;
enum ubyte OP_POP_PC = OP_PUSH_LR + 1;
enum ubyte OP_PUSH_SP = OP_POP_PC + 1;
enum ubyte OP_POP_SP = OP_PUSH_SP + 1;

enum ubyte OP_ADD8_REG_REG = OP_POP_SP + 1;
enum ubyte OP_ADD16_REG_REG = OP_ADD8_REG_REG + 1;
enum ubyte OP_ADD16_REG_IMM = OP_ADD16_REG_REG + 1;
enum ubyte OP_SUB8_REG_REG = OP_ADD16_REG_IMM + 1;
enum ubyte OP_SUB16_REG_REG = OP_SUB8_REG_REG + 1;
enum ubyte OP_SUB16_REG_IMM = OP_SUB16_REG_REG + 1;
enum ubyte OP_INC8_REG = OP_SUB16_REG_IMM + 1;
enum ubyte OP_INC16_REG = OP_INC8_REG + 1;
enum ubyte OP_DEC8_REG = OP_INC16_REG + 1;
enum ubyte OP_DEC16_REG = OP_DEC8_REG + 1;
enum ubyte OP_MUL8_REG_REG = OP_DEC16_REG + 1;
enum ubyte OP_MUL16_REG_REG = OP_MUL8_REG_REG + 1;
enum ubyte OP_MUL16_REG_IMM = OP_MUL16_REG_REG + 1;
enum ubyte OP_DIV8_REG_REG = OP_MUL16_REG_IMM + 1;
enum ubyte OP_DIV16_REG_REG = OP_DIV8_REG_REG + 1;
enum ubyte OP_DIV16_REG_IMM = OP_DIV16_REG_REG + 1;
enum ubyte OP_MOD8_REG_REG = OP_DIV16_REG_IMM + 1;
enum ubyte OP_MOD8_REG_IMM = OP_MOD8_REG_REG + 1;
enum ubyte OP_MOD16_REG_REG = OP_MOD8_REG_IMM + 1;
enum ubyte OP_MOD16_REG_IMM = OP_MOD16_REG_REG + 1;

enum ubyte OP_AND8_REG_REG = OP_MOD16_REG_IMM + 1;
enum ubyte OP_AND16_REG_REG = OP_AND8_REG_REG + 1;
enum ubyte OP_AND16_REG_IMM = OP_AND16_REG_REG + 1;
enum ubyte OP_OR8_REG_REG = OP_AND16_REG_IMM + 1;
enum ubyte OP_OR16_REG_REG = OP_OR8_REG_REG + 1;
enum ubyte OP_OR16_REG_IMM = OP_OR16_REG_REG + 1;
enum ubyte OP_XOR8_REG_REG = OP_OR16_REG_IMM + 1;
enum ubyte OP_XOR16_REG_REG = OP_XOR8_REG_REG + 1;
enum ubyte OP_XOR16_REG_IMM = OP_XOR16_REG_REG + 1;
enum ubyte OP_NOT8_REG = OP_XOR16_REG_IMM + 1;
enum ubyte OP_NOT16_REG = OP_NOT8_REG + 1;

enum ubyte OP_CMP8_REG_REG = OP_NOT16_REG + 1;
enum ubyte OP_CMP16_REG_REG = OP_CMP8_REG_REG + 1;
enum ubyte OP_CMP16_REG_IMM = OP_CMP16_REG_REG + 1;
enum ubyte OP_SLL8_REG_REG = OP_CMP16_REG_IMM + 1;
enum ubyte OP_SLL8_REG_IMM = OP_SLL8_REG_REG + 1;
enum ubyte OP_SLL16_REG_REG = OP_SLL8_REG_IMM + 1;
enum ubyte OP_SLL16_REG_IMM = OP_SLL16_REG_REG + 1;
enum ubyte OP_SRL8_REG_REG = OP_SLL16_REG_IMM + 1;
enum ubyte OP_SRL8_REG_IMM = OP_SRL8_REG_REG + 1;
enum ubyte OP_SRL16_REG_REG = OP_SRL8_REG_IMM + 1;
enum ubyte OP_SRL16_REG_IMM = OP_SRL16_REG_REG + 1;
enum ubyte OP_SRA8_REG_REG = OP_SRL16_REG_IMM + 1;
enum ubyte OP_SRA8_REG_IMM = OP_SRA8_REG_REG + 1;
enum ubyte OP_SRA16_REG_REG = OP_SRA8_REG_IMM + 1;
enum ubyte OP_SRA16_REG_IMM = OP_SRA16_REG_REG + 1;

enum ubyte OP_B_REG = OP_SRA16_REG_IMM + 1;
enum ubyte OP_B_IMM = OP_B_REG + 1;
enum ubyte OP_BEQ_IMM = OP_B_IMM + 1;
enum ubyte OP_BNE_IMM = OP_BEQ_IMM + 1;
enum ubyte OP_BLT_IMM = OP_BNE_IMM + 1;
enum ubyte OP_BLE_IMM = OP_BLT_IMM + 1;
enum ubyte OP_BGT_IMM = OP_BLE_IMM + 1;
enum ubyte OP_BGE_IMM = OP_BGT_IMM + 1;
enum ubyte OP_BL_REG = OP_BGE_IMM + 1;
enum ubyte OP_BL_IMM = OP_BL_REG + 1;
enum ubyte OP_RT = OP_BL_IMM + 1;

enum ubyte OP_MOV8_REG_IMM = 0x60;
enum ubyte OP_ADD8_REG_IMM = 0x70;
enum ubyte OP_SUB8_REG_IMM = 0x80;
enum ubyte OP_MUL8_REG_IMM = 0x90;
enum ubyte OP_DIV8_REG_IMM = 0xA0;
enum ubyte OP_AND8_REG_IMM = 0xB0;
enum ubyte OP_OR8_REG_IMM  = 0xC0;
enum ubyte OP_XOR8_REG_IMM = 0xD0;
enum ubyte OP_CMP8_REG_IMM = 0xE0;

enum ubyte OP_SYSCALL = 0xF0;
enum ubyte OP_END = 0xFF;

enum int SYSCALL_COPY_TO_RAM = 0
enum int SYSCALL_GET_ELEMENT_FIELD = SYSCALL_COPY_TO_RAM + 1;
enum int SYSCALL_SET_ELEMENT_FIELD = SYSCALL_GET_ELEMENT_FIELD + 1;
enum int SYSCALL_RENDER_ELEMENT = SYSCALL_SET_ELEMENT_FIELD + 1;
enum int SYSCALL_DRAW_LINE = SYSCALL_RENDER_ELEMENT + 1;
enum int SYSCALL_SET_PIXEL = SYSCALL_DRAW_LINE + 1;
enum int SYSCALL_GET_PIXEL = SYSCALL_SET_PIXEL + 1;
enum int SYSCALL_DRAW_IMAGE = SYSCALL_GET_PIXEL + 1;
enum int SYSCALL_DRAW_RECT = SYSCALL_DRAW_IMAGE + 1;
enum int SYSCALL_DRAW_TEXT = SYSCALL_DRAW_RECT + 1;
enum int SYSCALL_SLEEP = SYSCALL_DRAW_TEXT + 1;
enum int SYSCALL_GET_TIME_MS = SYSCALL_SLEEP + 1;
enum int SYSCALL_GET_TIME_S = SYSCALL_GET_TIME_MS + 1;
enum int SYSCALL_FS_READ = SYSCALL_GET_TIME_S + 1;
enum int SYSCALL_FS_WRITE = SYSCALL_FS_READ + 1;
enum int SYSCALL_FS_MKDIR = SYSCALL_FS_WRITE + 1;
enum int SYSCALL_FS_DELETE = SYSCALL_FS_MKDIR + 1;
enum int SYSCALL_FS_RENAME = SYSCALL_FS_DELETE + 1;
enum int SYSCALL_FS_MOVE = SYSCALL_FS_RENAME + 1;
enum int SYSCALL_STOP = SYSCALL_FS_MOVE + 1;
enum int SYSCALL_END = SYSCALL_STOP + 1;

immutable int[string] syscallMap = [
    "GET_ELEMENT_FIELD": SYSCALL_GET_ELEMENT_FIELD,
    "SET_ELEMENT_FIELD": SYSCALL_SET_ELEMENT_FIELD,
    "RENDER_ELEMENT": SYSCALL_RENDER_ELEMENT,
    "DRAW_LINE": SYSCALL_DRAW_LINE,
    "SET_PIXEL": SYSCALL_SET_PIXEL,
    "GET_PIXEL": SYSCALL_GET_PIXEL,
    "DRAW_IMAGE": SYSCALL_DRAW_IMAGE,
    "DRAW_RECT": SYSCALL_DRAW_RECT,
    "DRAW_TEXT": SYSCALL_DRAW_TEXT,
    "SLEEP": SYSCALL_SLEEP,
    "GET_TIME_MS": SYSCALL_GET_TIME_MS,
    "GET_TIME_S": SYSCALL_GET_TIME_S,
    "FS_READ": SYSCALL_FS_READ,
    "FS_WRITE": SYSCALL_FS_WRITE,
    "FS_MKDIR": SYSCALL_FS_MKDIR,
    "FS_DELETE": SYSCALL_FS_DELETE,
    "FS_RENAME": SYSCALL_FS_RENAME,
    "FS_MOVE": SYSCALL_FS_MOVE,
    "STOP": SYSCALL_STOP,
    "END": SYSCALL_END,
];

// Check if a file exists and is readable
bool validateInputFile(string filePath, ref string errorMsg) {
    if (filePath.length == 0) {
        errorMsg = "No file path specified";
        return false;
    }
    
    if (!exists(filePath)) {
        errorMsg = "File not found: " ~ filePath;
        return false;
    }
    
    if (!isFile(filePath)) {
        errorMsg = "Path is not a file: " ~ filePath;
        return false;
    }
    
    return true;
}

bool isIdentifierStart(dchar ch) {
    return (ch == '_') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

bool isIdentifierChar(dchar ch) {
    return isIdentifierStart(ch) || (ch >= '0' && ch <= '9');
}

bool isIdentifier(string s) {
    if (s.length == 0) return false;
    if (!isIdentifierStart(s[0])) return false;
    foreach (ch; s[1 .. $]) {
        if (!isIdentifierChar(ch)) return false;
    }
    return true;
}

string stripComment(string line) {
    bool inSingle = false;
    bool inDouble = false;
    for (size_t i = 0; i < line.length; i++) {
        char c = line[i];
        if (c == '\'' && !inDouble) {
            inSingle = !inSingle;
        } else if (c == '"' && !inSingle) {
            inDouble = !inDouble;
        }
        if (!inSingle && !inDouble) {
            if (c == ';') {
                return line[0 .. i];
            }
            if (c == '/' && i + 1 < line.length && line[i + 1] == '/') {
                return line[0 .. i];
            }
        }
    }
    return line;
}

string applyDefines(string line, string[string] defines) {
    bool inSingle = false;
    bool inDouble = false;
    string result;
    size_t i = 0;
    while (i < line.length) {
        char c = line[i];
        if (c == '\'' && !inDouble) {
            inSingle = !inSingle;
            result ~= c;
            i++;
            continue;
        }
        if (c == '"' && !inSingle) {
            inDouble = !inDouble;
            result ~= c;
            i++;
            continue;
        }
        if (!inSingle && !inDouble && isIdentifierStart(c)) {
            size_t start = i;
            i++;
            while (i < line.length && isIdentifierChar(line[i])) {
                i++;
            }
            string token = line[start .. i];
            if (auto rep = token in defines) {
                result ~= *rep;
            } else {
                result ~= token;
            }
            continue;
        }
        result ~= c;
        i++;
    }
    return result;
}

void preprocessFile(string filePath, ref string[string] defines, ref SourceLine[] lines, ref BuildResult result) {
    string content;
    try {
        content = cast(string) read(filePath);
    } catch (FileException e) {
        result.errorCode = BuildError.FILE_NOT_FOUND;
        result.errorMessage = "Failed to read input file: " ~ e.msg;
        return;
    }

    auto dir = dirName(filePath);
    auto rawLines = content.splitLines();
    foreach (lineIndex, rawLine; rawLines) {
        string trimmed = strip(rawLine);
        if (trimmed.length == 0) {
            continue;
        }

        string stripped = stripComment(rawLine);
        trimmed = strip(stripped);
        if (trimmed.length == 0) {
            continue;
        }

        if (trimmed.startsWith("#include")) {
            auto quoteStart = trimmed.indexOf('"');
            auto quoteEnd = trimmed.lastIndexOf('"');
            if (quoteStart == size_t.max || quoteEnd == size_t.max || quoteEnd <= quoteStart) {
                result.errorCode = BuildError.PARSE_ERROR;
                result.errorMessage = "Invalid #include syntax at " ~ filePath ~ ":" ~ (lineIndex + 1).to!string;
                return;
            }
            string includePath = trimmed[quoteStart + 1 .. quoteEnd];
            string resolved = buildPath(dir, includePath);
            preprocessFile(resolved, defines, lines, result);
            if (!result.success()) return;
            continue;
        }

        if (trimmed.startsWith("#define")) {
            string rest = trimmed[7 .. $].strip;
            if (rest.length == 0) {
                result.errorCode = BuildError.PARSE_ERROR;
                result.errorMessage = "Invalid #define syntax at " ~ filePath ~ ":" ~ (lineIndex + 1).to!string;
                return;
            }
            size_t spaceIndex = rest.indexOfAny(" \t");
            string name;
            string value;
            if (spaceIndex == size_t.max) {
                name = rest;
                value = "";
            } else {
                name = rest[0 .. spaceIndex].strip;
                value = rest[spaceIndex .. $].strip;
            }
            if (!isIdentifier(name)) {
                result.errorCode = BuildError.PARSE_ERROR;
                result.errorMessage = "Invalid #define name at " ~ filePath ~ ":" ~ (lineIndex + 1).to!string;
                return;
            }
            defines[name] = value;
            continue;
        }

        string replaced = applyDefines(trimmed, defines);
        if (replaced.strip.length == 0) {
            continue;
        }
        lines ~= SourceLine(replaced, filePath, lineIndex + 1);
    }
}

string[] splitOperands(string s) {
    string[] parts;
    bool inSingle = false;
    bool inDouble = false;
    int bracketDepth = 0;
    size_t start = 0;
    for (size_t i = 0; i < s.length; i++) {
        char c = s[i];
        if (c == '\'' && !inDouble) inSingle = !inSingle;
        if (c == '"' && !inSingle) inDouble = !inDouble;
        if (!inSingle && !inDouble) {
            if (c == '[') bracketDepth++;
            if (c == ']') bracketDepth--;
            if (c == ',' && bracketDepth == 0) {
                parts ~= s[start .. i].strip;
                start = i + 1;
            }
        }
    }
    if (start < s.length) {
        parts ~= s[start .. $].strip;
    }
    return parts;
}

bool parseReg8(string token, out int reg) {
    string lower = token.toLower();
    if (!lower.startsWith("r")) return false;
    string num = lower[1 .. $];
    if (num.length == 0) return false;
    foreach (ch; num) {
        if (ch < '0' || ch > '9') return false;
    }
    reg = to!int(num);
    return reg >= 0 && reg <= 15;
}

bool parseReg16(string token, out int reg) {
    string lower = token.toLower();
    if (!lower.startsWith("er")) return false;
    string num = lower[2 .. $];
    if (num.length == 0) return false;
    foreach (ch; num) {
        if (ch < '0' || ch > '9') return false;
    }
    reg = to!int(num);
    return reg >= 0 && reg <= 7;
}

bool parseCharLiteral(string token, out ImmValue imm) {
    if (token.length < 3) return false;
    if (token[0] != '\'' || token[$ - 1] != '\'') return false;
    string inner = token[1 .. $ - 1];
    if (inner.length == 1) {
        imm = ImmValue(false, "", inner[0], 1);
        return true;
    }
    if (inner.length == 2) {
        imm = ImmValue(false, "", (cast(int)inner[0]) | (cast(int)inner[1] << 8), 2);
        return true;
    }
    return false;
}

bool parseNumeric(string token, out long value, out int widthBytes) {
    string s = token.strip;
    if (s.length == 0) return false;
    bool negative = false;
    if (s[0] == '-') {
        negative = true;
        s = s[1 .. $];
    } else if (s[0] == '+') {
        s = s[1 .. $];
    }
    if (s.length == 0) return false;

    int base = 10;
    if (s.startsWith("0x") || s.startsWith("0X")) {
        base = 16;
        s = s[2 .. $];
    } else if (s.startsWith("0b") || s.startsWith("0B")) {
        base = 2;
        s = s[2 .. $];
    } else if (s.length > 1 && (s[$ - 1] == 'h' || s[$ - 1] == 'H')) {
        base = 16;
        s = s[0 .. $ - 1];
    } else {
        bool hasHexAlpha = false;
        foreach (ch; s) {
            if ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
                hasHexAlpha = true;
                break;
            }
        }
        if (hasHexAlpha) {
            base = 16;
        }
    }

    if (s.length == 0) return false;
    long parsed = 0;
    foreach (ch; s) {
        int digit;
        if (ch >= '0' && ch <= '9') digit = ch - '0';
        else if (ch >= 'a' && ch <= 'f') digit = 10 + (ch - 'a');
        else if (ch >= 'A' && ch <= 'F') digit = 10 + (ch - 'A');
        else return false;
        if (digit >= base) return false;
        parsed = parsed * base + digit;
    }
    if (negative) parsed = -parsed;

    value = parsed;
    widthBytes = 1;
    if (parsed < -128 || parsed > 255) {
        widthBytes = 2;
    }
    return true;
}

bool parseImmediate(string token, out ImmValue imm) {
    string s = token.strip;
    if (s.length == 0) return false;
    if (s.startsWith("#")) {
        s = s[1 .. $].strip;
    }

    if (s.startsWith("adr_of(") && s.endsWith(")")) {
        string label = s[7 .. $ - 1].strip;
        if (!isIdentifier(label)) return false;
        imm = ImmValue(true, label, 0, 2);
        return true;
    }

    if (parseCharLiteral(s, imm)) {
        return true;
    }

    long value;
    int widthBytes;
    if (parseNumeric(s, value, widthBytes)) {
        imm = ImmValue(false, "", value, widthBytes);
        return true;
    }

    if (isIdentifier(s)) {
        imm = ImmValue(true, s, 0, 2);
        return true;
    }
    return false;
}

void emitText(ref ubyte[] output, string text) {
    output ~= cast(ubyte[])text;
}

void emitByte(ref ubyte[] output, ubyte value) {
    output ~= value;
}

void emitOpcodeOperand(ref ubyte[] output, ubyte opcode, ubyte operand) {
    emitByte(output, opcode);
    emitByte(output, operand);
}

void emitImm8OrRef(ref ubyte[] output, ImmValue imm) {
    if (imm.isRef) {
        emitText(output, "ref(" ~ imm.label ~ ")");
        return;
    }
    emitByte(output, cast(ubyte)(imm.value & 0xFF));
}

void emitImm16OrRef(ref ubyte[] output, ImmValue imm) {
    if (imm.isRef) {
        emitText(output, "ref(" ~ imm.label ~ ")");
        return;
    }
    ushort v = cast(ushort)(imm.value & 0xFFFF);
    emitByte(output, cast(ubyte)(v & 0xFF));
    emitByte(output, cast(ubyte)((v >> 8) & 0xFF));
}

bool parseMemOperand(string token, out bool isReg, out int reg, out ImmValue imm) {
    string s = token.strip;
    if (!s.startsWith("[") || !s.endsWith("]")) return false;
    string inner = s[1 .. $ - 1].strip;
    if (parseReg16(inner, reg)) {
        isReg = true;
        imm = ImmValue(false, "", 0, 2);
        return true;
    }
    ImmValue temp;
    if (parseImmediate(inner, temp)) {
        isReg = false;
        reg = 0;
        imm = temp;
        return true;
    }
    return false;
}

bool parseSyscallNumber(string token, out ubyte number) {
    string upper = token.toUpper();
    if (auto value = upper in syscallMap) {
        number = cast(ubyte)(*value);
        return true;
    }
    ImmValue imm;
    if (parseImmediate(token, imm) && !imm.isRef) {
        number = cast(ubyte)(imm.value & 0xFF);
        return true;
    }
    return false;
}

void emitDefLabel(ref ubyte[] output, string label) {
    emitText(output, "def(" ~ label ~ ")");
}

void assembleInstruction(string mnemonicRaw, string[] operands, ref BuildResult result, ref ubyte[] output, string filePath, size_t lineNumber, ref AsmStats stats) {
    string mnemonic = mnemonicRaw.toLower();

    auto error = (string message) {
        result.errorCode = BuildError.ASSEMBLY_ERROR;
        result.errorMessage = message ~ " at " ~ filePath ~ ":" ~ lineNumber.to!string;
    };

    auto requireOperands = (size_t count) {
        if (operands.length != count) {
            error("Invalid operand count for " ~ mnemonic);
            return false;
        }
        return true;
    };

    auto bumpStack = (long delta) {
        stats.stackDepth += delta;
        if (stats.stackDepth < 0) stats.stackDepth = 0;
        if (stats.stackDepth > stats.maxStackDepth) stats.maxStackDepth = stats.stackDepth;
    };

    int regA = 0;
    int regB = 0;
    ImmValue imm;
    bool memIsReg = false;
    int memReg = 0;
    ImmValue memImm;

    if (mnemonic == "mov") {
        if (!requireOperands(2)) return;
        if (parseReg8(operands[0], regA) && parseReg8(operands[1], regB)) {
            emitOpcodeOperand(output, OP_MOV8_REG_REG, cast(ubyte)((regA << 4) | regB));
            return;
        }
        if (parseReg16(operands[0], regA) && parseReg16(operands[1], regB)) {
            emitOpcodeOperand(output, OP_MOV16_REG_REG, cast(ubyte)((regA << 4) | regB));
            return;
        }
        if (parseReg16(operands[0], regA) && parseImmediate(operands[1], imm)) {
            emitOpcodeOperand(output, OP_MOV16_REG_IMM, cast(ubyte)(regA << 4));
            emitImm16OrRef(output, imm);
            return;
        }
        if (parseReg8(operands[0], regA) && parseImmediate(operands[1], imm)) {
            emitOpcodeOperand(output, cast(ubyte)(OP_MOV8_REG_IMM + regA), cast(ubyte)(imm.value & 0xFF));
            return;
        }
        error("Invalid operands for mov");
        return;
    }

    if (mnemonic == "l") {
        if (!requireOperands(2)) return;
        if (parseReg8(operands[0], regA) && parseMemOperand(operands[1], memIsReg, memReg, memImm)) {
            if (memIsReg) {
                emitOpcodeOperand(output, OP_LOAD8_REG_MREG, cast(ubyte)((regA << 4) | memReg));
            } else {
                emitOpcodeOperand(output, OP_LOAD8_REG_MIMM, cast(ubyte)(regA << 4));
                emitImm16OrRef(output, memImm);
                if (!memImm.isRef && memImm.value >= 0) {
                    long endAddr = memImm.value;
                    if (!stats.hasStoreAddr || endAddr > stats.maxStoreAddr) {
                        stats.maxStoreAddr = endAddr;
                        stats.hasStoreAddr = true;
                    }
                }
            }
            return;
        }
        if (parseReg16(operands[0], regA) && parseMemOperand(operands[1], memIsReg, memReg, memImm)) {
            if (memIsReg) {
                emitOpcodeOperand(output, OP_LOAD16_REG_MREG, cast(ubyte)((regA << 4) | memReg));
            } else {
                emitOpcodeOperand(output, OP_LOAD16_REG_MIMM, cast(ubyte)(regA << 4));
                emitImm16OrRef(output, memImm);
                if (!memImm.isRef && memImm.value >= 0) {
                    long endAddr = memImm.value + 1;
                    if (!stats.hasStoreAddr || endAddr > stats.maxStoreAddr) {
                        stats.maxStoreAddr = endAddr;
                        stats.hasStoreAddr = true;
                    }
                }
            }
            return;
        }
        error("Invalid operands for l");
        return;
    }

    if (mnemonic == "st") {
        if (!requireOperands(2)) return;
        if (parseMemOperand(operands[0], memIsReg, memReg, memImm) && parseReg8(operands[1], regA)) {
            if (memIsReg) {
                emitOpcodeOperand(output, OP_STORE8_MREG_REG, cast(ubyte)((memReg << 4) | regA));
            } else {
                emitOpcodeOperand(output, OP_STORE8_MIMM_REG, cast(ubyte)(regA & 0x0F));
                emitImm16OrRef(output, memImm);
                if (!memImm.isRef && memImm.value >= 0) {
                    long endAddr = memImm.value;
                    if (!stats.hasStoreAddr || endAddr > stats.maxStoreAddr) {
                        stats.maxStoreAddr = endAddr;
                        stats.hasStoreAddr = true;
                    }
                }
            }
            return;
        }
        if (parseMemOperand(operands[0], memIsReg, memReg, memImm) && parseReg16(operands[1], regA)) {
            if (memIsReg) {
                emitOpcodeOperand(output, OP_STORE16_MREG_REG, cast(ubyte)((memReg << 4) | regA));
            } else {
                emitOpcodeOperand(output, OP_STORE16_MIMM_REG, cast(ubyte)(regA & 0x0F));
                emitImm16OrRef(output, memImm);
                if (!memImm.isRef && memImm.value >= 0) {
                    long endAddr = memImm.value + 1;
                    if (!stats.hasStoreAddr || endAddr > stats.maxStoreAddr) {
                        stats.maxStoreAddr = endAddr;
                        stats.hasStoreAddr = true;
                    }
                }
            }
            return;
        }
        if (parseMemOperand(operands[0], memIsReg, memReg, memImm) && parseImmediate(operands[1], imm)) {
            bool isWord = imm.isRef || imm.widthBytes > 1;
            if (memIsReg) {
                if (isWord) {
                    emitOpcodeOperand(output, OP_STORE16_MREG_IMM, cast(ubyte)(memReg << 4));
                    emitImm16OrRef(output, imm);
                } else {
                    emitOpcodeOperand(output, OP_STORE8_MREG_IMM, cast(ubyte)(memReg << 4));
                    emitImm16OrRef(output, imm);
                }
                return;
            }
            if (isWord) {
                emitOpcodeOperand(output, OP_STORE16_MIMM_IMM, 0);
                emitImm16OrRef(output, memImm);
                emitImm16OrRef(output, imm);
                if (!memImm.isRef && memImm.value >= 0) {
                    long endAddr = memImm.value + 1;
                    if (!stats.hasStoreAddr || endAddr > stats.maxStoreAddr) {
                        stats.maxStoreAddr = endAddr;
                        stats.hasStoreAddr = true;
                    }
                }
            } else {
                emitOpcodeOperand(output, OP_STORE8_MIMM_IMM, cast(ubyte)(imm.value & 0xFF));
                emitImm16OrRef(output, memImm);
                if (!memImm.isRef && memImm.value >= 0) {
                    long endAddr = memImm.value;
                    if (!stats.hasStoreAddr || endAddr > stats.maxStoreAddr) {
                        stats.maxStoreAddr = endAddr;
                        stats.hasStoreAddr = true;
                    }
                }
            }
            return;
        }
        error("Invalid operands for st");
        return;
    }

    if (mnemonic == "push") {
        if (!requireOperands(1)) return;
        if (operands[0].toLower() == "lr") {
            emitOpcodeOperand(output, OP_PUSH_LR, 0);
            bumpStack(2);
            return;
        }
        if (operands[0].toLower() == "sp") {
            emitOpcodeOperand(output, OP_PUSH_SP, 0);
            bumpStack(2);
            return;
        }
        if (parseReg8(operands[0], regA)) {
            emitOpcodeOperand(output, OP_PUSH8_REG, cast(ubyte)(regA << 4));
            bumpStack(1);
            return;
        }
        if (parseReg16(operands[0], regA)) {
            emitOpcodeOperand(output, OP_PUSH16_REG, cast(ubyte)(regA << 4));
            bumpStack(2);
            return;
        }
        if (parseImmediate(operands[0], imm)) {
            if (imm.widthBytes <= 1) {
                emitOpcodeOperand(output, OP_PUSH8_IMM, cast(ubyte)(imm.value & 0xFF));
                bumpStack(1);
            } else {
                emitOpcodeOperand(output, OP_PUSH16_IMM, 0);
                emitImm16OrRef(output, imm);
                bumpStack(2);
            }
            return;
        }
        error("Invalid operands for push");
        return;
    }

    if (mnemonic == "pop") {
        if (!requireOperands(1)) return;
        if (operands[0].toLower() == "pc") {
            emitOpcodeOperand(output, OP_POP_PC, 0);
            bumpStack(-2);
            return;
        }
        if (operands[0].toLower() == "sp") {
            emitOpcodeOperand(output, OP_POP_SP, 0);
            bumpStack(-2);
            return;
        }
        if (parseReg8(operands[0], regA)) {
            emitOpcodeOperand(output, OP_POP8_REG, cast(ubyte)(regA << 4));
            bumpStack(-1);
            return;
        }
        if (parseReg16(operands[0], regA)) {
            emitOpcodeOperand(output, OP_POP16_REG, cast(ubyte)(regA << 4));
            bumpStack(-2);
            return;
        }
        error("Invalid operands for pop");
        return;
    }

    if (mnemonic == "add" || mnemonic == "sub" || mnemonic == "mul" || mnemonic == "div" || mnemonic == "mod" ||
        mnemonic == "and" || mnemonic == "or" || mnemonic == "xor" || mnemonic == "cmp") {
        if (!requireOperands(2)) return;
        bool isCmp = mnemonic == "cmp";
        if (parseReg8(operands[0], regA) && parseReg8(operands[1], regB)) {
            ubyte opcode;
            switch (mnemonic) {
                case "add": opcode = OP_ADD8_REG_REG; break;
                case "sub": opcode = OP_SUB8_REG_REG; break;
                case "mul": opcode = OP_MUL8_REG_REG; break;
                case "div": opcode = OP_DIV8_REG_REG; break;
                case "mod": opcode = OP_MOD8_REG_REG; break;
                case "and": opcode = OP_AND8_REG_REG; break;
                case "or": opcode = OP_OR8_REG_REG; break;
                case "xor": opcode = OP_XOR8_REG_REG; break;
                case "cmp": opcode = OP_CMP8_REG_REG; break;
                default: opcode = 0; break;
            }
            emitOpcodeOperand(output, opcode, cast(ubyte)((regA << 4) | regB));
            return;
        }
        if (parseReg16(operands[0], regA) && parseReg16(operands[1], regB)) {
            ubyte opcode;
            switch (mnemonic) {
                case "add": opcode = OP_ADD16_REG_REG; break;
                case "sub": opcode = OP_SUB16_REG_REG; break;
                case "mul": opcode = OP_MUL16_REG_REG; break;
                case "div": opcode = OP_DIV16_REG_REG; break;
                case "mod": opcode = OP_MOD16_REG_REG; break;
                case "and": opcode = OP_AND16_REG_REG; break;
                case "or": opcode = OP_OR16_REG_REG; break;
                case "xor": opcode = OP_XOR16_REG_REG; break;
                case "cmp": opcode = OP_CMP16_REG_REG; break;
                default: opcode = 0; break;
            }
            emitOpcodeOperand(output, opcode, cast(ubyte)((regA << 4) | regB));
            return;
        }
        if (parseReg16(operands[0], regA) && parseImmediate(operands[1], imm)) {
            ubyte opcode;
            switch (mnemonic) {
                case "add": opcode = OP_ADD16_REG_IMM; break;
                case "sub": opcode = OP_SUB16_REG_IMM; break;
                case "mul": opcode = OP_MUL16_REG_IMM; break;
                case "div": opcode = OP_DIV16_REG_IMM; break;
                case "mod": opcode = OP_MOD16_REG_IMM; break;
                case "and": opcode = OP_AND16_REG_IMM; break;
                case "or": opcode = OP_OR16_REG_IMM; break;
                case "xor": opcode = OP_XOR16_REG_IMM; break;
                case "cmp": opcode = OP_CMP16_REG_IMM; break;
                default: opcode = 0; break;
            }
            emitOpcodeOperand(output, opcode, cast(ubyte)(regA << 4));
            emitImm16OrRef(output, imm);
            return;
        }
        if (parseReg8(operands[0], regA) && parseImmediate(operands[1], imm)) {
            if (mnemonic == "mod") {
                emitOpcodeOperand(output, OP_MOD8_REG_IMM, cast(ubyte)(regA << 4));
                emitImm16OrRef(output, imm);
                return;
            }
            ubyte baseOpcode;
            switch (mnemonic) {
                case "add": baseOpcode = OP_ADD8_REG_IMM; break;
                case "sub": baseOpcode = OP_SUB8_REG_IMM; break;
                case "mul": baseOpcode = OP_MUL8_REG_IMM; break;
                case "div": baseOpcode = OP_DIV8_REG_IMM; break;
                case "and": baseOpcode = OP_AND8_REG_IMM; break;
                case "or": baseOpcode = OP_OR8_REG_IMM; break;
                case "xor": baseOpcode = OP_XOR8_REG_IMM; break;
                case "cmp": baseOpcode = OP_CMP8_REG_IMM; break;
                default: baseOpcode = 0; break;
            }
            emitOpcodeOperand(output, cast(ubyte)(baseOpcode + regA), cast(ubyte)(imm.value & 0xFF));
            return;
        }
        error("Invalid operands for " ~ mnemonic);
        return;
    }

    if (mnemonic == "inc" || mnemonic == "dec" || mnemonic == "not") {
        if (!requireOperands(1)) return;
        if (parseReg8(operands[0], regA)) {
            ubyte opcode = mnemonic == "inc" ? OP_INC8_REG : (mnemonic == "dec" ? OP_DEC8_REG : OP_NOT8_REG);
            emitOpcodeOperand(output, opcode, cast(ubyte)(regA << 4));
            return;
        }
        if (parseReg16(operands[0], regA)) {
            ubyte opcode = mnemonic == "inc" ? OP_INC16_REG : (mnemonic == "dec" ? OP_DEC16_REG : OP_NOT16_REG);
            emitOpcodeOperand(output, opcode, cast(ubyte)(regA << 4));
            return;
        }
        error("Invalid operands for " ~ mnemonic);
        return;
    }

    if (mnemonic == "sll" || mnemonic == "srl" || mnemonic == "sra") {
        if (!requireOperands(2)) return;
        if (parseReg8(operands[0], regA) && parseReg8(operands[1], regB)) {
            ubyte opcode = mnemonic == "sll" ? OP_SLL8_REG_REG : (mnemonic == "srl" ? OP_SRL8_REG_REG : OP_SRA8_REG_REG);
            emitOpcodeOperand(output, opcode, cast(ubyte)((regA << 4) | regB));
            return;
        }
        if (parseReg16(operands[0], regA) && parseReg16(operands[1], regB)) {
            ubyte opcode = mnemonic == "sll" ? OP_SLL16_REG_REG : (mnemonic == "srl" ? OP_SRL16_REG_REG : OP_SRA16_REG_REG);
            emitOpcodeOperand(output, opcode, cast(ubyte)((regA << 4) | regB));
            return;
        }
        if (parseReg8(operands[0], regA) && parseImmediate(operands[1], imm)) {
            ubyte opcode = mnemonic == "sll" ? OP_SLL8_REG_IMM : (mnemonic == "srl" ? OP_SRL8_REG_IMM : OP_SRA8_REG_IMM);
            ubyte shift = cast(ubyte)(imm.value & 0x0F);
            emitOpcodeOperand(output, opcode, cast(ubyte)((regA << 4) | shift));
            return;
        }
        if (parseReg16(operands[0], regA) && parseImmediate(operands[1], imm)) {
            ubyte opcode = mnemonic == "sll" ? OP_SLL16_REG_IMM : (mnemonic == "srl" ? OP_SRL16_REG_IMM : OP_SRA16_REG_IMM);
            ubyte shift = cast(ubyte)(imm.value & 0x0F);
            emitOpcodeOperand(output, opcode, cast(ubyte)((regA << 4) | shift));
            return;
        }
        error("Invalid operands for " ~ mnemonic);
        return;
    }

    if (mnemonic == "b") {
        if (!requireOperands(1)) return;
        if (parseReg16(operands[0], regA)) {
            emitOpcodeOperand(output, OP_B_REG, cast(ubyte)(regA << 4));
            return;
        }
        if (parseImmediate(operands[0], imm)) {
            emitOpcodeOperand(output, OP_B_IMM, 0);
            emitImm16OrRef(output, imm);
            return;
        }
        error("Invalid operands for b");
        return;
    }

    if (mnemonic == "beq" || mnemonic == "bne" || mnemonic == "blt" || mnemonic == "ble" || mnemonic == "bgt" || mnemonic == "bge") {
        if (!requireOperands(1)) return;
        if (parseImmediate(operands[0], imm)) {
            ubyte opcode = OP_BEQ_IMM;
            switch (mnemonic) {
                case "beq": opcode = OP_BEQ_IMM; break;
                case "bne": opcode = OP_BNE_IMM; break;
                case "blt": opcode = OP_BLT_IMM; break;
                case "ble": opcode = OP_BLE_IMM; break;
                case "bgt": opcode = OP_BGT_IMM; break;
                case "bge": opcode = OP_BGE_IMM; break;
                default: break;
            }
            if (imm.isRef) {
                emitByte(output, opcode);
                emitText(output, "ref(" ~ imm.label ~ ")");
            } else {
                emitOpcodeOperand(output, opcode, cast(ubyte)(imm.value & 0xFF));
            }
            return;
        }
        error("Invalid operands for " ~ mnemonic);
        return;
    }

    if (mnemonic == "bl") {
        if (!requireOperands(1)) return;
        if (parseReg16(operands[0], regA)) {
            emitOpcodeOperand(output, OP_BL_REG, cast(ubyte)(regA << 4));
            return;
        }
        if (parseImmediate(operands[0], imm)) {
            emitOpcodeOperand(output, OP_BL_IMM, 0);
            emitImm16OrRef(output, imm);
            return;
        }
        error("Invalid operands for bl");
        return;
    }

    if (mnemonic == "rt") {
        if (!requireOperands(0)) return;
        emitOpcodeOperand(output, OP_RT, 0);
        return;
    }

    if (mnemonic == "syscall") {
        if (!requireOperands(1)) return;
        ubyte num;
        if (!parseSyscallNumber(operands[0], num)) {
            error("Invalid syscall number");
            return;
        }
        emitOpcodeOperand(output, OP_SYSCALL, num);
        return;
    }

    if (mnemonic == "end") {
        if (!requireOperands(0)) return;
        emitOpcodeOperand(output, OP_END, 0);
        return;
    }

    error("Unknown mnemonic " ~ mnemonic);
}

// Assemble the input file
BuildResult assemble(string inputFile, string outputFile) {
    BuildResult result;
    result.errorCode = BuildError.SUCCESS;
    
    // Validate input file exists
    string validationError;
    if (!validateInputFile(inputFile, validationError)) {
        result.errorCode = BuildError.FILE_NOT_FOUND;
        result.errorMessage = validationError;
        return result;
    }
    
    // Validate file extension
    if (!inputFile.endsWith(".tasm")) {
        result.warnings ~= "Warning: Input file does not have .tasm extension";
    }

    writeln("Assembling ", inputFile, "...");

    string[string] defines;
    SourceLine[] lines;
    preprocessFile(inputFile, defines, lines, result);
    if (!result.success()) return result;
    if (lines.length == 0) {
        result.errorCode = BuildError.INVALID_INPUT;
        result.errorMessage = "Input file is empty";
        return result;
    }

    ubyte[] output;
    emitText(output, "TASM");
    AsmStats stats;

    foreach (line; lines) {
        string text = line.text.strip;
        if (text.length == 0) continue;

        while (true) {
            size_t colonIndex = text.indexOf(":");
            if (colonIndex != size_t.max) {
                string label = text[0 .. colonIndex].strip;
                if (label.length > 0) {
                    if (!isIdentifier(label)) {
                        result.errorCode = BuildError.PARSE_ERROR;
                        result.errorMessage = "Invalid label name at " ~ line.filePath ~ ":" ~ line.lineNumber.to!string;
                        return result;
                    }
                    emitDefLabel(output, label);
                }
                text = text[colonIndex + 1 .. $].strip;
                if (text.length == 0) break;
                continue;
            }
            break;
        }
        if (text.length == 0) continue;

        size_t firstSpace = text.indexOfAny(" \t");
        string mnemonic;
        string operandText;
        if (firstSpace == size_t.max) {
            mnemonic = text;
            operandText = "";
        } else {
            mnemonic = text[0 .. firstSpace];
            operandText = text[firstSpace .. $].strip;
        }
        string[] operands = operandText.length == 0 ? [] : splitOperands(operandText);

        assembleInstruction(mnemonic, operands, result, output, line.filePath, line.lineNumber, stats);
        if (!result.success()) return result;
    }

    long estimate = stats.maxStackDepth;
    if (stats.hasStoreAddr) {
        long storeSize = stats.maxStoreAddr + 1;
        if (storeSize > estimate) estimate = storeSize;
    }
    if (estimate < 0) estimate = 0;
    if ((output.length & 1) == 1) {
        output ~= cast(ubyte)0x00;
    }
    output ~= cast(ubyte)(estimate & 0xFF);
    output ~= cast(ubyte)((estimate >> 8) & 0xFF);

    try {
        write(outputFile, output);
    } catch (FileException e) {
        result.errorCode = BuildError.OUTPUT_ERROR;
        result.errorMessage = "Failed to write output file: " ~ e.msg;
        return result;
    }

    writeln("Assembly completed successfully.");
    return result;
}

void main(string[] args) {
    string inputFile;
    string outputFile;
    bool verbose = false;

    // Parse arguments
    if (args.length < 2) {
        writeln("Usage: tas-t16 <file.tasm> [-o <file.to>] [-v]");
        writeln("Options:");
        writeln("  -o <file>  Specify output file");
        writeln("  -v         Verbose output");
        exit(BuildError.INVALID_INPUT);
    }

    size_t i = 1;
    while (i < args.length) {
        string arg = args[i];
        if (arg == "-o") {
            if (i + 1 < args.length) {
                outputFile = args[i + 1];
                i += 2;
            } else {
                writeln("Error: -o requires an output file");
                exit(BuildError.INVALID_INPUT);
            }
        } else if (arg == "-v") {
            verbose = true;
            i++;
        } else if (inputFile.length == 0) {
            inputFile = arg;
            i++;
        } else {
            writeln("Unknown argument: ", arg);
            exit(BuildError.INVALID_INPUT);
        }
    }

    if (inputFile.length == 0) {
        writeln("Error: no input file specified");
        exit(BuildError.INVALID_INPUT);
    }

    if (outputFile.length == 0) {
        // Default output file
        outputFile = replace(inputFile, ".tasm", ".to");
    }

    writeln("Tangent Assembler Framework");
    if (verbose) {
        writeln("Input file: ", inputFile);
        writeln("Output file: ", outputFile);
    }

    // Perform the assembly
    BuildResult result = assemble(inputFile, outputFile);
    
    // Output any warnings
    foreach (warning; result.warnings) {
        writeln(warning);
    }
    
    // Handle build errors
    if (!result.success()) {
        writeln("ERROR: ", result.errorMessage);
        writeln("Build failed with error code: ", cast(int)result.errorCode);
        exit(result.errorCode);
    }
    
    writeln("Build successful!");
    exit(BuildError.SUCCESS);
}
