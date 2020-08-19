Simple Test FMU using FMI 3.0 Array Variables
=============================================

This example FMU demonstrates the use of all FMI 3.0 defined
static array variable types, as well as basic co-simulation.

In order to compile the sample FMU the build process can be started
normally via CMake (no further requirements have to be fulfilled).

In order to test the proper functioning of the FMU, it is advisable
to switch on either `PUBLIC_LOGGING` (which logs via the standard FMI
logging callback), and/or `PRIVATE_LOGGING` (which logs to a fixed
file without interaction with the host implementation).  If very
fine-grained logging of actual FMI API calls is wanted, the flag
`VERBOSE_FMI_LOGGING` can be switched on.

The FMU provides tunable parameter, input, output constant, tunable
calculated parameter, and output variables for all basic data types
of FMI 3.0 (i.e. boolean, integer, floating-point, string and binary
data types) as 2x3 arrays. Enumerations are currently not being
provided.

For all types the following calculations are performed element-wise
on the corresponding array elements:

For the boolean type, the calculated parameter is calculated as the
XOR of the tunable parameter and the constant, and the output is
calculated as the XOR of the input and the tunable parameter.

For the numeric types, the calculated parameter is calculated as the
product of the tunable parameter and the constant, and the output is
calculated as the product of the input and the tunable parameter.

For the string type, the calculated parameter is calculated as the
concatenation of the tunable parameter and the constant, and the
output is calculated as the concatenation of the input and the
tunable parameter.

For the binary type, the FMU provides an additional output. The
calculatedParameter is derived from the tunable parameter by XORing
each byte with a corresponding byte from the binary constant (rotating
over those bytes if more bytes are needed). Similarly the second
binary output will be calculated by XORing each byte of the binary
input with the corresponding byte of the tunable parameter (again
wrapping around if necessary), whereas the first output will always be
a copy of the binary input.
