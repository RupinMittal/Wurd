#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"
#include <string>
#include <vector>

const int NUM_ACCEPTABLE_CHARS = 27;

class StudentSpellCheck : public SpellCheck 
{
	public:
		StudentSpellCheck();
		virtual ~StudentSpellCheck();
		bool load(std::string dict_file);
		bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
		void spellCheckLine(const std::string& line, std::vector<Position>& problems);

	private:
		struct TrieNode									//TrieNode struct
		{
			TrieNode* children[NUM_ACCEPTABLE_CHARS];
			bool isEndOfWord;
		};
		TrieNode* dict_root;							//root of the dictionary

		TrieNode* getNewNode();											//return a pointer to a new, empty node
		void insert(std::string word);									//insert a word
		bool find(TrieNode* root, std::string word);					//return if a word is in the dictionary
		TrieNode* getSubtree(TrieNode* root, std::string word); 		//return the pointer to the last letter in the word
		bool clean(std::string& word); 									//remove invalid characters, and return if the string still had >= 1 character
		char getChar(int index);										//get the character that would be at this index
		void clearTrie(TrieNode* root); 								//remove all the nodes from the tree (recursive function for destructor)
		void breakUp(const std::string& line, 			
			std::vector<SpellCheck::Position>& words);					//break up the line into words by putting their positions in the vector
};

#endif  // STUDENTSPELLCHECK_H_