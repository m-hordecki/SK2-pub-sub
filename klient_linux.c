#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <sys/socket.h>

#include <sys/types.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#define PORT 8080

char client_id[3]; //globalna zmienna z id zalogowanego klienta

void writeTopic(int clientSocket) {
  char mess[255];
  char topicType[2];
  char buffer[1024] = "";
  char buffer1[1024] = "";

  printf("ROZGLASZANIE WIADOMOSCI\n");
  printf("Podaj typ wiadomosci: ");
  scanf("%s", topicType);
  printf("Podaj tresc rozglaszanej wiadomosci: ");
  scanf(" %[^\n]%*c", mess);
  strcat(buffer1, "5");
  strcat(buffer1, topicType);
  strcat(buffer1, mess);
  strcpy( & buffer[0], buffer1);
  send(clientSocket, buffer, sizeof(buffer), 0);
}

void registerRcvr(int clientSocket) {
  char buffer[1024] = "";
  char buffer1[1024] = "";
  char username[30];
  char topicType[2];

  printf("REJESTRACJA ODBIORCY\n");
  printf("Podaj swoja nazwe: ");
  scanf("%s", username);
  printf("Podaj typ wiadomosi, ktory chcesz subskrybowac(1-9): ");
  scanf("%s", topicType);
  strcat(buffer1, "2");
  strcat(buffer1, topicType);
  strcat(buffer1, username);
  strcpy( & buffer[0], buffer1);
  send(clientSocket, buffer, sizeof(buffer), 0);
  printf("Server:%s\n", buffer);
  strcpy( & buffer[0], "");
  strcpy( & buffer1[0], "");
  recv(clientSocket, buffer, sizeof(buffer), 0);
  printf("Server:%s\n", buffer);
}

void showTypes(int clientSocket) {
  char buffer[1024] = "";
  char id[2];
  int backnote[10];
  strcpy( & buffer[0], "7");
  send(clientSocket, buffer, sizeof(buffer), 0);
  strcpy( & buffer[0], "");
  printf("Zarejstrowane tematy: \n");
  recv(clientSocket, buffer, sizeof(buffer), 0);
  for (int i = 0; i < 15; i++) {
    strncpy(id, buffer + i, 1);
    backnote[i] = atoi(id);
  }

  for (int i = 0; i < 15; i++) {
    if (backnote[i] != 0) {
      printf("Temat: %d\n", backnote[i]);
    }
  }

}

void registerType(int clientSocket) {

  char buffer[1024] = "";
  char buffer1[1024] = "";
  char topicType[2];
  printf("REJESTRACJA TYPU WIADOMOSCI\n");
  printf("Podaj nowy typ wiadomosci: ");
  scanf("%s", topicType);
  strcat(buffer1, "3");
  strcat(buffer1, topicType);
  strcpy( & buffer[0], buffer1);
  send(clientSocket, buffer, sizeof(buffer), 0);
  strcpy( & buffer[0], "");
  strcpy( & buffer1[0], "");
  recv(clientSocket, buffer, sizeof(buffer), 0);
  printf("%s\n", buffer);

}

int Login(int clientSocket) {
  char buffer[1024] = "";
  char buffer1[1024] = "";
  char typ[2];
  char username[30];
  char user_id[3];

  printf("            LOGOWANIE\n");
  printf("\n");
  printf(" Podaj id uzytkownika(10-99): ");
  scanf("%s", user_id);
  strcpy( & client_id[0], user_id);
  printf(" Podaj nazwe uzytkownika: ");
  scanf("%s", username);
  strcat(buffer1, "1");
  strcat(buffer1, user_id);
  strcat(buffer1, username);
  strcpy( & buffer[0], buffer1);
  send(clientSocket, buffer, sizeof(buffer), 0);
  strcpy( & buffer[0], "");
  strcpy( & buffer1[0], "");
  recv(clientSocket, buffer, sizeof(buffer), 0);
  strncpy(typ, buffer, 1);
  strncpy(username, buffer + 1, 29);
  printf("Server:%s\n", username);
  return atoi(typ);
}

void Menu() {
  printf("+-------------------MENU--------------------+\n");
  printf("| 1 - Rejestracja odbiorcy                  |\n");
  printf("| 2 - Rejestracja typu wiadomosci           |\n");
  printf("| 3 - Napisz wiadomosc                      |\n");
  printf("| 4 - Odbierz wiadomosci                    |\n");
  printf("| 5 - Pokaz zarejstrowane typy wiadomosci   |\n");
  printf("| 6 - Wyjscie                               |\n");
  printf("+-------------------------------------------+\n");
}

int main() {

  int clientSocket, ret;
  struct sockaddr_in serverAddr;
  char buffer[1024] = "";
  int checker = 1;
  int action = -1;

  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    printf("[-]Error in connection.\n");
    exit(1);
  }
  printf("[+]Client Socket is created.\n");

  memset( & serverAddr, '\0', sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  ret = connect(clientSocket, (struct sockaddr * ) & serverAddr, sizeof(serverAddr));
  if (ret < 0) {
    printf("[-]Error in connection.\n");
    exit(1);
  }
  printf("[+]Connected to Server.\n");

  while (1) {
    while (checker != 0) {
      checker = Login(clientSocket);
    }

    Menu();

    printf("Dokonaj wyboru: ");
    scanf("%d", & action);

    switch (action) {
    case 1:
      registerRcvr(clientSocket);
      break;
    case 2:
      registerType(clientSocket);
      break;
    case 3:
      writeTopic(clientSocket);
      break;
    case 4:
      strcpy( & buffer[0], "4");
      strcat(buffer, client_id);
      send(clientSocket, buffer, sizeof(buffer), 0);
      strcpy( & buffer[0], "");
      while (recv(clientSocket, buffer, sizeof(buffer), 0) != -1) {
        if (strcmp(buffer, "EOFFAIL") == 0) break;
        printf("Otrzymano: %s\n", buffer);
        strcpy( & buffer[0], "");
      }

      break;
    case 5:
      showTypes(clientSocket);
      break;
    case 6:
      strcpy( & buffer[0], "9");
      send(clientSocket, buffer, sizeof(buffer), 0);
      close(clientSocket);
      exit(0);
      break;
    default:
      printf("Nie ma takiej opcji!\n");
      break;
    }

  }

  return 0;
}