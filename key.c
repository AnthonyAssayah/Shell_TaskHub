//https://stackoverflow.com/questions/10463201/getch-and-arrow-codes
#include <stdio.h>

int main()
{
	int c,n = 0;

	while (( c=getchar()) != 'Q')
	{
		
		if (c == '\033')
		{
			
			
			//these two strange prints are from:
			//https://itnext.io/overwrite-previously-printed-lines-4218a9563527
			//and they delete the previous line
			printf("\033[1A");//line up
			printf("\x1b[2K");//delete line
		
			getchar(); //skip the [
				
			switch(getchar()) { // the real value
			
			case 'A':
				if( getchar() == '\n') {
					 if (history_index > 0) {
							history_index--;
						}
				}
			    break;
			case 'B':
			    if (getchar() == '\n') {
					  if (history_index < history.capacity - 1 && history.cmd_history[history_index + 1] != NULL) {
							history_index++;
						}	
				}
			    break;
			case default:
			    break;
    			}
			
		}
		else if (c == '\n')
		{
			strcpy(command, history.cmd_history[history_index]);
			printf("\r%s", command); // print the last command
			fflush(stdout);
		}
	}

	return 0;
}

//to get more info , you can read teh book chapter 5,page 151 (172 in pdf)
//also take a look here: https://dev.to/napicella/linux-terminals-tty-pty-and-shell-192e
