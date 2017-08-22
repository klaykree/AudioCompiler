# AudioCompiler
A language and compiler designed to be encoded into WAV files. IDE here: https://github.com/klaykree/AudioIDE

I made this compiler to learn about machine code and to have a language that can be encoded/decoded to and from a sound file,
inspired to do this due to games and programs being stored on 'datassettes' for some Commodore computers, and the easter eggs hidden
in sound files of the 2016 Doom game.  
I chose not to use LLVM because the purpose was to learn about compiling to a Portable Executable, not to make a useful language.  
This may make it an [esoteric programming language](https://www.esolangs.org)

#### File types
Exe and WAV export  
WAV import

#### Instruction set
- Add assign (+=)
- Subtract assign (-=)
- Multiply assign (*=)
- Divide assign (/=)
- Print: variable, variable with new line, new line
- Assign to variable: from constant or variable
- If: variable equals variable or constant
- "For" loop, iterate number of times. Can be a constant or variable

#### Limitations
- If can only perform equality check, no less than, greater than, or not equals
- Machine code is not optimised at all
- Data and text sections of the exe have a maximum of 512 bytes.  
  - Data section: there is a limit of ((512 - 9) / 4) = (125) variables that can be made.  
Minus 9 bytes because the printf formats reside in the data section, and divide by 4 because all variables are 32 bits
  - Text section: there is a limit of roughly (512 / 15) = (34) instructions.
  Divide by 15 because the instructions range between 5 and 21 bytes with 10 to 15 being the most common lengths
