//
//  main.cpp
//  spell-correct
//
//  Created by Manju Balasubramaniam on 1/17/15.
//  Copyright (c) 2015 Manju Balasubramaniam. All rights reserved.
//

#ifndef _SPELLCORRECTOR_H_
#define _SPELLCORRECTOR_H_

#include <vector>
#include
<tr1/unordered_map>

class SpellCorrector
{
private:
    typedef std::vector<std::string> Vector;
    typedef std::tr1::unordered_map<std::string, int> Dictionary;
    
    Dictionary dictionary;
    
    void edits(const std::string& word, Vector& result);
    void known(Vector& results, Dictionary& candidates);
public:
    void load(const std::string& filename);
    std::string correct(const std::string& word);
};

#endif

#include <fstream>
#include <string>
#include <algorithm>
#include <iostream>

#include "SpellCorrector.h"

using namespace std;

bool sortBySecond(const pair<std::string, int>& left, const pair<std::string, int>& right)
{
    return left.second < right.second;
}

char filterNonAlphabetic(char& letter)
{
    if (isalpha(letter))
        return tolower(letter);
    return '-';
}

void SpellCorrector::load(const std::string& filename)
{
    ifstream file(filename.c_str(), ios_base::binary | ios_base::in);
    
    file.seekg(0, ios_base::end);
    int length = file.tellg();
    file.seekg(0, ios_base::beg);
    
    string line(length, '0');
    
    file.read(&line[0], length);
    
    transform(line.begin(), line.end(), line.begin(), filterNonAlphabetic);
    
    string::size_type begin = 0;
    string::size_type end   = line.size();
    
    for (string::size_type i = 0;;)
    {
        while (line[++i] == '-' && i < end); //find first '-' character
        
        if (i >= end) { break; }
        
        begin = i;
        
        while (line[++i] != '-' && i < end); //find first not of '-' character
        
        dictionary[line.substr(begin, i - begin)]++;
    }
}

string SpellCorrector::correct(const std::string& word)
{
    Vector result;
    Dictionary candidates;
    
    if (dictionary.find(word) != dictionary.end()) { return word; }
    
    edits(word, result);
    known(result, candidates);
    
    if (candidates.size() > 0) { return max_element(candidates.begin(), candidates.end(), sortBySecond)->first; }
    
    for (unsigned int i = 0;i < result.size();i++)
    {
        Vector subResult;
        
        edits(result[i], subResult);
        known(subResult, candidates);
    }
    
    if (candidates.size() > 0) { return max_element(candidates.begin(), candidates.end(), sortBySecond)->first; }
    
    return "";
}

void SpellCorrector::known(Vector& results, Dictionary& candidates)
{
    Dictionary::iterator end = dictionary.end();
    
    for (unsigned int i = 0;i < results.size();i++)
    {
        Dictionary::iterator value = dictionary.find(results[i]);
        
        if (value != end) candidates[value->first] = value->second;
    }
}

void SpellCorrector::edits(const std::string& word, Vector& result)
{
    for (string::size_type i = 0;i < word.size();    i++) result.push_back(word.substr(0, i)             + word.substr(i + 1)); //deletions
    for (string::size_type i = 0;i < word.size() - 1;i++) result.push_back(word.substr(0, i) + word[i+1] + word.substr(i + 2)); //transposition
    
    for (char j = 'a';j <= 'z';++j)
    {
        for (string::size_type i = 0;i < word.size();    i++) result.push_back(word.substr(0, i) + j + word.substr(i + 1)); //alterations
        for (string::size_type i = 0;i < word.size() + 1;i++) result.push_back(word.substr(0, i) + j + word.substr(i)     ); //insertion
    }
}
/* I am working in some optimizations yet but it should be ready to use. Using the corrector is pretty easy, just download the words database, and see the exampl

 */
#include "SpellCorrector.h"

using namespace std;

int main()
{
    SpellCorrector corrector;
    
    corrector.load("big.txt");
    
    string request;
    while (request != "quit")
    {
        cout << "Type one word\n";
        cin >> request;
        
        string correct(corrector.correct(request));
        
        if (correct != "")
            cout << "Did you mean: " << correct << "?\n\n\n";
        else
            cout << "No corrections avaiable\n\n\n";
    }
    
    return 0;
}

