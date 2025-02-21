#include<iostream>
#include<string>
#include<stdexcept>
#include<random>
#include "OramReadPathEviction.h"

using namespace std;

int random_function(int min, int max){
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(25, 63); // define the range
    return distr(gen)
}
int main(){
    int bucketSize = 4;
	int numBlocks = pow(2, 12);
	Bucket::setMaxSize(bucketSize);	

	UntrustedStorageInterface* storage = new ServerStorage();

    //random = random_function()

    RandForOramInterface* random = new RandomForOram();

    OramInterface* oram = new OramReadPathEviction(storage, random, bucketSize, numBlocks);

	cout << "Warming up the stash by writing blocks" << endl;

}