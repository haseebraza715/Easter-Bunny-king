#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define NAME_LENGTH 50
#define POEM_LENGTH 200
#define MAX_BUNNIES 100

typedef struct {
    char name[NAME_LENGTH];
    char poem[POEM_LENGTH];
    int eggs;
} Bunny;

volatile sig_atomic_t bunnyArrived = 0;

void bunnySignalHandler(int sig) {
    if (sig == SIGUSR1) {
        bunnyArrived = 1;
        printf("[Chief Bunny] A bunny boy has arrived to water!\n");
    }
}

int loadBunnies(const char *filename, Bunny bunnies[], int maxBunnies) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Could not open bunny file for reading");
        return 0;
    }
    int count = 0;
    char line[256];

    while (count < maxBunnies && fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, "|");
        if (!token) continue;

        strncpy(bunnies[count].name, token, NAME_LENGTH - 1);
        bunnies[count].name[NAME_LENGTH - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(bunnies[count].poem, token, POEM_LENGTH - 1);
        bunnies[count].poem[POEM_LENGTH - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        bunnies[count].eggs = atoi(token);

        count++;
    }

    fclose(fp);
    return count;
}

int saveBunnies(const char *filename, Bunny bunnies[], int count) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Could not open bunny file for writing");
        return -1;
    }

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s|%s|%d\n", bunnies[i].name, bunnies[i].poem, bunnies[i].eggs);
    }

    fclose(fp);
    return 0;
}

void addBunny(const char *filename) {
    Bunny newBunny;

    printf("Enter bunny name: ");
    if (!fgets(newBunny.name, NAME_LENGTH, stdin)) return;
    newBunny.name[strcspn(newBunny.name, "\n")] = '\0';

    printf("Enter bunny poem: ");
    if (!fgets(newBunny.poem, POEM_LENGTH, stdin)) return;
    newBunny.poem[strcspn(newBunny.poem, "\n")] = '\0';

    newBunny.eggs = 0;

    FILE *fp = fopen(filename, "a");
    if (!fp) {
        perror("Could not open file for appending");
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
    if (!fgets(target, sizeof(target), stdin)) return;
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

    printf("Current poem: %s\nEnter new poem (or press Enter to keep): ", bunnies[index].poem);
    char buffer[POEM_LENGTH];
    if (fgets(buffer, sizeof(buffer), stdin) && buffer[0] != '\n') {
        buffer[strcspn(buffer, "\n")] = '\0';
        strncpy(bunnies[index].poem, buffer, POEM_LENGTH - 1);
        bunnies[index].poem[POEM_LENGTH - 1] = '\0';
    }

    printf("Current eggs: %d\nEnter new egg count (or press Enter to keep): ", bunnies[index].eggs);
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
    if (!fgets(target, sizeof(target), stdin)) return;
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

void startWateringCompetition(const char *filename) {
    Bunny bunnies[MAX_BUNNIES];
    int count = loadBunnies(filename, bunnies, MAX_BUNNIES);
    if (count == 0) {
        printf("No bunnies registered.\n");
        return;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Pipe creation failed");
        return;
    }

    signal(SIGUSR1, bunnySignalHandler);
    printf("\n[Chief Bunny] Starting watering competition...\n");

    for (int i = 0; i < count; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            return;
        } else if (pid == 0) {
            close(pipefd[0]); 
            usleep(rand() % 500000); 
            srand(time(NULL) ^ (getpid() << 16));
            kill(getppid(), SIGUSR1);
            sleep(1); 

            printf("\n[Bunny %s arrives to water!]\n", bunnies[i].name);
            printf("%s recites: \"%s\"\n", bunnies[i].name, bunnies[i].poem);

            int earnedEggs = rand() % 20 + 1;
            printf("%s received %d red eggs!\n", bunnies[i].name, earnedEggs);

            write(pipefd[1], &earnedEggs, sizeof(int));
            close(pipefd[1]);
            exit(0);
        } else {
            pause(); 
            bunnyArrived = 0;
        }
    }

    close(pipefd[1]);  

    for (int i = 0; i < count; i++) {
        int earnedEggs;
        read(pipefd[0], &earnedEggs, sizeof(int));
        bunnies[i].eggs += earnedEggs;
        wait(NULL);
    }

    close(pipefd[0]);
    saveBunnies(filename, bunnies, count);
    declareKing(filename);
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
        printf("7. Start Watering Competition\n");
        printf("Enter your choice (1-7): ");

        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        while (getchar() != '\n'); 

        switch (choice) {
            case 1: addBunny(filename); break;
            case 2: listBunnies(filename); break;
            case 3: modifyBunny(filename); break;
            case 4: deleteBunny(filename); break;
            case 5: declareKing(filename); break;
            case 6: printf("Exiting...\n"); break;
            case 7: startWateringCompetition(filename); break;
            default: printf("Invalid choice. Please select 1-7.\n");
        }
    } while (choice != 6);

    return 0;
}
