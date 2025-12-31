import std.stdio;
import std.array;
import std.string;
import std.file : exists, isFile, read, FileException;
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

// Link object files together
LinkResult link(string[] inputFiles, string outputFile, bool hexOutput, bool binOutput, string password, string clockspeed) {
    LinkResult result;
    result.errorCode = LinkError.SUCCESS;
    
    // First validate all input files exist
    result = validateAllInputFiles(inputFiles);
    if (!result.success()) {
        return result;
    }
    
    writeln("Linking ", inputFiles.length, " object file(s)...");
    
    // Display password and clockspeed if provided
    if (password.length > 0) {
        writeln("Using password: ", password);
    }
    if (clockspeed.length > 0) {
        writeln("Using clockspeed: ", clockspeed);
    }
    
    // Try to read all input files
    foreach (inputFile; inputFiles) {
        try {
            auto data = read(inputFile);
            if (data.length == 0) {
                result.warnings ~= "Warning: " ~ inputFile ~ " is empty";
            }
            // TODO: Process object file data
        } catch (FileException e) {
            result.errorCode = LinkError.FILE_NOT_FOUND;
            result.errorMessage = "Failed to read " ~ inputFile ~ ": " ~ e.msg;
            return result;
        }
    }
    
    // TODO: Implement actual linking logic here
    // - Symbol resolution
    // - Address relocation
    // - Output generation
    // - Apply password and clockspeed to ROM
    
    if (hexOutput) {
        writeln("Generating HEX output...");
        // TODO: Generate .hex file
    }
    
    if (binOutput) {
        writeln("Generating BIN output...");
        // TODO: Generate .bin file
    }
    
    writeln("Linking completed successfully.");
    return result;
}

void main(string[] args) {
    bool flagH = false; // HEX output
    bool flagB = false; // BIN output
    bool verbose = false;
    string outputFile;
    string password;
    string clockspeed;
    string[] inputFiles;

    if (args.length < 2) {
        writeln("Usage: tl-t16 [options] <file1.to> ...");
        writeln("Options:");
        writeln("  -h             Enable HEX file output");
        writeln("  -b             Enable BIN file output");
        writeln("  -o <name>      Specify output file name");
        writeln("  -p <password>  Chip password (64 hex chars = 32 bytes)");
        writeln("  -c <clock>     Clockspeed setting (32 hex chars = 16 bytes)");
        writeln("  -v             Verbose output");
        exit(LinkError.INVALID_INPUT);
    }

    size_t i = 1;
    while (i < args.length) {
        string arg = args[i];
        if (arg.startsWith("-")) {
            foreach (idx, c; arg[1 .. $]) {
                switch (c) {
                    case 'h': flagH = true; break;
                    case 'b': flagB = true; break;
                    case 'v': verbose = true; break;
                    case 'o':
                        if (i + 1 < args.length) {
                            outputFile = args[i + 1];
                            i++;
                        } else {
                            writeln("Error: -o requires an output file");
                            exit(LinkError.INVALID_INPUT);
                        }
                        break;
                    case 'p':
                        if (i + 1 < args.length) {
                            password = args[i + 1];
                            i++;
                        } else {
                            writeln("Error: -p requires a password hex string");
                            exit(LinkError.INVALID_INPUT);
                        }
                        break;
                    case 'c':
                        if (i + 1 < args.length) {
                            clockspeed = args[i + 1];
                            i++;
                        } else {
                            writeln("Error: -c requires a clockspeed hex string");
                            exit(LinkError.INVALID_INPUT);
                        }
                        break;
                    default:
                        writeln("Unknown flag: -", c);
                        exit(LinkError.INVALID_INPUT);
                }
            }
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
        outputFile = "out.bin"; // default output
    }

    writeln("Tangent Linker Framework");
    if (verbose) {
        writeln("Input files: ", inputFiles);
        writeln("Output file: ", outputFile);
        writeln("HEX output: ", flagH);
        writeln("BIN output: ", flagB);
        if (password.length > 0) {
            writeln("Password: ", password);
        }
        if (clockspeed.length > 0) {
            writeln("Clockspeed: ", clockspeed);
        }
    }

    // Perform the linking
    LinkResult result = link(inputFiles, outputFile, flagH, flagB, password, clockspeed);
    
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
