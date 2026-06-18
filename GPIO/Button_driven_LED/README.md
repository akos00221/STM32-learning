## Things I learned during this assignment:
- "Sliding window" button debounce method: We first read the value of the pin which controlls the button. Then we shift the either 0 or 1 value into a given variable and once all bits are either 0 or 1 then a change occurs. Previously I used thresholds to debounce buttons but this is a clear improvement. 
- NVIC (Nested Vector Interrupt Controller): Basically a more efficient way to handle interrutps. All interrupts are pre-mapped, priorities can be user defined and all interrupts themselves are interruptable by higher priority ones.
- TIM2->SR interrupts the program. We then verify why the interrupt happened with TIM_SR_UIF, execute the logic and then clear this same flag. 
Note: I still keep some abstraction by using the STM32CubeMx to set up the initial parameters.
