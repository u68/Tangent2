import std.stdio;
import std.array;
import std.string;
import std.file : exists, isFile, read, write, FileException;
import core.stdc.stdlib : exit;

// Build error codes
enum LinkError {
    SUCCESS = 0,
    FILE_NOT_FOUND = 1,
    INVALID_INPUT = 2,
    SYMBOL_ERROR = 3,
    LINK_ERROR = 4,
    OUTPUT_ERROR = 5,
    UNKNOWN_ERROR = 99
}

struct LinkResult {
    LinkError errorCode;
    string errorMessage;
    string[] warnings;
    string[] missingFiles;
    bool success() const { return errorCode == LinkError.SUCCESS; }
}

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

// Validate all input files exist
LinkResult validateAllInputFiles(string[] inputFiles) {
    LinkResult result;
    result.errorCode = LinkError.SUCCESS;
    
    foreach (inputFile; inputFiles) {
        string validationError;
        if (!validateInputFile(inputFile, validationError)) {
            result.missingFiles ~= inputFile;
        }
        
        // Check for correct extension
        if (!inputFile.endsWith(".to")) {
            result.warnings ~= "Warning: " ~ inputFile ~ " does not have .to extension";
        }
    }
    
    if (result.missingFiles.length > 0) {
        result.errorCode = LinkError.FILE_NOT_FOUND;
        result.errorMessage = "One or more input files not found";
    }
    
    return result;
}

bool startsWithAscii(const(ubyte)[] data, size_t offset, string token) {
    if (offset + token.length > data.length) {
        return false;
    }
    foreach (i, ch; token) {
        if (data[offset + i] != cast(ubyte)ch) {
            return false;
        }
    }
    return true;
}

bool parseLabel(const(ubyte)[] data, size_t startIndex, out string label, out size_t nextIndex, ref string errorMsg) {
    size_t i = startIndex;
    while (i < data.length && data[i] != cast(ubyte)')') {
        i++;
    }
    if (i >= data.length) {
        errorMsg = "Unterminated label token";
        return false;
    }
    if (i == startIndex) {
        errorMsg = "Empty label name";
        return false;
    }
    label = cast(string)data[startIndex .. i];
    nextIndex = i + 1;
    return true;
}

bool stripHeader(const(ubyte)[] data, out ubyte[] payload, ref string errorMsg) {
    if (data.length < 3) {
        errorMsg = "Input file too small to contain header";
        return false;
    }
    size_t headerLen = 0;
    if (startsWithAscii(data, 0, "TASM")) {
        headerLen = 4;
    } else if (startsWithAscii(data, 0, "TML")) {
        headerLen = 3;
    } else {
        errorMsg = "Missing TASM/TML header";
        return false;
    }
    if (data.length < headerLen) {
        errorMsg = "Invalid header length";
        return false;
    }
    payload = data[headerLen .. $].dup;
    return true;
}

void appendLe16(ref ubyte[] buffer, uint value) {
    buffer ~= cast(ubyte)(value & 0xFF);
    buffer ~= cast(ubyte)((value >> 8) & 0xFF);
}

// Link object files together into a Tangent Program (.tp) file
LinkResult link(string[] inputFiles, string outputFile) {
    LinkResult result;
    result.errorCode = LinkError.SUCCESS;
    
    // First validate all input files exist
    result = validateAllInputFiles(inputFiles);
    if (!result.success()) {
        return result;
    }
    
    writeln("Linking ", inputFiles.length, " object file(s)...");

    ubyte[] combined;

    // Try to read all input files
    foreach (inputFile; inputFiles) {
        try {
            auto data = read(inputFile);
            if (data.length == 0) {
                result.warnings ~= "Warning: " ~ inputFile ~ " is empty";
                continue;
            }
            ubyte[] payload;
            string headerError;
            if (!stripHeader(cast(ubyte[])data, payload, headerError)) {
                result.errorCode = LinkError.INVALID_INPUT;
                result.errorMessage = "Invalid input " ~ inputFile ~ ": " ~ headerError;
                return result;
            }
            combined ~= payload;
        } catch (FileException e) {
            result.errorCode = LinkError.FILE_NOT_FOUND;
            result.errorMessage = "Failed to read " ~ inputFile ~ ": " ~ e.msg;
            return result;
        }
    }

    if (combined.length < 2) {
        result.errorCode = LinkError.LINK_ERROR;
        result.errorMessage = "Linked output too small to contain RAM size";
        return result;
    }

    // First pass: build symbol table and compute addresses
    uint[string] symbols;
    size_t i = 0;
    uint logicalIndex = 0;

    while (i < combined.length) {
        if (startsWithAscii(combined, i, "def(")) {
            string label;
            size_t nextIndex;
            string parseError;
            if (!parseLabel(combined, i + 4, label, nextIndex, parseError)) {
                result.errorCode = LinkError.INVALID_INPUT;
                result.errorMessage = "Parse error in def(): " ~ parseError;
                return result;
            }
            if (label in symbols) {
                result.errorCode = LinkError.SYMBOL_ERROR;
                result.errorMessage = "Duplicate label definition: " ~ label;
                return result;
            }
            uint address = logicalIndex + 4;
            if (address > 0xFFFF) {
                result.errorCode = LinkError.SYMBOL_ERROR;
                result.errorMessage = "Label address exceeds 16-bit limit: " ~ label;
                return result;
            }
            symbols[label] = address;
            i = nextIndex;
            continue;
        }

        if (startsWithAscii(combined, i, "ref(")) {
            string label;
            size_t nextIndex;
            string parseError;
            if (!parseLabel(combined, i + 4, label, nextIndex, parseError)) {
                result.errorCode = LinkError.INVALID_INPUT;
                result.errorMessage = "Parse error in ref(): " ~ parseError;
                return result;
            }
            logicalIndex += 2;
            i = nextIndex;
            continue;
        }

        logicalIndex++;
        i++;
    }

    // Second pass: emit output with resolved references
    ubyte[] resolved;
    i = 0;
    while (i < combined.length) {
        if (startsWithAscii(combined, i, "def(")) {
            string label;
            size_t nextIndex;
            string parseError;
            if (!parseLabel(combined, i + 4, label, nextIndex, parseError)) {
                result.errorCode = LinkError.INVALID_INPUT;
                result.errorMessage = "Parse error in def(): " ~ parseError;
                return result;
            }
            i = nextIndex;
            continue;
        }

        if (startsWithAscii(combined, i, "ref(")) {
            string label;
            size_t nextIndex;
            string parseError;
            if (!parseLabel(combined, i + 4, label, nextIndex, parseError)) {
                result.errorCode = LinkError.INVALID_INPUT;
                result.errorMessage = "Parse error in ref(): " ~ parseError;
                return result;
            }
            if (auto addrPtr = label in symbols) {
                uint address = *addrPtr;
                if (address > 0xFFFF) {
                    result.errorCode = LinkError.SYMBOL_ERROR;
                    result.errorMessage = "Label address exceeds 16-bit limit: " ~ label;
                    return result;
                }
                appendLe16(resolved, address);
            } else {
                result.errorCode = LinkError.SYMBOL_ERROR;
                result.errorMessage = "Undefined label reference: " ~ label;
                return result;
            }
            i = nextIndex;
            continue;
        }

        resolved ~= combined[i];
        i++;
    }

    if (resolved.length < 2) {
        result.errorCode = LinkError.LINK_ERROR;
        result.errorMessage = "Linked output too small to contain RAM size";
        return result;
    }

    uint codeSize = cast(uint)(resolved.length - 2);
    if (codeSize > 0xFFFF) {
        result.errorCode = LinkError.LINK_ERROR;
        result.errorMessage = "Code size exceeds 16-bit limit";
        return result;
    }

    ubyte[] finalOutput;
    appendLe16(finalOutput, codeSize);
    finalOutput ~= resolved[$ - 2 .. $];
    finalOutput ~= resolved[0 .. $ - 2];

    writeln("Generating output: ", outputFile);
    try {
        write(outputFile, finalOutput);
    } catch (FileException e) {
        result.errorCode = LinkError.OUTPUT_ERROR;
        result.errorMessage = "Failed to write output: " ~ e.msg;
        return result;
    }

    writeln("Linking completed successfully.");
    return result;
}

void main(string[] args) {
    bool verbose = false;
    string outputFile;
    string[] inputFiles;

    if (args.length < 2) {
        writeln("Usage: tl-t16 [options] <file1.to> ...");
        writeln("Options:");
        writeln("  -o <file>      Specify output file (full name with extension, e.g., program.tp)");
        writeln("  -v             Verbose output");
        exit(LinkError.INVALID_INPUT);
    }

    size_t i = 1;
    while (i < args.length) {
        string arg = args[i];
        if (arg == "-o") {
            if (i + 1 < args.length) {
                outputFile = args[i + 1];
                i++;
            } else {
                writeln("Error: -o requires an output file");
                exit(LinkError.INVALID_INPUT);
            }
        } else if (arg == "-v") {
            verbose = true;
        } else if (arg.startsWith("-")) {
            writeln("Unknown flag: ", arg);
            exit(LinkError.INVALID_INPUT);
        } else {
            inputFiles ~= arg;
        }
        i++;
    }

    if (inputFiles.length == 0) {
        writeln("Error: no input files specified");
        exit(LinkError.INVALID_INPUT);
    }

    if (outputFile.length == 0) {
        outputFile = "out.tp"; // default output
    }

    writeln("Tangent Linker Framework");
    if (verbose) {
        writeln("Input files: ", inputFiles);
        writeln("Output file: ", outputFile);
    }

    // Perform the linking
    LinkResult result = link(inputFiles, outputFile);
    
    // Output any warnings
    foreach (warning; result.warnings) {
        writeln(warning);
    }
    
    // Report missing files
    if (result.missingFiles.length > 0) {
        writeln("Missing files:");
        foreach (missingFile; result.missingFiles) {
            writeln("  - ", missingFile);
        }
    }
    
    // Handle build errors
    if (!result.success()) {
        writeln("ERROR: ", result.errorMessage);
        writeln("Link failed with error code: ", cast(int)result.errorCode);
        exit(result.errorCode);
    }
    
    writeln("Link successful!");
    exit(LinkError.SUCCESS);
}
