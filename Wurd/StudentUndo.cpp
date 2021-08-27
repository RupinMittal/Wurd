#include "StudentUndo.h"
#include <string>
using namespace std;

	//get pointer to new Undo object
Undo* createUndo()
{
	return new StudentUndo;
}

	//submit a undo request
	//this will process batching where appropriate
void StudentUndo::submit(const Action action, int row, int col, char ch) 
{
	string text = "";		//get the text involved in this action
	text += ch;

	//we test batching only for INSERT and DELETE actions
	if ((action == INSERT || action == DELETE) && m_reverses.empty() == false)				//if we have a potential to batch
	{
		Reverse prev = m_reverses.top(); 													//get the previous undo

		if (prev.act == action && prev.r == row												//if we need to batch																									
			&& (((col - prev.c) == getColDifferenceForBatch(action)) || (col == prev.c)))			
		{
			if (!(col == prev.c && action == INSERT))										//should not batch inserts that occur at the same exact column (since that means user inserted, moved left, and then inserted again)
			{
				text = (col == prev.c) ? prev.t + ch : ch + prev.t;								//change the text involved appropriately
				m_reverses.pop();
			}
		}
	}

	m_reverses.push(Reverse(action, row, col, text));
}

	//get the most recent undo and load the data needed to undo the action
StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) 
{
	if (m_reverses.empty() == true)								// if stack is empty, return error (no more undos left)
		return ERROR;

	Reverse current = m_reverses.top();							//load the data from the action
	m_reverses.pop();
	row = current.r;
	col = current.c;
	count = current.act == INSERT ? current.t.size() : 1;		//only need number of deletions done if action was deleting
	text = current.act == DELETE ? current.t : "";				//only need to know what text was added if action was inserting

	switch (current.act)										// return the opposite action that needs to be done
	{
		case INSERT:
			return DELETE;
		case DELETE:
			return INSERT;
		case SPLIT:
			return JOIN;
		case JOIN:
			return SPLIT;
		default:
			return ERROR;
	}
}

	//clear the stack of undos so that it's empty
void StudentUndo::clear() 
{
	while (!m_reverses.empty())
		m_reverses.pop();
}

	//ASSUMES only INSERT and DELETE will be passed as arguments
	//returns the appropriate difference between current and prev's columns needed for batching to occur
int StudentUndo::getColDifferenceForBatch(Action ac)
{
	if (ac == INSERT)		//to batch inserts, return 1 (current col should be 1 greater than previous col)
		return 1;
	return -1;				//to batch deletes (really backspaces), return -1 (current col should be 1 less than previous call)
}