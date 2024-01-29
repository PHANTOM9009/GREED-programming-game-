#include <iostream>
#include <conio.h> // For _getch()

int main() {
    const int MAX_PASSWORD_LENGTH = 20;
    char password[MAX_PASSWORD_LENGTH + 1]; // +1 for null terminator

    std::cout << "Enter your password: ";

   

    int i = 0;
    char ch;
    while (i < MAX_PASSWORD_LENGTH) {
        ch = _getch();

        if (ch == 13 || ch == 10) { // Enter key
            break;
        }

        std::cout << '*';
        password[i++] = ch;
    }

    password[i] = '\0'; // Null-terminate the password

    std::cout << "\nPassword entered: " << password << std::endl;

    return 0;
}
