#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LENGTH 50
#define POEM_LENGTH 200
#define MAX_BUNNIES 100

typedef struct {
    char name[NAME_LENGTH];
    char poem[POEM_LENGTH];
    int eggs;
} Bunny;

int loadBunnies(const char *filename, Bunny bunnies[], int maxBunnies) {
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return 0;
    int count = 0;
    char line[256];

    while (count < maxBunnies && fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, "|");
        if (!token)
            continue;
        strncpy(bunnies[count].name, token, NAME_LENGTH);
        bunnies[count].name[NAME_LENGTH - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strncpy(bunnies[count].poem, token, POEM_LENGTH);
        bunnies[count].poem[POEM_LENGTH - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token)
            continue;
        bunnies[count].eggs = atoi(token);
        count++;
    }
    fclose(fp);
    return count;
}

int saveBunnies(const char *filename, Bunny bunnies[], int count) {
    FILE *fp = fopen(filename, "w");
    if (!fp)
        return -1;
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s|%s|%d\n", bunnies[i].name, bunnies[i].poem, bunnies[i].eggs);
    }
    fclose(fp);
    return 0;
}

void addBunny(const char *filename) {
    Bunny newBunny;
    printf("Enter bunny name: ");
    if (!fgets(newBunny.name, NAME_LENGTH, stdin))
        return;
    newBunny.name[strcspn(newBunny.name, "\n")] = '\0';

    printf("Enter bunny poem: ");
    if (!fgets(newBunny.poem, POEM_LENGTH, stdin))
        return;
    newBunny.poem[strcspn(newBunny.poem, "\n")] = '\0';

    newBunny.eggs = 0;
    FILE *fp = fopen(filename, "a");
    if (!fp) {
        perror("File open error");
        return;
    }
    fprintf(fp, "%s|%s|%d\n", newBunny.name, newBunny.poem, newBunny.eggs);
    fclose(fp);
    printf("Bunny '%s' added.\n", newBunny.name);
}

void listBunnies(const char *filename) {
    Bunny bunnies[MAX_BUNNIES];
    int count = loadBunnies(filename, bunnies, MAX_BUNNIES);
    if (count == 0) {
        printf("No bunnies registered.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        printf("\nName: %s\nPoem: %s\nEggs: %d\n", 
               bunnies[i].name, bunnies[i].poem, bunnies[i].eggs);
    }
}

void modifyBunny(const char *filename) {
    char target[NAME_LENGTH];
    printf("Enter bunny name to modify: ");
    if (!fgets(target, sizeof(target), stdin))
        return;
    target[strcspn(target, "\n")] = '\0';

    Bunny bunnies[MAX_BUNNIES];
    int count = loadBunnies(filename, bunnies, MAX_BUNNIES);
    int index = -1;
    for (int i = 0; i < count; i++) {
        if (strcmp(bunnies[i].name, target) == 0) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf("Bunny '%s' not found.\n", target);
        return;
    }
    printf("Current poem: %s\nEnter new poem (or press Enter to keep): ", 
           bunnies[index].poem);
    char buffer[POEM_LENGTH];
    if (fgets(buffer, sizeof(buffer), stdin) && buffer[0] != '\n') {
        buffer[strcspn(buffer, "\n")] = '\0';
        strncpy(bunnies[index].poem, buffer, POEM_LENGTH);
        bunnies[index].poem[POEM_LENGTH - 1] = '\0';
    }
    printf("Current eggs: %d\nEnter new egg count (or press Enter to keep): ", 
           bunnies[index].eggs);
    char eggBuff[10];
    if (fgets(eggBuff, sizeof(eggBuff), stdin) && eggBuff[0] != '\n') {
        bunnies[index].eggs = atoi(eggBuff);
    }
    if (saveBunnies(filename, bunnies, count) == 0)
        printf("Bunny '%s' modified.\n", target);
    else
        perror("Error saving changes");
}

void deleteBunny(const char *filename) {
    char target[NAME_LENGTH];
    printf("Enter bunny name to delete: ");
    if (!fgets(target, sizeof(target), stdin))
        return;
    target[strcspn(target, "\n")] = '\0';

    Bunny bunnies[MAX_BUNNIES];
    int count = loadBunnies(filename, bunnies, MAX_BUNNIES);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(bunnies[i].name, target) == 0) {
            for (int j = i; j < count - 1; j++) {
                bunnies[j] = bunnies[j + 1];
            }
            count--;
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Bunny '%s' not found.\n", target);
        return;
    }
    if (saveBunnies(filename, bunnies, count) == 0)
        printf("Bunny '%s' deleted.\n", target);
    else
        perror("Error saving changes");
}

void declareKing(const char *filename) {
    Bunny bunnies[MAX_BUNNIES];
    int count = loadBunnies(filename, bunnies, MAX_BUNNIES);
    if (count == 0) {
        printf("No bunnies registered.\n");
        return;
    }
    int kingIndex = 0;
    for (int i = 1; i < count; i++) {
        if (bunnies[i].eggs > bunnies[kingIndex].eggs)
            kingIndex = i;
    }
    printf("Easter Bunny King: %s with %d red eggs!\n", 
           bunnies[kingIndex].name, bunnies[kingIndex].eggs);
}

int main(void) {
    const char *filename = "bunny_data.txt";
    int choice;
    do {
        printf("\n===== Bunny Registry Menu =====\n");
        printf("1. Add Bunny\n");
        printf("2. List Bunnies\n");
        printf("3. Modify Bunny\n");
        printf("4. Delete Bunny\n");
        printf("5. Declare Easter Bunny King\n");
        printf("6. Exit\n");
        printf("Enter your choice (1-6): ");
        
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');
        switch (choice) {
            case 1:
                addBunny(filename);
                break;
            case 2:
                listBunnies(filename);
                break;
            case 3:
                modifyBunny(filename);
                break;
            case 4:
                deleteBunny(filename);
                break;
            case 5:
                declareKing(filename);
                break;
            case 6:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please select 1-6.\n");
        }
    } while (choice != 6);
    return 0;
}
