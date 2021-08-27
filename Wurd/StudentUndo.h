#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>

class StudentUndo : public Undo 
{
	public:
		void submit(Action action, int row, int col, char ch = 0);
		Action get(int& row, int& col, int& count, std::string& text);
		void clear();

	private:
		struct Reverse		//stores the complete reverse instructions
		{
			Reverse(Action a, int row, int col, std::string text)
				: act(a), r(row), c(col), t(text) {}
			Action act;
			int r;
			int c;
			std::string t;
		};

		std::stack<Reverse> m_reverses;
		int getColDifferenceForBatch(Action ac); 	//get the difference in columns needed for batching to occur
};

#endif // STUDENTUNDO_H_