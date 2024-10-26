#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


struct User {
    char fullName[100];
    char username[30];
    char email[50];
    char phone[15];
    char accountNumber[15];
    char address[50];
    char nidNo[15];
    float balance;
    char password[30];
};
void registerUser();
int loginUser(char* loggedInUsername);
void transferMoney(const char* senderAccountNo);
void checkBalance(const char* username);
void depositMoney(const char* username);
void withdrawMoney(const char* username);
void deleteAccount(const char* username);
void logout(char* loggedInUsername);
void aboutBank();
void loggedInMenu(char* loggedInUsername);

//register function starts here
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
    scanf("%f", &user.balance);
    getchar();

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
//register function ends here


//login function starts here
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
//register function ends here

//logTransaction function starts here
void logTransaction(const char* username, const char* type, float amount) {
    FILE *file = fopen("transactions.csv", "a");
    if (file == NULL) {
        printf("Error opening transaction log file!\n");
        return;
    }

    time_t t;
    time(&t);
    char* timeStr = ctime(&t);
    timeStr[strlen(timeStr) - 1] = '\0';

    fprintf(file, "%s,%s,%.2f,%s\n", type, username, amount, timeStr);
    fclose(file);
}
//logTransaction function ends here


//viewStatement function starts here
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
    char fileUsername[20];
    float amount;
    char date[100];

    int found = 0;

    while (fscanf(file, "%[^,],%[^,],%f,%[^\n]\n", type, fileUsername, &amount, date) == 4) {
        if (strcmp(fileUsername, username) == 0) {
            printf("%-15s | %.2f    | %s\n", type, amount, date);
            found = 1;
        }
    }

    if (!found) {
        printf("No transactions found for %s.\n", username);
    }

    fclose(file);
}
//viewStatement function ends here


//transferMoney function starts here
void transferMoney(const char* senderUsername) {
    FILE *file, *tempFile;
    struct User user;
    char recipientAccountNo[15];
    float amount;
    int senderFound = 0, recipientFound = 0;

    printf("Enter recipient account number: ");
    fgets(recipientAccountNo, sizeof(recipientAccountNo), stdin);
    strtok(recipientAccountNo, "\n");

    printf("Enter amount to transfer: ");
    scanf("%f", &amount);
    getchar();

    if (amount <= 0) {
        printf("Amount should be greater than zero.\n");
        return;
    }

    file = fopen("users.csv", "r");
    if (file == NULL) {
        printf("No users registered yet.\n");
        return;
    }

    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, senderUsername) == 0) {
            senderFound = 1;
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

    if (!recipientFound) {
        printf("Recipient account not found.\n");
        return;
    }

    file = fopen("users.csv", "r");
    tempFile = fopen("temp.csv", "w");
    if (file == NULL || tempFile == NULL) {
        printf("Error opening file!\n");
        return;
    }

    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, senderUsername) == 0) {
            user.balance -= amount;
            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n", user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, user.balance, user.password);
        } else if (strcmp(user.accountNumber, recipientAccountNo) == 0) {
            user.balance += amount;
            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n", user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, user.balance, user.password);
        } else {
            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n", user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, user.balance, user.password);
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("users.csv");
    rename("temp.csv", "users.csv");

    logTransaction(senderUsername, "Transfer Out", amount);
    logTransaction(recipientAccountNo, "Transfer In", amount);
    printf("Transfer successful!\n");
}
//transferMoney function ends here


//checkBalance function starts here
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
//checkBalance function ends here

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

    int found = 0;
    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, username) == 0) {
            found = 1;
            printf("Enter amount to deposit: ");
            scanf("%f", &amount);
            getchar();

            if (amount <= 0) {
                printf("Amount should be greater than zero.\n");
                fclose(file);
                return;
            }

            user.balance += amount;

            FILE *tempFile = fopen("temp.csv", "w");
            if (tempFile == NULL) {
                printf("Error opening temporary file!\n");
                fclose(file);
                return;
            }

            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n",
                user.fullName, user.username, user.email, user.phone,
                user.accountNumber, user.address, user.nidNo, user.balance, user.password);

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

            logTransaction(username, "Deposit", amount);
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


//withdrawMoney function starts here
void withdrawMoney(const char* username) {
    FILE *file;
    struct User user;
    float amount;

    file = fopen("users.csv", "r+");
    if (file == NULL) {
        printf("No users registered yet.\n");
        return;
    }

    int found = 0;
    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%[^\n]\n",
                  user.fullName, user.username, user.email, user.phone, user.accountNumber, user.address, user.nidNo, &user.balance, user.password) == 9) {
        if (strcmp(user.username, username) == 0) {
            found = 1;
            printf("Current balance: %.2f\n", user.balance);
            printf("Enter amount to withdraw: ");
            scanf("%f", &amount);
            getchar();

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

            user.balance -= amount;

            FILE *tempFile = fopen("temp.csv", "w");
            if (tempFile == NULL) {
                printf("Error opening temporary file!\n");
                fclose(file);
                return;
            }

            fprintf(tempFile, "%s,%s,%s,%s,%s,%s,%s,%.2f,%s\n",
                user.fullName, user.username, user.email, user.phone,
                user.accountNumber, user.address, user.nidNo, user.balance, user.password);

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

            logTransaction(username, "Withdraw", amount);

            printf("Withdrawal successful! \n");
            return;
        }
    }

    fclose(file);
    if (!found) {
        printf("User not found.\n");
    }
}
//withdrawMoney function ends here


//deleteAccount function starts here
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
//deleteAccount function ends here

//logout function starts here
void logout(char* loggedInUsername) {
    strcpy(loggedInUsername, "");
    printf("You have logged out.\n");
}
//logout function ends here

//aboutBank function starts here
void aboutBank() {
    printf("Bank Name: Fraud Bank PLC\n");
    printf("Manager: Opi Sharma\n");
    printf("Head Office: BUBT\n");
}
//aboutBank function ends here

//loggedInMenu function starts here
void loggedInMenu(char* loggedInUsername) {
    int choice;
    do {
        printf("\n--- User Menu ---\n");
        printf("1. Transfer Money\n"
               "2. Check Balance\n"
               "3. Deposit Money\n"
               "4. Withdraw Money\n"
               "5. Statement\n"
               "6. Delete Account\n"
               "7. About Bank\n"
               "8. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

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
                deleteAccount(loggedInUsername);
                return;
            case 7:
                aboutBank();
                break;
            case 8:
                logout(loggedInUsername);
                return;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    } while (strlen(loggedInUsername) > 0);
}
//loggedInMenu function ends here

//main function starts here
int main() {
    char loggedInUsername[30] = "";
    int choice;

    do {
        printf("\n--- Main Menu ---\n");
        printf("1. Register\n2. Login\n3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                if (loginUser(loggedInUsername)) {
                    printf("\nWelcome, %s!\n", loggedInUsername);
                    loggedInMenu(loggedInUsername);
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
//main function ends here
