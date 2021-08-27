#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cctype>

using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

	//Constructor
StudentSpellCheck::StudentSpellCheck() 
{
	dict_root = getNewNode();			//start off the intial node (which does not represent a real letter)
}

	//Destructor
StudentSpellCheck::~StudentSpellCheck() 
{
	clearTrie(dict_root);
}
	//remove all the nodes from the tree (recursive function for destructor)
void StudentSpellCheck::clearTrie(TrieNode* root)			
{
	if (root == nullptr)								//base case
		return; 

	for (int i = 0; i < NUM_ACCEPTABLE_CHARS; ++i)		//remove all of root's subtrees
		clearTrie(root->children[i]);

	delete root;										//remove root itself
}				

bool StudentSpellCheck::load(std::string dictionaryFile) 
{
	ifstream infile(dictionaryFile);				//try and open the file
	if ( ! infile)									//if could not open file
		return false;

	//if we could open the dictionary, load it
	clearTrie(dict_root); 									//clear the previous dictionary
	dict_root = getNewNode();

	string word;
	while (getline(infile, word))					//for each word in the file
	{
		if (clean(word) == true)					//if it still has characters after cleaning, load it
			insert(word);
	}

	return true;	
}

	//If word is spelled wrong, provide no more that max_suggestions number
	//or correctly spelled words that differ by no more than 1 letter.
bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) 
{
	string lowerCase = "";						//make word lower case
	for (int i = 0; i < word.length(); ++i)
		lowerCase += tolower(word[i]);
	word = lowerCase;

	if (find(dict_root, word) == true)			//if it's spelled correctly, just return true, otherwise, look for suggestions 
		return true;

	suggestions.clear();						//clear any old suggestions from previous runs

	for (int i = 0; i < word.length() && max_suggestions > 0; ++i)
	{
		string first = word.substr(0, i);								//get the first part of the word
		string second = word.substr(i + 1, word.length() - i - 1);		//and the second, while skipping one letter every run
		TrieNode* lookAfterThis = dict_root;

		if (i != 0)
			lookAfterThis = getSubtree(dict_root, first);								//get the end of the first part to look for endings

		if (lookAfterThis != nullptr)													//if there is a word with first as it's prefix
		{
			for (int k = 0; k < NUM_ACCEPTABLE_CHARS && max_suggestions > 0; ++k)		//for every character
				if (find(lookAfterThis, getChar(k) + second))							//if inserting that character between first and second is a valid word
				{
					suggestions.push_back(first + getChar(k) + second);					//add it to the list of suggestions
					max_suggestions--;
				}
		}
	}

	return false;
}

	//Spellcheck an entire line and put in problems the positions for the words that 
	//are misspelled
void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) 
{
	if (line.length() == 0)
		return;

	problems.clear(); 							//ensure that problems is empty
	std::vector<SpellCheck::Position> words;
	breakUp(line, words); 						//get the positions of the words in the line

	for (int i = 0; i < words.size(); ++i)		//for all the words
	{
		int startPos = (words[i]).start;
		int numChars = (words[i]).end - startPos + 1;
		if (find(dict_root, line.substr(startPos, numChars)) == false)		//if they are not in the dictionary
			problems.push_back(words[i]); 									//add their positions to problems vector+
	}
}

	//break up the line into words
	//by putting in words, the positions for each word in line
void StudentSpellCheck::breakUp(const std::string& line, std::vector<SpellCheck::Position>& words)
{
	int startPos = 0;

	for (int i = 0; i <= line.length(); i++)													//for every character (account for the word ending because the line is over)
	{
		if (i == line.length() || (isalpha(tolower(line[i])) == false && line[i] != '\''))		//if it's not a valid character, or if the word is ending since the line is ending
		{
			if (i != startPos)											//and there are other characters in the word
			{
				SpellCheck::Position w = SpellCheck::Position();		//put the start and end positions into the word vector
				w.start = startPos;
				w.end = i - 1;
				words.push_back(w);
			}
			startPos = i + 1;											//advance startPos to after the invalid character; i will be incremented by loop
		}
	}
}

	//return an empty, zeroed-out node
	//ASSUMES that ch is lower case
StudentSpellCheck::TrieNode* StudentSpellCheck::getNewNode()
{
	TrieNode* newNode = new TrieNode();
	
	for (int i = 0; i < NUM_ACCEPTABLE_CHARS; ++i)	//set all children to nullptr
		newNode->children[i] = nullptr;
	newNode->isEndOfWord = false;					//end of word is false by default

	return newNode;
}

	//insert a word into the dictionary
	//assumes the string being passed in has been through the clean function
void StudentSpellCheck::insert(string word)
{
	TrieNode* current = dict_root;
	for (int i = 0; i < word.length(); ++i)												//for every character in word
	{
		int letter_index = tolower(word[i]) == '\'' ? 
						   NUM_ACCEPTABLE_CHARS - 1: tolower(word[i]) - 'a';			//index of apostrophe is 26;

		if (current->children[letter_index] == nullptr)									//if a letter is not there
			current->children[letter_index] = getNewNode();								//add it in
		current = current->children[letter_index];										//keep going down the tree
	}
	current->isEndOfWord = true;														//mark where the word ends
}

	//return if word is in the dictionary
	//assumes the string being passed in has been through the clean function
bool StudentSpellCheck::find(TrieNode* root, string word)
{
	TrieNode* finalLetter = getSubtree(root, word);		//get a pointer to the last letter in the word in the trie
	if (finalLetter != nullptr)							//if that word is in the tree, return if the word ends at the letter specified (yes == word is in dictionary, no means not there)
		return finalLetter->isEndOfWord;
	return false;										//if finalLetter == nullptr, the word is definitely not in the dictionary
}

	//return the pointer to the last letter in the word (will be a nullptr if that word is not in the dictionary after the specified root)
StudentSpellCheck::TrieNode* StudentSpellCheck::getSubtree(TrieNode* root, string word)
{
	if (root == nullptr)
		return nullptr;

	TrieNode* current = root;

	for (int i = 0; i < word.length(); ++i)								//for every character in word (use lower case since trie's indices are calculated from lower case letters)
	{
		int letter_index = tolower(word[i]) == '\'' ? 
						   NUM_ACCEPTABLE_CHARS - 1: tolower(word[i]) - 'a';			//index of apostrophe is 26;

		if (current->children[letter_index] == nullptr)					//if that character does not have a valid pointer (meaning that word is not in the dictionary)
			return nullptr;							
		current = current->children[letter_index];						//otherwise, keep going down the tree
	}
	return current;														//return the pointer to the last letter (will be nullptr if this word isn't in the dictionary )
}	

	//remove any invalid characters in word, and return if it still has any characters left in it after the cleaning
bool StudentSpellCheck::clean(string& word)
{
	string cleanWord = "";

	for (int i = 0; i < word.length(); ++i)								//for every character
	{
		char current = tolower(word[i]);
		if (isalpha(current) || current == '\'')		//if its a letter or a apostrophe, add it in
			cleanWord += current;
	}

	word = cleanWord;
	return (cleanWord.length() > 0);									//return if our cleanWord still has characters in it
}

	//return the character that a certain index in the array will reprsent
	//either a an apostrophe, or a letter
char StudentSpellCheck::getChar(int index)
{
	if (index == (NUM_ACCEPTABLE_CHARS - 1))
		return '\'';
	return index + 'a'; 
}