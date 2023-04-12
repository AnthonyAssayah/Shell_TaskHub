# MyShell
> Made by Anthony Assayah and Adi Yafe

This project is a simple Unix shell program written in C that allows users to execute basic commands and run simple scripts.



## Available Commands


My shell program supports all the standard commands available in bash. Additionally, it has implemented custom commands required by the assignment. These include:


` cd ` -  Changes the current working directory.

` echo ` - Prints the text after the echo command, including variables as `echo $variable_name`.

`!!` - Executes the last command entered.

`quit` - Exits the shell.

`echo $?` - Prints the exit status of the last executed command.

`read` - Waits for user input and assigns it to a variable.

`$variable_name = value` - Variables can be declared using the syntax. 

`prompt = <new prompt>` - The prompt can also be changed using the syntax.

Redirection: `2>` , `>` , `>>` , and `<` as in bash.

In the implemented shell program, if the user types `Control-C` during command execution, the program will not terminate but instead print the message "You typed Control-C!" to the console.

The shell program supports conditional statements such as if/else, allowing users to execute different commands based on the result of a condition. The syntax is as follows:

``` 
if <command>
then
  <command>
else
  <command>
fi
```

The if statement executes a command and checks if the exit status is successful (0). If it is, the then block is executed. Otherwise, the else block is executed. The statement ends with fi.


  
## Usage


1. Download the program files to your local machine.
2. Open a terminal and navigate to the directory where you saved the program files.
3. Run the command make all to compile the program.
```
make all
```
4. Once the program is compiled, run the command ./myshell to start the shell.
```
./myshell
```
 You should now see the shell prompt (hello: by default).

5. After exiting the shell, run the following command to clean the created files.
```
make clean
```


