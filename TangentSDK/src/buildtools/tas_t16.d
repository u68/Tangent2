import std.stdio;
import std.array;
import std.string;
import std.file : exists, isFile, read, FileException;
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
    
    // Try to read the input file
    string sourceCode;
    try {
        sourceCode = cast(string) read(inputFile);
    } catch (FileException e) {
        result.errorCode = BuildError.FILE_NOT_FOUND;
        result.errorMessage = "Failed to read input file: " ~ e.msg;
        return result;
    }
    
    // Validate file extension
    if (!inputFile.endsWith(".tasm")) {
        result.warnings ~= "Warning: Input file does not have .tasm extension";
    }
    
    // TODO: Implement actual assembly logic here
    // For now, we simulate a successful assembly
    
    writeln("Assembling ", inputFile, "...");
    
    // Example: Check for basic syntax (placeholder for real implementation)
    if (sourceCode.length == 0) {
        result.errorCode = BuildError.INVALID_INPUT;
        result.errorMessage = "Input file is empty";
        return result;
    }
    
    // TODO: Parse and assemble the source code
    // TODO: Write output to outputFile
    
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
