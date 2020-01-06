# anyfile2bmp
>anyfile2bmp is a program to encrypt or decipher files with .bmp format.

<code>anyfile2bmp [OPTION] [SRCFNAME] [DESFNAME] [KEYWORD]</code>

## [Option]
anyfile2bmp has 4 options. Actually, 'Option' is __NOT__ optional.
It decides what program will do.

* --help, -h      : shows help message
* --version       : shows program version
* --encrypt, -e   : encrypt [SRCFNAME] to [DESFNAME] with [KEYWORD]
* --decipher, -d  : decipher [SRCNAME] to [DESFNAME] with [KEYWORD]

For example, if you want to encrypt 'Secret.txt' with keyword 'MyPassword', Run this:

<code>anyfile2bmp --encrypt Secret.txt NothingInHere.bmp MyPassword</code>

Then 'NothingInHere.bmp' will be created, it contains encrypted informations of 'Secret.txt'.
To decipher, 

<code>anyfile2bmp --decipher NothingInHere.bmp Original.txt 'MyPassword'</code>

And Original.txt will created, it is the same one with 'Secret.txt' __but__ more bigger.

## To use this program
1. Compile the source code - anyfile2bmp.c
2. (In Windows) Open CMD in the directory which has anyfile2bmp.exe
3. __In the terminal__...

      <code>anyfile2bmp [OPTION] [SRCFNAME] [DESFNAME] [KEYWORD]</code>

## Problems
I'm trying to solve problems below. If you have some idea to solve them, please notice me:)
### The size of bmp file
When encrypt files, because .bmp is 24-bit based image format, Some images(which contains more than 256 infos) are can't be shown.
### Increasing data size
Image file has pixel size. So, when you encrypt and decipher same files again, the result of decipher might be bigger than original one.
It contains many <code>'\0'</code> at the end.
### Weakness
The keyword can be found when someone knows original file format.
anyfile2bmp uses Vigenere Cipher so with original format, part of keyword can be leaked.
