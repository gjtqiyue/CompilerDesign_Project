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
    - scan
    - tokens
    - parse
    - pretty
    - symbol
    - typecheck
    - codegen
test.sh: Automatically runs your compiler against test programs in the programs directory and checks the output
