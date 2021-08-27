#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include <list>

class Undo;

const std::string START_FIRST_LINE = "";			//if we want to start a document, intitialize with this
const std::string TAB = "    "; 					//TAB is 4 spaces

class StudentTextEditor : public TextEditor 
{
	public:
		StudentTextEditor(Undo* undo);
		~StudentTextEditor();
		bool load(std::string file);
		bool save(std::string file);
		void reset();
		void move(Dir dir);
		void del();
		void backspace();
		void insert(char ch);
		void enter();
		void getPos(int& row, int& col) const;
		int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
		void undo();

	private:
		std::list<std::string>::iterator m_cur_line;	// access to the current line being edited										
		int m_row;										// store the current editing position in the text		
		int m_col;
		std::list<std::string> m_lines;					// store the lines currently being edited	
		int m_num_lines; 								// number of lines being edited

		void moveTo(int r, int c); 						// move cursor to this row, col position
		void del(bool addToUndoStack);					// overloads of functions to ensure that when we undo something, the action of undoing is not added to stack
		void backspace(bool addToUndoStack);
		void insert(char ch, bool addToUndoStack);
		void enter(bool addToUndoStack);
};

#endif // STUDENTTEXTEDITOR_H_