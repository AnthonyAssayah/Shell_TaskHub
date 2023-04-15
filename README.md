# MyShell
> Made by Anthony Assayah and Adi Yafe

This project is a simple Unix shell program written in C that allows users to execute basic commands and run simple scripts.



## Available Commands


My shell program supports all the standard commands available in bash. Additionally, it has implemented custom commands required by the assignment. These include:


- ` cd ` -  Changes the current working directory.

- ` echo ` - Prints the text after the echo command, including variables as `echo $variable_name`.

- `!!` - Executes the last command entered.

- `quit` - Exits the shell.

- `echo $?` - Prints the exit status of the last executed command.

- `read` - Waits for user input and assigns it to a variable.

- `$variable_name = value` - Variables can be declared using the syntax. 

- `prompt = <new prompt>` - The prompt can also be changed using the syntax.

- Redirection: `2>` , `>` , `>>` , and `<` as in bash. (make sure to not forgot `SPACE` between the last character and the sign in each side)

- Multiple pipes commands, separate by `|` work correctly. (make sure to not forgot `SPACE` between the last character and the sign in each side)

- In the implemented shell program, if the user types `Control-C` during command execution, the program will not terminate but instead print the message "You typed Control-C!" to the console.

## Logic Command

The shell program supports conditional statements such as if/else, allowing users to execute different commands based on the result of a condition. The syntax is as follows:

``` 
if <command>
then
  <command>
else
  <command>
fi
```

The _if_ statement executes a command and checks if the exit status is successful (0). If it is, the _then_ block is executed. Otherwise, the _else_ block is executed. The statement ends with _fi_. For example,

```
if ls | grep my_file.txt
then
  echo "file exists"
else
  echo "file does not exist"
fi
```
This command will print "file exists" if the file exists in the current directory, otherwise it will display "file does not exist".

## Video example _if/else_


https://user-images.githubusercontent.com/92322613/232252545-f0d8b214-3664-462e-bb73-480a33508899.mp4



## History Command

The history feature in this shell allows you to navigate through the last 20 commands executed using the ` arrow up ` or ` arrow down ` keys and then ` Enter `. The last executed command is displayed on the terminal and can be executed again by pressing ` Enter `. (In the begging of the function, we let the Printhistory() function for debugging, you can erase it you want!)

## Examples

- _Read_ command need to be in this format:
```
hello: read $variable_name
$value
hello: echo $varibale_name
$value

hello: read name
David
hello: echo $name
David
```
- Adding new variables:
```
hello: $city = London
hello: echo $city
London
```

## Video with examples

https://user-images.githubusercontent.com/92322613/232251585-857d3cff-c798-42cf-9e9f-7c05bf0c1a07.mp4



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


