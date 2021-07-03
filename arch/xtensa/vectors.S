#include <linkage.h>

    .section .WindowVectors.text, "ax"
ENTRY(WindowVectors)

END(WindowVectors)

    .section .DebugInterruptVector.text, "ax"
ENTRY(DebugInterruptVector)

END(DebugInterruptVector)

    .section .NMIInterruptVector.text, "ax"
ENTRY(NMIInterruptVector)

END(NMIInterruptVector)

    .section .UserExceptionVector.text, "ax" 
ENTRY(UserExceptionVector)

END(UserExceptionVector)

    .section .KernelExceptionVector.text, "ax"
ENTRY(KernelExceptionVector)

END(KernelExceptionVector)

    .section .DoubleExceptionVector.text, "ax"
ENTRY(DoubleExceptionVector)

END(DoubleExceptionVector)
