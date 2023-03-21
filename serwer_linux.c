/*
	Projekt zaliczeniowy z Sieci Komputerowych 2 - program serwera
	System wymiany komunikatow typu publish/subscribe
	Agnieszka Gapinska
	Michal Hordecki
*/

#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <errno.h>

#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <sys/time.h>

#define PORT 8080

typedef struct {
  int user_id;
  char username[30];
}
user;

typedef struct {
  int topic;
  int user[30];
  int pos;
}
topics;

typedef struct {
  int topics[10];
}
usertopic;

int main(int argc, char * argv[]) {

  int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30, activity, i, sd;
  int max_sd;
  struct sockaddr_in address;

  char buffer[1024];
  char buffer1[1024] = "";
  user userList[30] = {
    {
      0, ""
    }
  };
  topics topicList[10] = {
    {
      0, {0}, 0
    }
  };
  usertopic uTopic[99] = {0};

  fd_set readfds;
  FILE * fp;

  char typ[2]; 
  char id[3];
  char username[30];
  int counter = 0;
  char conv[3];
  int j = 0;

  for (i = 0; i < max_clients; i++) {
    client_socket[i] = 0;
  }

  //create a master socket
  if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  //type of socket created
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  //bind the socket to localhost port 8888
  if (bind(master_socket, (struct sockaddr * ) & address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  printf("Listener on port %d \n", PORT);

  //try to specify maximum of 3 pending connections for the master socket
  if (listen(master_socket, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  //accept the incoming connection
  addrlen = sizeof(address);
  puts("Waiting for connections ...");

  while (1) {
    //clear the socket set
    FD_ZERO( & readfds);

    //add master socket to set
    FD_SET(master_socket, & readfds);
    max_sd = master_socket;

    //add child sockets to set
    for (i = 0; i < max_clients; i++) {
      //socket descriptor
      sd = client_socket[i];

      //if valid socket descriptor then add to read list
      if (sd > 0)
        FD_SET(sd, & readfds);

      //highest file descriptor number, need it for the select function
      if (sd > max_sd)
        max_sd = sd;
    }

    //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
    activity = select(max_sd + 1, & readfds, NULL, NULL, NULL);

    if ((activity < 0) && (errno != EINTR)) {
      printf("select error");
    }

    //If something happened on the master socket , then its an incoming connection
    if (FD_ISSET(master_socket, & readfds)) {
      if ((new_socket = accept(master_socket, (struct sockaddr * ) & address, (socklen_t * ) & addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
      }

      //inform user of socket number - used in send and receive commands
      printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

      //add new socket to array of sockets
      for (i = 0; i < max_clients; i++) {
        //if position is empty
        if (client_socket[i] == 0) {
          client_socket[i] = new_socket;
          printf("Adding to list of sockets as %d\n", i);

          break;
        }
      }
    }

    for (i = 0; i < max_clients; i++) {
      sd = client_socket[i];

      if (FD_ISSET(sd, & readfds)) {

        int checker = 0;

        read(sd, buffer, sizeof(buffer));

        strncpy(typ, buffer, 1);

        if (atoi(typ) == 1) {
          strncpy(id, buffer + 1, 2);
          strncpy(username, buffer + 3, 29);
          strcpy(buffer, "0 Zalogowano pomyslnie :)");
          userList[i].user_id = atoi(id);
          strcpy(userList[counter].username, username);
          counter = counter + 1;

          send(sd, buffer, sizeof(buffer), 0);
          bzero(buffer, sizeof(buffer));
          printf("temat: %d\n", userList[0].user_id);
          printf("Klient: %s\n", userList[0].username);
          printf("Klient: %d\n", userList[2].user_id);
        }

        if (atoi(typ) == 2) {
          int temp_user;
          char id_t[2];
          char id_user[3];
          strncpy(id_t, buffer + 1, 1);
          strncpy(id_user, buffer + 2, 2);
          for (int y = 0; y < 15; y++) {
            if (strcmp(username, userList[y].username) == 0) {
              temp_user = userList[y].user_id;
            }

          }
          for (int y = 0; y < 15; y++) {
            if (atoi(id_t) == topicList[y].topic) {
              topicList[y].user[topicList[y].pos] = temp_user;
            }
            topicList[y].pos++;
          }
          int yi = 0;
          while (yi < 16) {
            if (uTopic[temp_user].topics[yi] == 0) {
              uTopic[temp_user].topics[yi] = atoi(id_t);
              yi = yi + 15;
              break;
            };
            yi++;
          }

          strcpy(buffer, " Zasubskrybowal_s wybrany temat :)");
          send(sd, buffer, sizeof(buffer), 0);
          bzero(buffer, sizeof(buffer));

        }

        //rejestracja typu wiadomosci
        if (atoi(typ) == 3) {
          char t_id[2];
          strncpy(t_id, buffer + 1, 1);
          for (int ip = 0; ip < 15; ip++) {
            if (atoi(t_id) == topicList[ip].topic) {
              strcpy( & buffer1[0], "!!! Podany typ juz istnieje !!!");
              checker = 1;

            }
          }
          if (checker == 0) {
            topicList[j].topic = atoi(t_id);
            topicList[j].pos = 0;
            j = j + 1;
            strcpy( & buffer1[0], "Typ zostal dodany pomyslnie :)");
          }
          send(sd, buffer1, sizeof(buffer1), 0);
          strcpy( & buffer[0], "");
          strcpy( & buffer1[0], "");

        }

        //rozglaszanie wiadomosci
        if (atoi(typ) == 5) {
          char client_name[30] = "";
          char recv_message[255] = "";
          char m_id[2] = "";
          strncpy(m_id, buffer + 1, 1);
          strncpy(recv_message, buffer + 2, 255);
          for (int x = 0; x < 15; x++) {
            if (topicList[x].topic == atoi(m_id)) {
              for (int xy = 0; xy < 15; xy++) {
                if (topicList[x].user[xy] != 0) {
                  sprintf(conv, "%d", topicList[x].user[xy]);
                  strcpy(client_name, "user_");
                  strcat(client_name, conv);
                  strcat(client_name, ".txt");
                  fp = fopen(client_name, "a");
                  bzero(client_name, sizeof(client_name));
                  fputs(recv_message, fp);
                  fputs("\n", fp);
                  fclose(fp);

                }
              }
            }

          }
        }

        //odbieranie wiadomosci
        if (atoi(typ) == 4) {

          char recv_id[3] = "";
          char recv_name[30] = "";

          strncpy(recv_id, buffer + 1, 2);
          strcpy(recv_name, "user_");
          strcat(recv_name, recv_id);
          strcat(recv_name, ".txt");
          fp = fopen(recv_name, "r+");
          if (fp == NULL) {
            strcpy( & buffer[0], "EOFFAIL");
            send(sd, buffer, sizeof(buffer), 0);
          } else {

            while (fgets(buffer, sizeof(buffer), fp)) {

              send(sd, buffer, sizeof(buffer), 0);

            }

            strcpy( & buffer[0], "EOFFAIL");
            send(sd, buffer, sizeof(buffer), 0);
            fclose(fp);
            fp = fopen(recv_name, "w+");
            fputs("", fp);
            fclose(fp);
          }
        }

        //wypisz zarejestrowane typy wiadomosci
        if (atoi(typ) == 7) {
          strcpy( & buffer[0], "");
          int ind = 0;
          char top_list[10] = "";

          for (int y = 0; y < 15; y++) {
            if (topicList[y].topic != 0) {

              printf("done");
              sprintf(conv, "%d", topicList[y].topic);
              strcat(top_list, conv);
              ind++;
            }
            strcpy( & buffer[0], top_list);
            send(sd, buffer, sizeof(buffer), 0);
            strcpy( & buffer[0], "");
            strcpy( & buffer1[0], "");
          }
        }

        if (atoi(typ) == 9) {
          close(sd);
        }

      }
    }

  }
  return 0;
}