#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRAINS 5

struct Train 
{
    char name[50];
    char from[50];
    char to[50];
    int totalSeats;
    int seatsAvailable;
} trains[MAX_TRAINS] = 
{
    {"Intercity Express", "Mumbai", "Pune", 5, 5},
    {"Superfast Express", "Delhi", "Chandigarh", 3, 3},
    {"Shatabdi Express", "New delhi", "Amritsar", 4, 4},
    {"Rajdhani Express","Delhi","Mumbai",5,5},
    {"Vande Bharat","New Delhi","varanasi", 5,5}
};

struct Reservation 
{
    int ticketID;
    char passengerName[50];
    char trainName[50];
    struct Reservation* next;
};

struct Reservation* head = NULL;
int nextTicketID = 1001;

void showTrainOptions();
void bookTicket();
void cancelTicket(int ticketID);
void displayReservations();
void searchByTicketID(int ticketID);
void updateReservation(int ticketID);
void saveReservationsToFile();
void loadReservationsFromFile();

void showTrainOptions() 
{
    printf("\nAvailable Trains:\n");
    for (int i = 0; i < MAX_TRAINS; i++) 
    {
        printf("%d. %s (%s to %s) - Seats Available: %d\n", i + 1,
               trains[i].name, trains[i].from, trains[i].to, trains[i].seatsAvailable);
    }
}
void bookTicket() 
{
    int choice;
    char name[100];
    showTrainOptions();
    printf("Choose a train (1-%d): ", MAX_TRAINS);
    scanf("%d", &choice);
    getchar();

    if (choice < 1 || choice > MAX_TRAINS) 
    {
        printf("Invalid train choice.\n");
        return;
    }
    if (trains[choice - 1].seatsAvailable <= 0) 
    {
        printf("Sorry! No seats available on %s.\n", trains[choice - 1].name);
        return;
    }
    printf("Enter passenger name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';
    struct Reservation* newNode = (struct Reservation*)malloc(sizeof(struct Reservation));
    newNode->ticketID = nextTicketID++;
    strcpy(newNode->passengerName, name);
    strcpy(newNode->trainName, trains[choice - 1].name);
    newNode->next = NULL;

    if (head == NULL)
        head = newNode;
    else 
    {
        struct Reservation* temp = head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newNode;
    }
    trains[choice - 1].seatsAvailable--;
    printf("Ticket booked successfully! Ticket ID: %d\n", newNode->ticketID);
    saveReservationsToFile();
}
void cancelTicket(int ticketID) 
{
    struct Reservation* temp = head;
    struct Reservation* prev = NULL;

    while (temp != NULL && temp->ticketID != ticketID) 
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) 
    {
        printf("Ticket ID %d not found.\n", ticketID);
        return;
    }
    for (int i = 0; i < MAX_TRAINS; i++) 
    {
        if (strcmp(trains[i].name, temp->trainName) == 0) {
            trains[i].seatsAvailable++;
            break;
        }
    }
    if (prev == NULL)
        head = temp->next;
    else
        prev->next = temp->next;

    free(temp);
    printf("Ticket ID %d cancelled successfully.\n", ticketID);
    saveReservationsToFile();
}
void displayReservations() 
{
    if (head == NULL) 
    {
        printf("No current reservations.\n");
        return;
    }

    struct Reservation* temp = head;
    printf("\nReservation List:\n");
    while (temp != NULL) {
        printf("-----------------------------\n");
        printf("Ticket ID     : %d\n", temp->ticketID);
        printf("Passenger Name: %s\n", temp->passengerName);
        printf("Train Name    : %s\n", temp->trainName);
        temp = temp->next;
    }
    printf("-----------------------------\n");
}
void searchByTicketID(int ticketID) {
    struct Reservation* temp = head;
    while (temp != NULL) {
        if (temp->ticketID == ticketID) {
            printf("Reservation found:\n");
            printf("Ticket ID     : %d\n", temp->ticketID);
            printf("Passenger Name: %s\n", temp->passengerName);
            printf("Train Name    : %s\n", temp->trainName);
            return;
        }
        temp = temp->next;
    }
    printf("Ticket ID %d not found.\n", ticketID);
}
void updateReservation(int ticketID) {
    struct Reservation* temp = head;
    while (temp != NULL) {
        if (temp->ticketID == ticketID) {
            printf("Enter new passenger name: ");
            fgets(temp->passengerName, sizeof(temp->passengerName), stdin);
            temp->passengerName[strcspn(temp->passengerName, "\n")] = '\0';

            printf("Train change is not allowed in this version.\n");
            saveReservationsToFile();
            return;
        }
        temp = temp->next;
    }
    printf("Ticket ID %d not found.\n", ticketID);
}
void saveReservationsToFile() 
{
    FILE* file = fopen("reservations.txt", "w");
    if (file == NULL) {
        printf("Error saving to file.\n");
        return;
    }

    struct Reservation* temp = head;
    while (temp != NULL) {
        fprintf(file, "%d,%s,%s\n", temp->ticketID, temp->passengerName, temp->trainName);
        temp = temp->next;
    }

    fclose(file);
}
void loadReservationsFromFile() 
{
    FILE* file = fopen("reservations.txt", "r");
    if (file == NULL) return;

    char line[200];
    while (fgets(line, sizeof(line), file)) 
    {
        struct Reservation* newNode = (struct Reservation*)malloc(sizeof(struct Reservation));
        sscanf(line, "%d,%49[^,],%49[^\n]", &newNode->ticketID, newNode->passengerName, newNode->trainName);
        newNode->next = NULL;

        if (newNode->ticketID >= nextTicketID)
            nextTicketID = newNode->ticketID + 1;

        for (int i = 0; i < MAX_TRAINS; i++) {
            if (strcmp(trains[i].name, newNode->trainName) == 0) {
                trains[i].seatsAvailable--;
                break;
            }
        }

        if (head == NULL)
            head = newNode;
        else {
            struct Reservation* temp = head;
            while (temp->next != NULL)
                temp = temp->next;
            temp->next = newNode;
        }
    }

    fclose(file);
}

int main() 
{
    int choice, ticketID;

    loadReservationsFromFile();

    do 
    {
        printf("\n====== Railway Reservation System ======\n");
        printf("1. Book Ticket\n");
        printf("2. Cancel Ticket\n");
        printf("3. Display All Reservations\n");
        printf("4. Search by Ticket ID\n");
        printf("5. Update Passenger Name\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) 
        {
            case 1: bookTicket(); break;
            case 2:
                printf("Enter Ticket ID to cancel: ");
                scanf("%d", &ticketID);
                cancelTicket(ticketID);
                break;
            case 3: displayReservations(); break;
            case 4:
                printf("Enter Ticket ID to search: ");
                scanf("%d", &ticketID);
                searchByTicketID(ticketID);
                break;
            case 5:
                printf("Enter Ticket ID to update: ");
                scanf("%d", &ticketID);
                getchar();
                updateReservation(ticketID);
                break;
            case 6: printf("Exiting system. Thank you!\n"); break;
            default: printf("Invalid option. Try again.\n");
        }
    } while (choice != 6);

    struct Reservation* temp;
    while (head != NULL) 
    {
        temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}
