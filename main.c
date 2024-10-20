#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


struct User {
    char fullName[100];
    char username[30];
    char email[50];
    char phone[15];
    char accountNumber[15]; // Fixed typo in variable name
    char address[50];
    char nidNo[15];
    float balance; // Changed to float for balance
    char password[30];
};

// Function Prototypes
void registerUser();
int loginUser(char* loggedInUsername);
void transferMoney(const char* senderAccountNo);
void checkBalance(const char* username);
void depositMoney(const char* username);
void withdrawMoney(const char* username);
void editDetails(const char* username);
void deleteAccount(const char* username);
void logout(char* loggedInUsername);
void aboutBank();
void loggedInMenu(char* loggedInUsername);

// Register User Function
void registerUser() {
    FILE *file;
    struct User user;
    char line[256];
    int usernameExists = 0, emailExists = 0;

    printf("Enter full name: ");
    fgets(user.fullName, sizeof(user.fullName), stdin);
    strtok(user.fullName, "\n");

    printf("Enter username: ");
    fgets(user.username, sizeof(user.username), stdin);
    strtok(user.username, "\n");

    printf("Enter email: ");
    fgets(user.email, sizeof(user.email), stdin);
    strtok(user.email, "\n");

    printf("Enter phone number: ");
    fgets(user.phone, sizeof(user.phone), stdin);
    strtok(user.phone, "\n");

    printf("Enter account number: ");
    fgets(user.accountNumber, sizeof(user.accountNumber), stdin);
    strtok(user.accountNumber, "\n");

    printf("Enter address: ");
    fgets(user.address, sizeof(user.address), stdin);
    strtok(user.address, "\n");

    printf("Enter NID No: ");
    fgets(user.nidNo, sizeof(user.nidNo), stdin);
    strtok(user.nidNo, "\n");

    printf("Enter initial deposit amount: ");
    scanf("%f", &user.balance); // Changed to float for initial deposit
    getchar(); // Clear newline character

    printf("Enter password: ");
    fgets(user.password, sizeof(user.password), stdin);
    strtok(user.password, "\n");

    file = fopen("users.csv", "r");
    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            char existingUsername[30], existingEmail[50];
            sscanf(line, "%*[^,],%[^,],%[^,]", existingUsername, existingEmail);

            if (strcmp(existingUsername, user.username) == 0) {
                usernameExists = 1;
            }
            if (strcmp(existingEmail, user.email) == 0) {
                emailExists = 1;
            }
        }
        fclose(file);
    }

    if (usernameExists) {
        printf("\nUsername already exists. Please choose a different username.\n");
        return;
    }
    if (emailExists) {
        printf("Email already exists. Please choose a different email.\n");
        return;
    }

    file = fopen("users.csv", "a");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fprintf(file, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n", user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, user.balance, user.password);
    fclose(file);
    printf("\nRegistration successful!\n\n");
}

// Login User Function
int loginUser(char* loggedInUsername) {
    FILE *file;
    struct User user;
    char username[30], password[30];

    file = fopen("users.csv", "r");
    if (file == NULL) {
        printf("No users registered yet. Please register first.\n");
        return 0;
    }

    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    strtok(username, "\n");

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    strtok(password, "\n");

    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, username) == 0 && strcmp(user.password, password) == 0) {
            strcpy(loggedInUsername, username);
            fclose(file);
            printf("\nLogin Successful!\n");
            return 1;
        }
    }

    fclose(file);
    printf("Invalid username or password. Please try again.\n\n");
    return 0;
}

// Function to log a transaction
void logTransaction(const char* username, const char* type, float amount) {
    FILE *file = fopen("transactions.csv", "a");
    if (file == NULL) {
        printf("Error opening transaction log file!\n");
        return;
    }

    // Get current time
    time_t t;
    time(&t);
    char* timeStr = ctime(&t);
    timeStr[strlen(timeStr) - 1] = '\0'; // Remove newline character

    // Write transaction to file
    fprintf(file, "%s,%s,%.2f,%s\n", type, username, amount, timeStr);
    fclose(file);
}

// View Statement Function
void viewStatement(const char* username) {
    FILE *file = fopen("transactions.csv", "r");
    if (file == NULL) {
        printf("No transactions found.\n");
        return;
    }

    printf("\nTransaction Statement for %s:\n", username);
    printf("--------------------------------------------------\n");
    printf("Type           | Amount   | Date & Time\n");
    printf("--------------------------------------------------\n");

    char type[20];
    float amount;
    char date[100];

    while (fscanf(file, "%[^,],%[^,],%f,%[^\n]\n", type, username, &amount, date) == 4) {
        printf("%-15s | %.2f    | %s\n", type, amount, date);
    }

    fclose(file);
}

// Transfer Money Function
// Transfer Money Function
void transferMoney(const char* senderUsername) {
    FILE *file, *tempFile;
    struct User user;
    char recipientAccountNo[15];
    float amount;
    int senderFound = 0, recipientFound = 0;

    printf("Enter recipient account number: ");
    fgets(recipientAccountNo, sizeof(recipientAccountNo), stdin);
    strtok(recipientAccountNo, "\n");  // Remove newline character

    printf("Enter amount to transfer: ");
    scanf("%f", &amount);
    getchar(); // Clear newline character

    if (amount <= 0) {
        printf("Amount should be greater than zero.\n");
        return;
    }

    // Open user data file for reading
    file = fopen("users.csv", "r");
    if (file == NULL) {
        printf("No users registered yet.\n");
        return;
    }

    // Check if both sender and recipient accounts exist
    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, senderUsername) == 0) {
            senderFound = 1;
            // Check if sender has enough balance
            if (amount > user.balance) {
                printf("Insufficient balance for this transfer.\n");
                fclose(file);
                return;
            }
        } else if (strcmp(user.accountNumber, recipientAccountNo) == 0) {
            recipientFound = 1;
        }
    }
    fclose(file);

    // If recipient not found, stop the process
    if (!recipientFound) {
        printf("Recipient account not found.\n");
        return;
    }

    // Now proceed with updating the balances
    file = fopen("users.csv", "r");
    tempFile = fopen("temp.csv", "w");
    if (file == NULL || tempFile == NULL) {
        printf("Error opening file!\n");
        return;
    }

    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, senderUsername) == 0) {
            // Deduct the amount from the sender's balance
            user.balance -= amount;
            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n", user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, user.balance, user.password);
        } else if (strcmp(user.accountNumber, recipientAccountNo) == 0) {
            // Add the amount to the recipient's balance
            user.balance += amount;
            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n", user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, user.balance, user.password);
        } else {
            // Write other users' data unchanged
            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n", user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, user.balance, user.password);
        }
    }

    fclose(file);
    fclose(tempFile);

    // Replace old file with updated file
    remove("users.csv");
    rename("temp.csv", "users.csv");

    // Log transactions if successful
    logTransaction(senderUsername, "Transfer Out", amount); // Log sender's transaction
    logTransaction(recipientAccountNo, "Transfer In", amount); // Log recipient's transaction
    printf("Transfer successful!\n");
}





// Check Balance Function
void checkBalance(const char* username) {
    FILE *file;
    struct User user;

    file = fopen("users.csv", "r");
    if (file == NULL) {
        printf("No users registered yet.\n");
        return;
    }

    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, username) == 0) {
            printf("Current Balance for %s: %.2f\n", username, user.balance);
            fclose(file);
            return;
        }
    }

    fclose(file);
    printf("User not found.\n");
}

// Deposit Money Function starts here
void depositMoney(const char* username) {
    FILE *file;
    struct User user;
    float amount;

    file = fopen("users.csv", "r+");
    if (file == NULL) {
        printf("No users registered yet.\n");
        return;
    }

    int found = 0; // Flag to check if user is found
    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, username) == 0) {
            found = 1;
            printf("Enter amount to deposit: ");
            scanf("%f", &amount);
            getchar(); // Clear newline character

            if (amount <= 0) {
                printf("Amount should be greater than zero.\n");
                fclose(file);
                return;
            }

            user.balance += amount;

            // Create a temporary file to write updated data
            FILE *tempFile = fopen("temp.csv", "w");
            if (tempFile == NULL) {
                printf("Error opening temporary file!\n");
                fclose(file);
                return;
            }

            // Write updated data back to the temporary file
            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n",
                user.fullName, user.username, user.email, user.phone,
                user.accountNumber, user.address, user.nidNo, user.balance, user.password);

            // Write remaining users to the temporary file
            while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                          user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
                fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n",
                    user.fullName, user.username, user.email, user.phone,
                    user.accountNumber, user.address, user.nidNo, user.balance, user.password);
            }

            fclose(file);
            fclose(tempFile);

            remove("users.csv");
            rename("temp.csv", "users.csv");

            logTransaction(username, "Deposit", amount); // Log the deposit transaction
            printf("Deposit successful!\n");
            return;
        }
    }

    fclose(file);
    if (!found) {
        printf("User not found.\n");
    }
}


// Deposit Money Function ends here
// Withdraw Money Function
void withdrawMoney(const char* username) {
    FILE *file;
    struct User user;
    float amount;

    file = fopen("users.csv", "r+");
    if (file == NULL) {
        printf("No users registered yet.\n");
        return;
    }

    int found = 0; // Flag to check if user is found
    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, username) == 0) {
            found = 1;
            printf("Current balance: %.2f\n", user.balance);
            printf("Enter amount to withdraw: ");
            scanf("%f", &amount);
            getchar(); // Clear newline character

            if (amount <= 0) {
                printf("Amount should be greater than zero.\n");
                fclose(file);
                return;
            }

            if (amount > user.balance) {
                printf("Insufficient balance for this withdrawal.\n");
                fclose(file);
                return;
            }

            // Update the balance
            user.balance -= amount;

            // Create a temporary file to write updated data
            FILE *tempFile = fopen("temp.csv", "w");
            if (tempFile == NULL) {
                printf("Error opening temporary file!\n");
                fclose(file);
                return;
            }

            // Write updated data back to the temporary file
            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n",
                user.fullName, user.username, user.email, user.phone,
                user.accountNumber, user.address, user.nidNo, user.balance, user.password);

            // Write remaining users to the temporary file
            while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                          user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
                fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n",
                    user.fullName, user.username, user.email, user.phone,
                    user.accountNumber, user.address, user.nidNo, user.balance, user.password);
            }

            fclose(file);
            fclose(tempFile);

            // Replace the old user file with the updated one
            remove("users.csv");
            rename("temp.csv", "users.csv");

            // Log the withdrawal transaction
            logTransaction(username, "Withdraw", amount); // Log the withdrawal transaction

            // Display the new balance after withdrawal
            printf("Withdrawal successful! \n");  // This should now show the correct balance
            return;
        }
    }

    fclose(file);
    if (!found) {
        printf("User not found.\n");
    }
}



// Edit Details Function
void editDetails(const char* username) {
    FILE *file;
    struct User user;
    char line[256];

    file = fopen("users.csv", "r+");
    if (file == NULL) {
        printf("No users registered yet.\n");
        return;
    }

    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, username) == 0) {
            printf("Edit your details (leave blank to keep unchanged):\n");

            printf("Full name (%s): ", user.fullName);
            char newFullName[100];
            fgets(newFullName, sizeof(newFullName), stdin);
            if (strcmp(newFullName, "\n") != 0) {
                strtok(newFullName, "\n");
                strcpy(user.fullName, newFullName);
            }

            printf("Email (%s): ", user.email);
            char newEmail[50];
            fgets(newEmail, sizeof(newEmail), stdin);
            if (strcmp(newEmail, "\n") != 0) {
                strtok(newEmail, "\n");
                strcpy(user.email, newEmail);
            }

            printf("Phone number (%s): ", user.phone);
            char newPhone[15];
            fgets(newPhone, sizeof(newPhone), stdin);
            if (strcmp(newPhone, "\n") != 0) {
                strtok(newPhone, "\n");
                strcpy(user.phone, newPhone);
            }

            printf("Address (%s): ", user.address);
            char newAddress[50];
            fgets(newAddress, sizeof(newAddress), stdin);
            if (strcmp(newAddress, "\n") != 0) {
                strtok(newAddress, "\n");
                strcpy(user.address, newAddress);
            }

            printf("NID No (%s): ", user.nidNo);
            char newNidNo[15];
            fgets(newNidNo, sizeof(newNidNo), stdin);
            if (strcmp(newNidNo, "\n") != 0) {
                strtok(newNidNo, "\n");
                strcpy(user.nidNo, newNidNo);
            }

            printf("Password (leave blank to keep unchanged): ");
            char newPassword[30];
            fgets(newPassword, sizeof(newPassword), stdin);
            if (strcmp(newPassword, "\n") != 0) {
                strtok(newPassword, "\n");
                strcpy(user.password, newPassword);
            }

            // Update user data in the file
            rewind(file); // Move to the beginning of the file
            FILE *tempFile = fopen("temp.csv", "w");
            if (tempFile == NULL) {
                printf("Error opening temporary file!\n");
                fclose(file);
                return;
            }

            // Write updated data back to the file
            while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                          user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
                if (strcmp(user.username, username) == 0) {
                    fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n", user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, user.balance, user.password);
                } else {
                    fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n", user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, user.balance, user.password);
                }
            }

            fclose(file);
            fclose(tempFile);
            remove("users.csv");
            rename("temp.csv", "users.csv");

            printf("Details updated successfully.\n");
            return;
        }
    }

    fclose(file);
    printf("User not found.\n");
}

// Delete Account Function
void deleteAccount(const char* username) {
    FILE *file, *tempFile;
    struct User user;
    int found = 0;

    file = fopen("users.csv", "r");
    tempFile = fopen("temp.csv", "w");

    if (file == NULL || tempFile == NULL) {
        printf("Error opening files!\n");
        return;
    }

    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, username) == 0) {
            found = 1;
        } else {
            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n", user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, user.balance, user.password);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove("users.csv");
        rename("temp.csv", "users.csv");
        printf("Account deleted successfully.\n");
    } else {
        remove("temp.csv");
        printf("Account not found.\n");
    }
}

// Logout Function
void logout(char* loggedInUsername) {
    strcpy(loggedInUsername, "");
    printf("You have logged out.\n");
}

// About Bank Function
void aboutBank() {
    printf("Bank Name: Fraud Bank\n");
    printf("Manager: Opi Sharma\n");
    printf("Head Office: BUBT\n");
}

// Logged-in Menu Function
void loggedInMenu(char* loggedInUsername) {
    int choice;
    do {
        printf("\n--- User Menu ---\n");
        printf("1. Transfer Money\n"
               "2. Check Balance\n"
               "3. Deposit Money\n"
               "4. Withdraw Money\n"
               "5. Statement\n"
               "6. Edit Details\n"
               "7. Delete Account\n"
               "8. About Bank\n"
               "9. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear the newline character

        switch (choice) {
            case 1:
                transferMoney(loggedInUsername);
                break;
            case 2:
                checkBalance(loggedInUsername);
                break;
            case 3:
                depositMoney(loggedInUsername);
                break;
            case 4:
                withdrawMoney(loggedInUsername);
                break;
            case 5:
                viewStatement(loggedInUsername);
                break;
            case 6:
                editDetails(loggedInUsername);
                break;
            case 7:
                deleteAccount(loggedInUsername);
                strcpy(loggedInUsername, ""); // Log the user out after deleting the account
                return;
            case 8:
                aboutBank();
                break;
            case 9:
                logout(loggedInUsername);
                return;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    } while (strlen(loggedInUsername) > 0);
}

// Main Function
int main() {
    char loggedInUsername[30] = "";
    int choice;

    do {
        printf("\n--- Main Menu ---\n");
        printf("1. Register\n2. Login\n3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear the newline character

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                if (loginUser(loggedInUsername)) {
                    printf("\nWelcome, %s!\n", loggedInUsername);
                    loggedInMenu(loggedInUsername); // Show the menu after successful login
                }
                break;
            case 3:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    } while (choice != 3);

    return 0;
}
