import std.stdio;
import std.array;
import std.string;
import std.file : exists, isFile, read, FileException;
import core.stdc.stdlib : exit;

// Build error codes
enum CompileError {
    SUCCESS = 0,
    FILE_NOT_FOUND = 1,
    INVALID_INPUT = 2,
    SYNTAX_ERROR = 3,
    SEMANTIC_ERROR = 4,
    CODEGEN_ERROR = 5,
    OUTPUT_ERROR = 6,
    UNKNOWN_ERROR = 99
}

struct CompileResult {
    CompileError errorCode;
    string errorMessage;
    string[] warnings;
    int errorLine;
    int errorColumn;
    bool success() const { return errorCode == CompileError.SUCCESS; }
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

// Compile the TML source file
CompileResult compile(string inputFile, string outputFile) {
    CompileResult result;
    result.errorCode = CompileError.SUCCESS;
    result.errorLine = -1;
    result.errorColumn = -1;
    
    // Validate input file exists
    string validationError;
    if (!validateInputFile(inputFile, validationError)) {
        result.errorCode = CompileError.FILE_NOT_FOUND;
        result.errorMessage = validationError;
        return result;
    }
    
    // Try to read the input file
    string sourceCode;
    try {
        sourceCode = cast(string) read(inputFile);
    } catch (FileException e) {
        result.errorCode = CompileError.FILE_NOT_FOUND;
        result.errorMessage = "Failed to read input file: " ~ e.msg;
        return result;
    }
    
    // Validate file extension
    if (!inputFile.endsWith(".tml")) {
        result.warnings ~= "Warning: Input file does not have .tml extension";
    }
    
    writeln("Compiling ", inputFile, "...");
    
    // Check for empty input
    if (sourceCode.length == 0) {
        result.errorCode = CompileError.INVALID_INPUT;
        result.errorMessage = "Input file is empty";
        return result;
    }
    
    // TODO: Implement actual compilation stages:
    // 1. Lexical analysis (tokenization)
    // 2. Syntax analysis (parsing)
    // 3. Semantic analysis (type checking, etc.)
    // 4. Code generation
    
    // Example error reporting (placeholder for real implementation)
    // if (syntaxError) {
    //     result.errorCode = CompileError.SYNTAX_ERROR;
    //     result.errorMessage = "Unexpected token";
    //     result.errorLine = 10;
    //     result.errorColumn = 5;
    //     return result;
    // }
    
    // TODO: Write compiled output to outputFile
    
    writeln("Compilation completed successfully.");
    return result;
}

void main(string[] args) {
    string inputFile;
    string outputFile;
    bool verbose = false;
    bool emitWarnings = true;

    // Parse arguments
    if (args.length < 2) {
        writeln("Usage: tmlc-t16 <file.tml> [-o <file.to>] [-v] [-w]");
        writeln("Options:");
        writeln("  -o <file>  Specify output file");
        writeln("  -v         Verbose output");
        writeln("  -w         Suppress warnings");
        exit(CompileError.INVALID_INPUT);
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
                exit(CompileError.INVALID_INPUT);
            }
        } else if (arg == "-v") {
            verbose = true;
            i++;
        } else if (arg == "-w") {
            emitWarnings = false;
            i++;
        } else if (inputFile.length == 0) {
            inputFile = arg;
            i++;
        } else {
            writeln("Unknown argument: ", arg);
            exit(CompileError.INVALID_INPUT);
        }
    }

    if (inputFile.length == 0) {
        writeln("Error: no input file specified");
        exit(CompileError.INVALID_INPUT);
    }

    if (outputFile.length == 0) {
        // Default output file
        outputFile = replace(inputFile, ".tml", ".to");
    }

    writeln("Tangent ML Compiler Framework");
    if (verbose) {
        writeln("Input file: ", inputFile);
        writeln("Output file: ", outputFile);
    }

    // Perform the compilation
    CompileResult result = compile(inputFile, outputFile);
    
    // Output any warnings
    if (emitWarnings) {
        foreach (warning; result.warnings) {
            writeln(warning);
        }
    }
    
    // Handle compile errors with location info
    if (!result.success()) {
        if (result.errorLine >= 0) {
            writef("%s:%d:%d: error: %s\n", 
                   inputFile, result.errorLine, result.errorColumn, result.errorMessage);
        } else {
            writeln("ERROR: ", result.errorMessage);
        }
        writeln("Compilation failed with error code: ", cast(int)result.errorCode);
        exit(result.errorCode);
    }
    
    writeln("Compilation successful!");
    exit(CompileError.SUCCESS);
}
