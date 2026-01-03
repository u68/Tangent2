import std.stdio;
import std.array;
import std.string;
import std.file : exists, isFile, read, FileException;
import std.conv;
import core.stdc.stdlib : exit;

// Build error codes
enum CombinerError {
    SUCCESS = 0,
    FILE_NOT_FOUND = 1,
    INVALID_INPUT = 2,
    INVALID_ADDRESS = 3,
    OUTPUT_ERROR = 4,
    UNKNOWN_ERROR = 99
}

struct CombinerResult {
    CombinerError errorCode;
    string errorMessage;
    string[] warnings;
    string[] missingFiles;
    bool success() const { return errorCode == CombinerError.SUCCESS; }
}

// Validate a 5-digit hex address
bool validateHexAddress(string addr, ref string errorMsg) {
    if (addr.length != 5) {
        errorMsg = "Inject address must be exactly 5 hex digits, got: " ~ addr;
        return false;
    }
    
    foreach (c; addr) {
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
            errorMsg = "Invalid hex character in address: " ~ addr;
            return false;
        }
    }
    
    return true;
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
CombinerResult validateAllInputFiles(string[] inputFiles) {
    CombinerResult result;
    result.errorCode = CombinerError.SUCCESS;
    
    foreach (inputFile; inputFiles) {
        string validationError;
        if (!validateInputFile(inputFile, validationError)) {
            result.missingFiles ~= inputFile;
        }
        
        // Check for correct extension (.tp for Tangent Program)
        if (!inputFile.endsWith(".tp")) {
            result.warnings ~= "Warning: " ~ inputFile ~ " does not have .tp extension";
        }
    }
    
    if (result.missingFiles.length > 0) {
        result.errorCode = CombinerError.FILE_NOT_FOUND;
        result.errorMessage = "One or more input files not found";
    }
    
    return result;
}

// Combine multiple .tp files into a single ROM
CombinerResult combine(string[] inputFiles, string outputName, bool hexOutput, bool binOutput, 
                       string password, string clockspeed, string injectAddress) {
    CombinerResult result;
    result.errorCode = CombinerError.SUCCESS;
    
    // First validate all input files exist
    result = validateAllInputFiles(inputFiles);
    if (!result.success()) {
        return result;
    }
    
    // Validate inject address
    string addrError;
    if (!validateHexAddress(injectAddress, addrError)) {
        result.errorCode = CombinerError.INVALID_ADDRESS;
        result.errorMessage = addrError;
        return result;
    }
    
    writeln("Combining ", inputFiles.length, " program file(s)...");
    writeln("Inject address: 0x", injectAddress.toUpper());
    
    // Display password and clockspeed
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
            // TODO: Process program file data
        } catch (FileException e) {
            result.errorCode = CombinerError.FILE_NOT_FOUND;
            result.errorMessage = "Failed to read " ~ inputFile ~ ": " ~ e.msg;
            return result;
        }
    }
    
    // TODO: Implement actual combining logic here
    // - Load base ROM (if using one)
    // - Parse each .tp program file
    // - Inject programs at specified address
    // - Apply password to ROM
    // - Apply clockspeed settings to ROM
    // - Generate output files
    
    if (hexOutput) {
        writeln("Generating HEX output: ", outputName, ".hex");
        // TODO: Generate .hex file
    }
    
    if (binOutput) {
        writeln("Generating BIN output: ", outputName, ".bin");
        // TODO: Generate .bin file
    }
    
    writeln("Combining completed successfully.");
    return result;
}

void main(string[] args) {
    bool flagBH = false; // BIN and HEX output
    bool verbose = false;
    string outputName;
    string password;
    string clockspeed;
    string injectAddress;
    string[] inputFiles;

    if (args.length < 2) {
        writeln("Usage: combiner-t16 [options] <file1.tp> ...");
        writeln("Options:");
        writeln("  -bh            Enable both BIN and HEX file output");
        writeln("  -o <name>      Specify output file name (without extension)");
        writeln("  -p <password>  Chip password (64 hex chars = 32 bytes)");
        writeln("  -c <clock>     Clockspeed setting (32 hex chars = 16 bytes)");
        writeln("  -i <address>   Inject address (5 hex digits, e.g., 06ACE)");
        writeln("  -v             Verbose output");
        exit(CombinerError.INVALID_INPUT);
    }

    size_t i = 1;
    while (i < args.length) {
        string arg = args[i];
        if (arg == "-bh") {
            flagBH = true;
        } else if (arg == "-o") {
            if (i + 1 < args.length) {
                outputName = args[i + 1];
                i++;
            } else {
                writeln("Error: -o requires an output name");
                exit(CombinerError.INVALID_INPUT);
            }
        } else if (arg == "-p") {
            if (i + 1 < args.length) {
                password = args[i + 1];
                i++;
            } else {
                writeln("Error: -p requires a password hex string");
                exit(CombinerError.INVALID_INPUT);
            }
        } else if (arg == "-c") {
            if (i + 1 < args.length) {
                clockspeed = args[i + 1];
                i++;
            } else {
                writeln("Error: -c requires a clockspeed hex string");
                exit(CombinerError.INVALID_INPUT);
            }
        } else if (arg == "-i") {
            if (i + 1 < args.length) {
                injectAddress = args[i + 1];
                i++;
            } else {
                writeln("Error: -i requires a 5-digit hex inject address");
                exit(CombinerError.INVALID_INPUT);
            }
        } else if (arg == "-v") {
            verbose = true;
        } else if (arg.startsWith("-")) {
            writeln("Unknown flag: ", arg);
            exit(CombinerError.INVALID_INPUT);
        } else {
            inputFiles ~= arg;
        }
        i++;
    }

    if (inputFiles.length == 0) {
        writeln("Error: no input files specified");
        exit(CombinerError.INVALID_INPUT);
    }

    if (outputName.length == 0) {
        outputName = "rom"; // default output name
    }

    if (injectAddress.length == 0) {
        writeln("Error: Inject address (-i) is required");
        exit(CombinerError.INVALID_INPUT);
    } else if (injectAddress.length > 0 && pasinjectAddresssword.length != 5) {
        writeln("Error: Inject address has to be 5 hex digits long, got ", injectAddress.length);
    }

    // Validate password format if provided
    if (password.length > 0 && password.length != 64) {
        writeln("Error: Password should be 64 hex characters (32 bytes), got ", password.length);
        exit(CombinerError.INVALID_INPUT);
    }

    // Validate clockspeed format if provided
    if (clockspeed.length > 0 && clockspeed.length != 32) {
        writeln("Error: Clockspeed should be 32 hex characters (16 bytes), got ", clockspeed.length);
        exit(CombinerError.INVALID_INPUT);
    }

    writeln("Tangent Combiner Framework");
    if (verbose) {
        writeln("Input files: ", inputFiles);
        writeln("Output name: ", outputName);
        writeln("BH output: ", flagBH);
        writeln("Inject address: 0x", injectAddress.toUpper());
        if (password.length > 0) {
            writeln("Password: ", password);
        }
        if (clockspeed.length > 0) {
            writeln("Clockspeed: ", clockspeed);
        }
    }

    // Perform the combining
    CombinerResult result = combine(inputFiles, outputName, flagBH, flagBH, password, clockspeed, injectAddress);
    
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
        writeln("Combine failed with error code: ", cast(int)result.errorCode);
        exit(result.errorCode);
    }
    
    writeln("Combine successful!");
    exit(CombinerError.SUCCESS);
}
