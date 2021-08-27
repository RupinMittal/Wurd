# Wurd Text Editor

### Description

This is a text editor built in C++. These are it's features:

* Up, Down, Left, Right arrows  
  - Move the cursor       
* Home (Command+Left on Mac)  
  - Move to the start of the current line
* End (Comand+right on Mac)  
  - Move to the end of the current line
* PgUp (Function+Up on Mac)  
  - Up one page
* PgDown (Function+Down on Mac)  
  - Down one page
* Delete (Function+Delete on Mac)  
  - Delete the character after the cursor
* Backspace (Delete on Mac)  
  - Backspace over the previous character
* Ctrl-Z  
  - Undo last change
* Ctrl-S  
  - Save the current file
* Ctrl-L  
  - Load a new file (on success, discard any changes to the current file)
* Ctrl-D  
  - Load a new dictionary (on success, discard the entire old dictionary)
* Ctrl-X  
  - Exit the editor (discard any changes to the current file)

### How to use
1. Clone the repo: ```$ git clone https://github.com/RupinMittal/Wurd.git```
2. Change into Wurd: ```$ cd Wurd```
3. Use the Makefile: ```$ make```
4. Launch the executable: ```$ ./wurd```

A sample dictionary has been provided, and it will automatically be loaded. There
are also sample txt files (like warandpeace). Of course, you may load your own dictionary 
and load/create your own documents.

5. To clear object files: ```$ make clean```

Special thank you to Prof. David Smallberg for his support & guidance
