# Coding style

To remain consistent with custard's codebase, anytime curly braces are required to be used, they should have their own dedicated lines.

Method calls with more than  or equal to 3 parameters, or method calls with long parameter values, with precedence for the latter, should have dedicate a single line to each parameter in said call.

Data-types should be on the same line as the method name and paremeters.

Where possible, an if condition should prefer to use implicit boolean evaluation, unless the value of the tested condition can be any value other than true or false (with respect to 1 and 0).

If-conditions should always wrap their codeblocks with curly braces, regardless of whether it is a single line block or not.

Avoid using a break in a loop when possible.

Any dependencies should go only in main.cpp.

Prefer pointers to references.

Methods and variables declared in header files must be defined in the same order in the accompanying C++ file.

The asterisk (representing a pointer) in a data-type must be in front of the variable name, not around the data-type itself, unless a method returns a pointer, in which case the asterisk should directly follow the data-type.

Any data-type that has a modifier of some sort should have said modifier(s) placed before the data-type itself. In the case of data-type representing a number, the sign modifier should follow the number's size allocated for memory (e.g. `short unsigned int` should be prefered to `unsigned short int`).

Arithmetic should have parenthesis around each operation so as not to make incorrect assumptions about the order of operations.

# Commits

Each commit should represent at most three logical changes to the codebase, and be prefixed with `Change`, `Fix`, `Add`, or `Remove`.

Each commit message should be as descriptive as possible.
