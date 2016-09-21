/*--------
(1) Read in a Rainbow table (built using B.cpp)
(2) Read 5000 digests from standard input and  output the preimage.
------------*/

#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>

#include "sha1.h"

using namespace std;
unsigned long TOTAL_SHA = 0;       // Count the number of hashes performed.

const int maxChainLength = 147;
const int maxNumberOfChains = 85750;

const int maxDigestSize = 5;
const int maxWordSize = 3;

const int maxArraySize = 1024 * 512;

unsigned char wordArray[maxArraySize][maxWordSize];   // array to store the word read from the table (head of chain)
unsigned int digestArray[maxArraySize][maxDigestSize];   // array to store the digest read from the table  (end of chain)

//-------   Data Structure for searching    -----------//
std::unordered_map<unsigned int, int> hashTable;
std::unordered_map<unsigned int, int>::const_iterator hashTableIterator;

//-----------    Hash     ----------------------------//
void Hash(unsigned char word[maxWordSize], unsigned int digest[maxDigestSize])
{
	SHA1 sha;

	sha.Reset();

	sha.Input(word[0]);
	sha.Input(word[1]);
	sha.Input(word[2]);

	sha.Result(digest);
	TOTAL_SHA = TOTAL_SHA + 1;
}

//-----------    Reduce  -----------------------------//
//   d:   input digest
//   m:   output word
//   i:   the index of the reduce function 
//---------------------------------------------------//
void Reduce1(unsigned int digest[5], unsigned char word[3], int reductionIndex)
{
	word[0] = (unsigned char)((digest[2] + reductionIndex) % 256);   //8 bits
	word[1] = (unsigned char)((digest[4] + reductionIndex) % 256);   //8 bits
	word[2] = (unsigned char)((digest[3] + reductionIndex) % 256);   //8 bits
}

void Reduce2(unsigned int digest[5], unsigned char word[3], int reductionIndex)
{
	word[0] = (unsigned char)((digest[2] - reductionIndex) % 256);   //8 bits
	word[1] = (unsigned char)((digest[4] - reductionIndex) % 256);   //8 bits
	word[2] = (unsigned char)((digest[3] - reductionIndex) % 256);   //8 bits
}

void Reduce3(unsigned int digest[5], unsigned char word[3], int reductionIndex)
{
	word[0] = (unsigned char)((digest[1] + reductionIndex) % 256);   //8 bits
	word[1] = (unsigned char)((digest[2] + reductionIndex) % 256);   //8 bits
	word[2] = (unsigned char)((digest[3] + reductionIndex) % 256);   //8 bits
}

//------------  Read in the Table ------------------//
//   Store the result in M and D 
void ReadInRainbowTable(char* file_name)
{
	FILE *input_file = fopen(file_name, "r");
	int numOfEntriesRead = 0;

	for (int i = 0; i < maxNumberOfChains; i++)
	{
		fread(&(wordArray[i][0]), sizeof(unsigned char), 1, input_file);
		fread(&(wordArray[i][1]), sizeof(unsigned char), 1, input_file);
		fread(&(wordArray[i][2]), sizeof(unsigned char), 1, input_file);

		fread(&(digestArray[i][0]), sizeof(unsigned int), 1, input_file);

		hashTable.insert(std::pair<unsigned int, int>(digestArray[i][0], i));
		numOfEntriesRead++;
	}
	cout << "Number of Rainbow Table Entries Read: " << numOfEntriesRead << endl;

	fclose(input_file);
}


//-----------   reading the next digest from the standard input  ----------------//
void ReadNextDigest(unsigned int d[maxDigestSize], std::ifstream &infile)
{
	unsigned int digest_fragment;

	for (int i = 0; i < 5; i++)
	{
		infile >> std::hex >> digest_fragment;
		d[i] = digest_fragment;
	}

}

bool IsSameDigest(unsigned int d1[maxDigestSize], unsigned int d2[maxDigestSize])
{
	return (d1[0] == d2[0]);
}


bool TestCandidate(unsigned int d[maxDigestSize], unsigned char answer[maxWordSize])
{
	bool is_valid_candidate;

	int i = hashTable[d[0]];

	// find the head of the chain
	unsigned char curr_word[maxWordSize];

	curr_word[0] = wordArray[i][0];
	curr_word[1] = wordArray[i][1];
	curr_word[2] = wordArray[i][2];

	unsigned int curr_digest[maxDigestSize];

	for (int i = 0; i < maxChainLength; i++)
    {
        Hash(curr_word, curr_digest);
	if(i%3==0){
        Reduce1(curr_digest, curr_word, i);
	}
	else if(i%3==1){
	Reduce2(curr_digest, curr_word, i);
	}
	else{
	Reduce3(curr_digest,curr_word,i);
	}

        if (IsSameDigest(curr_digest, d))
        {
            is_valid_candidate = true;

            answer[0] = curr_word[0];
            answer[1] = curr_word[1];
            answer[2] = curr_word[2];
            break;
        }
    }
	return is_valid_candidate;
}

//------------------------------------------------------------------------------------
//      Given a digest,  search for the pre-image   answer_m[3].
//------------------------------------------------------------------------------------
bool Search1(unsigned int target_digest[maxDigestSize], unsigned char answer[maxWordSize])
{
	bool isFound = false;

	unsigned char colour_word[maxChainLength][maxWordSize];
	unsigned int colour_digest[maxChainLength][maxDigestSize];

	unsigned char curr_answer[maxWordSize];

	int i, j, k;

	// fills the starting point of the chain rows with the target digest
	for (j = 0; j < maxChainLength; j++)
	{
		colour_digest[j][0] = target_digest[0];
		colour_digest[j][1] = target_digest[1];
		colour_digest[j][2] = target_digest[2];
		colour_digest[j][3] = target_digest[3];
		colour_digest[j][4] = target_digest[4];
	}
	
	for (i = 0; i < maxChainLength; i++)
    {
        for (k = 0; k < i + 1; k++)
        {
            Reduce1(colour_digest[k], colour_word[k], i);
            Hash(colour_word[k], colour_digest[k]);
        }

        hashTableIterator = hashTable.find(colour_digest[i][0]);

        if (hashTableIterator != hashTable.end())
        {
            isFound = TestCandidate(colour_digest[i], curr_answer);

            // std::cout << curr_answer[0] << curr_answer[1] << curr_answer[2] << std::endl;
        }
	else{
		for (k = 0; k < i + 1; k++)
		{
		    Reduce2(colour_digest[k], colour_word[k], i);
		    Hash(colour_word[k], colour_digest[k]);
		}

		hashTableIterator = hashTable.find(colour_digest[i][0]);

		if (hashTableIterator != hashTable.end())
		{
		    isFound = TestCandidate(colour_digest[i], curr_answer);

		    // std::cout << curr_answer[0] << curr_answer[1] << curr_answer[2] << std::endl;
		}
		else{
			for (k = 0; k < i + 1; k++)
			{
			    Reduce3(colour_digest[k], colour_word[k], i);
			    Hash(colour_word[k], colour_digest[k]);
			}

			hashTableIterator = hashTable.find(colour_digest[i][0]);

			if (hashTableIterator != hashTable.end())
			{
			    isFound = TestCandidate(colour_digest[i], curr_answer);

			    // std::cout << curr_answer[0] << curr_answer[1] << curr_answer[2] << std::endl;
			}
		}
		
	}
	if (isFound)
            {
                answer[0] = curr_answer[0];
                answer[1] = curr_answer[1];
                answer[2] = curr_answer[2];
            }
    }
	return isFound;
}

void SearchDigestFile(char* digest_file_name)
{
	unsigned int d[maxDigestSize];
	unsigned char curr_answer[maxWordSize];

	//--------  PROJECT  INPUT/OUTPUT FORMAT ----------------//
	int total_digests_found = 0;
	int total_digests_not_found = 0;

	std::ifstream digest_file(digest_file_name, std::ios::binary);

	std::cout.setf(std::ios::hex, std::ios::basefield);  //   setting display to Hexdecimal format.  (this is the irritating part of using C++).
	std::cout.setf(std::ios::uppercase);

	for (int i = 0; i < 5000; i++)
	{
		ReadNextDigest(d, digest_file);

		if (Search1(d, curr_answer) == true)
		{
			total_digests_found++;
			//------   print the word in hexdecimal format   -----------
			std::cout << std::setw(1) << (unsigned int)curr_answer[0] / 16;
			std::cout << std::setw(1) << (unsigned int)curr_answer[0] % 8;
			std::cout << std::setw(1) << (unsigned int)curr_answer[1] / 16;
			std::cout << std::setw(1) << (unsigned int)curr_answer[1] % 8;
			std::cout << std::setw(1) << (unsigned int)curr_answer[2] / 16;
			std::cout << std::setw(1) << (unsigned int)curr_answer[2] % 8 << std::endl;

		}
		else
		{
			total_digests_not_found++;
			std::cout << std::setw(6) << 0 << std::endl;
		}
	}

	digest_file.close();

	std::cout.setf(std::ios::dec);
	cout << "Accuracy       C is: " << total_digests_found / 5000.0 << endl;
	cout << "Speedup factor F is: " << (5000.0 / TOTAL_SHA) * 8388608 << endl;
	cout << "Number of digests not found " << total_digests_not_found << endl;
}

int main(int argc, char* argv[])
{
	SHA1 sha;
	//------------ R E A D     R A I N B O W    T A B L E  --------//
	ReadInRainbowTable(argv[1]);
	cout << "READ DONE" << endl;

	SearchDigestFile(argv[2]);

	return 0;
}

