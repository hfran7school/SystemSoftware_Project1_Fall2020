# SystemSoftware_Project1_Fall2020
COP3404: Project 1 -- Pass 1 through SIC Assembly
 * Author: Hailey Francis (n01402670@unf.edu)
 * Last Edited: 9/24/20

I did not upload all 10 test.sic files we were given since I do not own them, however I found it necessary to still upload the first test file to show what the give an example of a symbol table generated from a given sic file.

The following is the documentation I turned in to my professor upon completion of the project. 

# Hailey Francis (n01402670) Project 1 Documentation
My symbol and opcode tables are declared outside of my main function so that my all of my functions, including my main, can reference it. I used seperate chaining with linked lists to add the symbols to the table hashing with the alphabet method, using the examples used in the past few lectures, including the Friday meeting on 9/11/20.

Errors are thrown if there are duplicate symbols, invalid bytes when declaring a byte word constant, labels with directive names, going above the SIC memory limit at address 8000(hex), when a word constant is bigger than 24 bits (signed with 2's complement), and ignores comment lines entirely. It will also throw an error if a symbol name is too large or if a symbol has an invalid character and if there's a duplicate START symbol. Overall, it catches all the errors in the test.sic files provided, as well as some others.

To check if there is a label, the program identifies if line[0] is ASCII A-Z. If so, it commences with parsing the line into temporary label, opcode, and operand strings, error checking along the way. If no errors are found in the current line, it updates the address location counter and sourceline counter, and dereferences the Name, Sourceline, and Address (in decimal)  to a temporary Symbol pointer structure and inserts that Symbol pointer into the hash table. It prints the Symbol name and address not only to the terminal but an intermediate file named SymTab.txt. It will calculate the length of the program if there are no errors.

When there is not a symbol, the program still checks that each line has a valid opcode/directive and updates the location counter and sourceline counter accordingly.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

*SHORTCOMINGS:
As of now, there's not a strong error checking for if there is an END directive. If there is an END directive and more code after, an error is not thrown, however it will not add anything to the symbol table after the END directive is encountered.  There were no examples of this happening in the 10 test.sic files we were given. 

*I'm also not sure if I caught all of the errors in checking the parameters for the directives. I don't know what makes a character constant invalid and did not do any checks for that.

*I also noticed in test7.sic (the one with empty lines) that in my VScode on Windows said that the empty line line[0] = 10, which is Line Feed, but on the Linux server line[0] = 13, which is Carriage Return. I accounted for both but I don't know if there's more I should look out for here.
