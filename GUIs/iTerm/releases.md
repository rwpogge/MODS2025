# iTerm Release Notes
Last Build: 2024 Jul 8

## Version 2.2.0
Monday, July 8th, 2024

- Qt 5.X -> Qt 6.X
    - The GUI should now be compiled using Qt 6.

## Version 2.1.0
Friday, February 23rd, 2024

- Qt 4.X -> Qt 5.X
    - The GUI should now be compiled using Qt 5.
- All compiler warnings were fixed.
    - In several instances `QString::simplified()` was used - but the return value wasn't. These locations were fixed and marked with TODOs.
    
## Version 2.0 bino
This was the first version committed to the repository.