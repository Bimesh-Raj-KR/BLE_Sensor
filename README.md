# Shared Memory

A C project to create two **apps**, one to write a new line to a file and one to read the number of new lines in the same file. Both **apps** are synchronized using a **shared memory mutex.**

## Features

**appWrite** write a new line in a file until the loop iterates **300** times.   
**appRead** reads the new line count from the file indefinitely until count remains same for **5** consecutive iterations.    
The delay between iterations for write is **600ms** and for read is **900ms.**   

## Building with Make

### $ make write

Output: appWrite

### $ make read

Output: appRead

## Requirements

Run **./appWrite** before **./appRead** inorder to create **shared memory mutex** and file **memoryAccess.txt** from which appRead count lines.