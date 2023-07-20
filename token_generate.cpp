#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

std::string generateRandomSequence() {
    const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*+=|?><";
    const int length = 16;

    std::string randomSequence;
    randomSequence.reserve(length);

    // Set the seed for the random number generator
    

    for (int i = 0; i < length; ++i) {
        int randomIndex = std::rand() % characters.length();
        randomSequence += characters[randomIndex];
    }

    return randomSequence;
}

int main() 
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 0; i < 10; i++)
    {

        std::string randomSequence = generateRandomSequence();
        std::cout << "Random sequence: " << randomSequence << std::endl;
    }

    return 0;
}
