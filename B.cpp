/*----------------------------------
This program build the rainbow table  for the other program F.cpp.
------------------------------------*/

#include <iostream>
#include <unordered_map>
#include "sha1.h"
#include <time.h>
#include <fstream>
#include <string.h>
#include <random>

using namespace std;


//  A table to store all the words and digests. 
//    infeasible to have such large table in practice.   
//    for programming convenient, we store the whole table in memory. 
unsigned char wordArray[1048576][3];  
unsigned int  digestArray[1048576][5];

unordered_map <unsigned long, unsigned int> HashTable;
unordered_map <unsigned long, unsigned int>::const_iterator hashtableIterator;
const int maxDigestSize = 5;
const int maxWordSize = 3;
const int maxNumberOfChainsPerTable = 65536;
const int maxLengthOfEachChain = 256;


//----  return the next word to be considered 
void nextWord( unsigned char word[maxWordSize])
{
	memset(word, 0, 3);
	word[0] = (unsigned char)(rand() % 256);
	word[1] = (unsigned char)(rand() % 256);
	word[2] = (unsigned char)(rand() % 256);
}


//-------   Hash 
int Hash (unsigned char word[maxWordSize], unsigned int digest[maxDigestSize] )
{
   SHA1 sha;
   sha.Reset(); 
   sha.Input(word[0]); 
   sha.Input(word[1]); 
   sha.Input(word[2]); 
   sha.Result(digest);
   return(0);
}

//-------  Reduce
int Reduce1 (unsigned int digest[maxDigestSize], unsigned char word[maxWordSize],  int reductionIndex )
{
   word[0]=(unsigned char)( (digest[0] + reductionIndex ) %256); //8 bits
   word[1]=(unsigned char)( (digest[1]   ) %256);   //8 bits 
   word[2]=(unsigned char)( (digest[2]   ) %256);   //8 bits

   return(0);
}

int Reduce2 (unsigned int digest[maxDigestSize], unsigned char word[maxWordSize],  int reductionIndex )
{
   word[0]=(unsigned char)( (digest[0]  - reductionIndex  ) %256); //8 bits
   word[1]=(unsigned char)( (digest[1]   ) %256);   //8 bits 
   word[2]=(unsigned char)( (digest[2]   ) %256);   //8 bits

   return(0);
}

int Reduce3 (unsigned int digest[maxDigestSize], unsigned char word[maxWordSize],  int reductionIndex )
{
   word[0]=(unsigned char)( (digest[0]   ) %256); //8 bits
   word[1]=(unsigned char)( (digest[1]   ) %256);   //8 bits 
   word[2]=(unsigned char)( (digest[2] + reductionIndex   ) %256);   //8 bits

   return(0);
}

int buildT1()
{
  unsigned int  digest[maxDigestSize];
  unsigned char word[maxWordSize];

  srand(time(NULL));

  FILE *outfile = fopen("rainbowtable.data", "w");

  for (int i=0;i < maxNumberOfChainsPerTable; i++)
    {
        nextWord (word);
        cout << "Table 1, building chain number "<< i << endl;
		
		//store random generated word into array
		wordArray[i][0] = word[0];
		wordArray[i][1] = word[1];
		wordArray[i][2] = word[2];

		//building chain
		for (int j = 0; j < maxLengthOfEachChain; j++) {
			Hash(word, digest);
			Reduce1(digest, word, j);
		}

		//Checking if chain has been generated. Store if haven't generate. Or make a new random word if generated
		hashtableIterator = HashTable.find(digest[0]);

		if (hashtableIterator != HashTable.end()) {
			i--;
		} else {
			HashTable[digest[0]] = 1;

			digestArray[i][0] = digest[0];
			digestArray[i][1] = digest[1];
			digestArray[i][2] = digest[2];
			digestArray[i][3] = digest[3];
			digestArray[i][4] = digest[4];
		}

    }



    //---    Write to the output file
    //note that to reduce the size of the table, it is not neccessary to write the full digest.

    for (long k=0; k < maxNumberOfChainsPerTable; k++)
      {
      fwrite (&(wordArray[k][0]),  sizeof( unsigned char), 1, outfile);
      fwrite (&(wordArray[k][1]),  sizeof( unsigned char), 1, outfile);
      fwrite (&(wordArray[k][2]),  sizeof( unsigned char), 1, outfile);

      fwrite (&(digestArray[k][0]),  sizeof( unsigned int), 1, outfile);

      }
      fclose(outfile);
  return(0);
}

int buildT2()
{
  unsigned int  digest[maxDigestSize];
  unsigned char word[maxWordSize];

  srand(time(NULL));

  FILE *outfile = fopen("rainbowtable.data", "a");

  for (int i=0;i < maxNumberOfChainsPerTable; i++)
    {
        nextWord (word);
        cout << "Table 2, building chain number "<< i << endl;
		
		//store random generated word into array
		wordArray[i][0] = word[0];
		wordArray[i][1] = word[1];
		wordArray[i][2] = word[2];

		//building chain
		for (int j = 0; j < maxLengthOfEachChain; j++) {
			Hash(word, digest);
			Reduce2(digest, word, j);
		}

		//Checking if chain has been generated. Store if haven't generate. Or make a new random word if generated
		hashtableIterator = HashTable.find(digest[0]);

		if (hashtableIterator != HashTable.end()) {
			i--;
		} else {
			HashTable[digest[0]] = 1;

			digestArray[i][0] = digest[0];
			digestArray[i][1] = digest[1];
			digestArray[i][2] = digest[2];
			digestArray[i][3] = digest[3];
			digestArray[i][4] = digest[4];
		}

    }



    //---    Write to the output file
    //note that to reduce the size of the table, it is not neccessary to write the full digest.

    for (long k=0; k < maxNumberOfChainsPerTable; k++)
      {
      fwrite (&(wordArray[k][0]),  sizeof( unsigned char), 1, outfile);
      fwrite (&(wordArray[k][1]),  sizeof( unsigned char), 1, outfile);
      fwrite (&(wordArray[k][2]),  sizeof( unsigned char), 1, outfile);

      fwrite (&(digestArray[k][0]),  sizeof( unsigned int), 1, outfile);

      }
      fclose(outfile);
  return(0);
}

int buildT3()
{
  unsigned int  digest[maxDigestSize];
  unsigned char word[maxWordSize];

  srand(time(NULL));

  FILE *outfile = fopen("rainbowtable.data", "a");

  for (int i=0;i < maxNumberOfChainsPerTable; i++)
    {
        nextWord (word);
        cout << "Table 3, building chain number "<< i << endl;
		
		//store random generated word into array
		wordArray[i][0] = word[0];
		wordArray[i][1] = word[1];
		wordArray[i][2] = word[2];

		//building chain
		for (int j = 0; j < maxLengthOfEachChain; j++) {
			Hash(word, digest);
			Reduce3(digest, word, j);
		}

		//Checking if chain has been generated. Store if haven't generate. Or make a new random word if generated
		hashtableIterator = HashTable.find(digest[0]);

		if (hashtableIterator != HashTable.end()) {
			i--;
		} else {
			HashTable[digest[0]] = 1;

			digestArray[i][0] = digest[0];
			digestArray[i][1] = digest[1];
			digestArray[i][2] = digest[2];
			digestArray[i][3] = digest[3];
			digestArray[i][4] = digest[4];
		}

    }



    //---    Write to the output file
    //note that to reduce the size of the table, it is not neccessary to write the full digest.

    for (long k=0; k < maxNumberOfChainsPerTable; k++)
      {
      fwrite (&(wordArray[k][0]),  sizeof( unsigned char), 1, outfile);
      fwrite (&(wordArray[k][1]),  sizeof( unsigned char), 1, outfile);
      fwrite (&(wordArray[k][2]),  sizeof( unsigned char), 1, outfile);

      fwrite (&(digestArray[k][0]),  sizeof( unsigned int), 1, outfile);

      }
      fclose(outfile);
  return(0);
}

int main( int argc, char*argv[])
{

    SHA1 sha;

    //----  Setting the parameters
   
    //----   Build the table.
    buildT1();
    buildT2();
    //buildT3();
    cout<<"Table Built. Exiting program now" <<endl;;
	exit(0);
}

