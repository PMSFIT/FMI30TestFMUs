Simple Test FMU using FMI 3.0 Dynamic Array Variables
=====================================================

This example FMU demonstrates the use of FMI 3.0 dynamic
array variables, as well as basic co-simulation.

In order to compile the sample FMU the build process can be started
normally via CMake (no further requirements have to be fulfilled).

In order to test the proper functioning of the FMU, it is advisable
to switch on either `PUBLIC_LOGGING` (which logs via the standard FMI
logging callback), and/or `PRIVATE_LOGGING` (which logs to a fixed
file without interaction with the host implementation).  If very
fine-grained logging of actual FMI API calls is wanted, the flag
`VERBOSE_FMI_LOGGING` can be switched on.
