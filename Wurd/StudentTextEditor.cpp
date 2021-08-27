#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

	//StudentTextEditor Constructor
StudentTextEditor::StudentTextEditor(Undo* undo)
	: TextEditor(undo) 
{
	m_row = 0;									// start off at first row, first column, with no lines
	m_col = 0;									
	m_num_lines = 0;
}

	//StudentTextEditor Destructor
StudentTextEditor::~StudentTextEditor()
{
	//no dynamic storage
}

	//Load in a file into the editor
bool StudentTextEditor::load(std::string file) 
{
	ifstream infile(file);									//try and open the file
	if ( ! infile)											//if could not open file
		return false;

	//if we could open the file, load the file:						
	reset();												//reset the current state if we were already working on a file

	//load the file:
	string str;
	while (getline(infile, str))											//for each line (string) in the file
	{
		if (str.length() >= 1 && str[str.length() - 1] == '\r')				//add it, and remove the '\r' character if there is one
			m_lines.push_back(str.substr(0, str.length() - 1));
		else
			m_lines.push_back(str);
		m_num_lines++;														//increment the number of lines
	}
	m_cur_line = m_lines.begin();											//editing position is at the very first character after loading

	return true;
}

	//save the file currently being edited
bool StudentTextEditor::save(std::string file) 
{
	ofstream outfile(file);   // try and open the file
	if ( ! outfile )		  // if opening/creation failed, return false
	    return false;  

	if (m_num_lines > 0)	  // if current file is not empty
		for (list<string>::iterator current = m_lines.begin(); current != m_lines.end(); current++)		//put every line into the editor 
			outfile << *current << endl;																//and add a newline character
 
	return true;
}

	//Reset the state of the editor
void StudentTextEditor::reset() 
{
	m_lines.clear();				// clear all the lines being edited
	m_num_lines = 0;
	m_cur_line = m_lines.begin();	// start at first line
	m_row = 0;						// start off row at first line
	m_col = 0;						// start off column at first character
	
	getUndo()->clear();				// clear the undo stack
}

	//move based on user input
void StudentTextEditor::move(Dir dir) 
{
	if (m_num_lines > 0)				//if there are lines in the editor, do the move (we cannot move if there are no lines)
	{
		switch (dir)
		{
			case UP:
				if (m_row != 0)										//if not at the top line, move up one line
				{
					m_cur_line--;
					m_row--;
					if (m_col > (*m_cur_line).length())				//don't go past just past the end of the line we moved to
						move(END);
				}
				break;
			case DOWN:												
				if (m_num_lines > 0 && m_row != m_num_lines - 1)	//if current position is not at the bottom line, move down one line
				{
					m_cur_line++;
					m_row++;
					if (m_col > (*m_cur_line).length())				//don't go past just past the end of the line
						move(END);
				}
				break;
			case LEFT:												
				if (m_col != 0)										//if not at the start of line, move left
					m_col--;
				else
				{
					if (m_row != 0)									//if at the start of the line, move to end of line above
					{
						move(UP);
						m_col = (*m_cur_line).length();
					}
				}
				break;
			case RIGHT:												
				if (m_col != (*m_cur_line).length())				//if not at the end of the line, move right
					m_col++;
				else
				{
					if (m_row != m_num_lines - 1)					//if at the end of line, move to start of line below
					{
						move(DOWN);
						m_col = 0;
					}
				}
				break;
			case HOME:												//move to start of current line
				m_col = 0;
				break;
			case END:												//move to position just past the end of current line
				m_col = (*m_cur_line).length();
				break;
		}
	}
}

	//delete action
void StudentTextEditor::del() 
{
	del(true);		//do a deletion and add to undo stack
}

	//delete action, need to specifiy if this action should add to undo stack
void StudentTextEditor::del(bool addToUndoStack)
{
	if (m_col == 0 && m_row == 0 && m_num_lines == 0)		//if we are at the start of the first line, do nothing
		return;

	int end_of_line = (*m_cur_line).length();

	if (m_col == end_of_line && m_row == m_num_lines - 1)				//if currently past the end of the last line, do nothing
		return;
	else
	{
		if (m_col == end_of_line)										//if at end of not-last-lane
		{	
			list<string>::iterator nextLine = m_cur_line;
			nextLine++;
			*m_cur_line += *nextLine;									//append next line to current line
			m_cur_line = m_lines.erase(nextLine);						//erase next line and come back to current line
			m_cur_line--;
			m_num_lines--;
			if (addToUndoStack == true)										//add to undostack if appropriate
				getUndo()->submit(Undo::Action::JOIN, m_row, m_col, '\0');	//pass in null character since no characters were involved
		}
		else																//if in the middle of a line
		{	string current = *m_cur_line;
			*m_cur_line = current.substr(0, m_col) + current.substr(m_col + 1, current.size() - m_col - 1);			//delete the character we are at, without moving positions
			if (addToUndoStack == true)																				//and add to undo stack if appropriate
				getUndo()->submit(Undo::Action::DELETE, m_row, m_col, current[m_col]);
		}
	}
}

	//backspace action
void StudentTextEditor::backspace() 
{
	backspace(true);
}

	//backspace action, need to specifiy if this action should add to undo stack
void StudentTextEditor::backspace(bool addToUndoStack)
{
	//backspace can implemented using delete
	//the undo is taken care of by delete

	if (m_col == 0 && m_row == 0)		//if we are at the start of the first line, do nothing
		return;
	else
	{
		if (m_col == 0)					//if we are at the start of the not-first line, merge with line above (same as doing del from end of above line)
		{
			move(LEFT);
			del(addToUndoStack);
		}
		else							//if we are in the middle of line, remove character in front and stay on current character (same as calling del on previous character)
		{
			m_col--;
			del(addToUndoStack);
		}
	}
}

	//insert action
void StudentTextEditor::insert(char ch) 
{
	insert(ch, true); 	//insert and add to undo stack
}

	//insert action, need to specifiy if this action should add to undo stack
void StudentTextEditor::insert(char ch, bool addToUndoStack)
{
	if (m_lines.empty())
	{
		m_lines.push_back(START_FIRST_LINE);		//for empty document, put in an empty first line
		m_cur_line = m_lines.begin();				//start at first line
		m_num_lines++;
	}

	string currentLine = (*m_cur_line);
	if (ch == '\t')																											//if we have a tab
	{	
		(*m_cur_line) = currentLine.substr(0, m_col) + TAB + currentLine.substr(m_col, currentLine.size() - m_col);			//add in 4 spaces
		m_col += TAB.length();

		for (int i = 0; i < TAB.size(); i++)																				//tabs always go on undo stack, since tabs are inserted
			getUndo()->submit(Undo::Action::INSERT, m_row, m_col - TAB.length() + i + 1, ' ');								//as spaces, so we'll never need to undo a '\t'
	}
	else
	{
		(*m_cur_line) = currentLine.substr(0, m_col) + ch + currentLine.substr(m_col, currentLine.size() - m_col);			//add the regular character in
		m_col++;

		if (addToUndoStack == true)																							//and put it on the undostack if appropriate
			getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);
	}
}

	//press enter action
void StudentTextEditor::enter() 
{
	enter(true);		//enter and add to undo stack
}

	//press enter action, need to specifiy if this action should add to undo stack
void StudentTextEditor::enter(bool addToUndoStack)
{
	if (m_lines.empty())							//if there is nothing in the file yet
	{
		m_lines.push_back(START_FIRST_LINE);		//for empty document, put in an empty first line
		m_cur_line = m_lines.begin();				//start at first line
		m_num_lines++;
	}

	string currentLine = *m_cur_line;													//get the current line
	string nextLine = currentLine.substr(m_col, currentLine.length() - m_col);			//make the split and decide what the next line should have
	*m_cur_line = currentLine.substr(0, m_col);											//change our current line with the split
	m_cur_line = m_lines.insert(++m_cur_line, nextLine);								//insert a new line below with the split, and stay at the line just added

	m_num_lines++;																		//update positions
	if (addToUndoStack == true)															//and call undo before updating row and col. use null character 
		getUndo()->submit(Undo::Action::SPLIT, m_row, m_col, '\0');						//since no other characters were affected
	m_row++;
	m_col = 0;
}

	//get the current editing position
void StudentTextEditor::getPos(int& row, int& col) const 
{
	row = m_row;
	col = m_col;
}

	//copy the specified lines from the current file into the vector
int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const 
{
	std::list<std::string>::iterator current = m_cur_line;				//temp iterator starts at initial position
	if (startRow < 0 || startRow >= m_num_lines)						//if startRow is not a valid start point, return -1
		return -1;

	for (int i = abs(m_row - startRow); i > 0; --i)						//move temp iterator to startRow
	{
		if (startRow < m_row)
			current--;
		else
			current++;
	}

	lines.clear();														//ensure that the vector is empty
	for (int i = 0; i < numRows && current != m_lines.end(); ++i)		//add numRows number of lines and stop if there are no more lines to add
	{	
		lines.push_back(*current);
		current++;
	}

	return lines.size();												//return how many lines were loaded into vector
}

	//Undo when CTRL-Z is pressed
void StudentTextEditor::undo() 
{
	int row, col, count;
	string text;
	Undo::Action act = getUndo()->get(row, col, count, text);

	if (act != Undo::Action::ERROR)					//move to correct position
		moveTo(row, col);

	switch (act)
	{
		case Undo::Action::ERROR:					//for error, do nothing
			break;
		case Undo::Action::DELETE:					//for deletions, delete the appropriate characters (by using backspace), and don't add this to undo stack
			for (int i = 0; i < count; i++)
				backspace(false);
			break;
		case Undo::Action::INSERT:					//for insert, insert the appropriate characters, and don't add this to undo stack
			for (int i = 0; i < text.length(); i++)
				insert(text[i], false);
			moveTo(row, col);						//go back to where we started inserting
			break;
		case Undo::Action::JOIN:					//delete at the end of the line to join, and don't add this to undo stack
			del(false);
			break;
		case Undo::Action::SPLIT:					//enter at the end of the line to split, and don't add this to undo stack			
			enter(false);							//and then move to position from before enter's call
			moveTo(row, col);
			break;
	}
}

	//Move current editing position to r, c
void StudentTextEditor::moveTo(int r, int c)
{
	m_col = c;										//move to column

	for (int i = abs(m_row - r); i > 0; --i)		//move iterator to row r
	{
		if (r < m_row)
			m_cur_line--;
		else
			m_cur_line++;
	}
	m_row = r;										//move to row r
}