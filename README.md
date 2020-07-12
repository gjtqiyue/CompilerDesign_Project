# Comp520_CompilerDesign

We provide 3 scripts for convenience and two directories for organization:

programs: Test programs are organized by compilation phase and by expected result. Valid programs should output OK and status code 0, while invalid programs should output Error: <description> and status code 1.
 
Scan+parse: Runs both the scanner and parser phases

Typecheck: Runs until the end of the typechecker phase

Codegen: Runs until your compiler outputs the target code

src: Source code for your assignment

build.sh: Builds your compiler using Make or similar. You should replace the commands here if necessary to build your compiler

run.sh: Runs your compiler using two arguments (mode - $1 and input file - $2). You should replace the commands here if necessary to invoke your compiler
  
 modes:
 
    - scan : Outputs OK if the input is lexically correct, or an appropriate error message
    
    - tokens : Outputs the token kinds, one per line, until the end of le. Tokens with associated data (literals, identiers, etc) should be printed with their respective information
    
    - parse : Outputs OK if the input is syntactically correct, or an appropriate error message
    
    - pretty : Outputs the pretty printed code
    
    - symbol : Outputs the symbol representation for the tree
    
    - typecheck : Outputs OK if the input is typechecked
    
    - codegen : Output generated GoLite code to the provided file destination
    
test.sh: Automatically runs your compiler against test programs in the programs directory and checks the output
