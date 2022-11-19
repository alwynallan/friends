// friends.c Copyright (C) 2022 alwynallan@gmail.com, MIT License

// $ gcc -Wall -O3 -c grand.c
// $ gcc -Wall -O3 friends.c grand.o -lm -o friends
// $ ./friends

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "grand.h"

#define PEOPLE 1000000
#define FRIENDSHIPS 5000000
#define MAX_FRIENDS 30
//#define PEOPLE 50
//#define FRIENDSHIPS 250
//#define WRITE_DOT_FILE
// $ dot -Tpng friends.dot > friends_dot.png

int friend_count[PEOPLE];
int friend_list[PEOPLE][MAX_FRIENDS]; // adjacency lists

void add_friend(int a, int b) {
  assert(friend_count[a] < MAX_FRIENDS);
  friend_list[a][friend_count[a]++] = b;
  assert(friend_count[b] < MAX_FRIENDS);
  friend_list[b][friend_count[b]++] = a;
}

bool is_friend(int a, int b) {
  if(friend_count[a] <= friend_count[b]) { // scan the shorter list
    for(int i=0; i < friend_count[a]; i++)
      if(friend_list[a][i] == b) return true;
  }
  else {
    for(int i=0; i < friend_count[b]; i++)
      if(friend_list[b][i] == a) return true;
  }
  return false;
}

int main() {
  printf("\"Simulation: %d People with %d Friendships\"\n", PEOPLE, FRIENDSHIPS);
  memset(friend_count, 0, sizeof(friend_count));
  for(int i=0; i<FRIENDSHIPS; i++) {
    int a, b;
    do {
      a = grand_0_m(PEOPLE-1);
      b = grand_0_m(PEOPLE-1);
    } while(a == b || is_friend(a,b));
    add_friend(a, b);
  }
  
  histogram *fh = histogram_init(-.5, 20.5, 1.);
  histogram *ffh = histogram_init(-.5, 20.5, 1.);
  int average_friends_of_friends_more = 0;
  int people_with_friends = 0;
  for(int p=0; p<PEOPLE; p++) {
    histogram_count(fh, friend_count[p]);
    if(friend_count[p] > 0) {
      int total_friends_of_friends = 0;
      for(int f=0; f<friend_count[p]; f++) total_friends_of_friends += friend_count[friend_list[p][f]];
      people_with_friends++;
      double average_friends_of_friends = (double)total_friends_of_friends / (double)friend_count[p];
      histogram_count(ffh, average_friends_of_friends);
      if(average_friends_of_friends > (double)friend_count[p]) average_friends_of_friends_more++;
    }
  }
  printf("\"Friends (u=%.2Lf sd=%.2Lf)\"\n",
         fh->dsum / (long double)fh->dcount,
         sqrtl(fh->d2sum / (long double)fh->dcount - powl(fh->dsum / (long double)fh->dcount,2.)));
  histogram_dump(fh, 1);
  printf("\"Friends (average) of friends (u=%.2Lf sd=%.2Lf)\"\n",
         ffh->dsum / (long double)ffh->dcount,
         sqrtl(ffh->d2sum / (long double)ffh->dcount - powl(ffh->dsum / (long double)ffh->dcount,2.)));
  histogram_dump(ffh, 1);
  printf("\"%d people's friends have more friends (on average) than them out of %d people with friends.\"\n",
         average_friends_of_friends_more, people_with_friends);

  #ifdef WRITE_DOT_FILE
  FILE * fp = fopen("friends.dot", "w");
  assert(fp != NULL);
  fprintf(fp, "strict graph {\n");
  for(int a=0; a<PEOPLE; a++)
    for(int i=0; i<friend_count[a]; i++) {
      int b = friend_list[a][i];
      if(a<b) fprintf(fp, "  %d -- %d\n", a, b);
    }
  fprintf(fp, "}\n");
  fclose(fp);
  #endif // WRITE_DOT_FILE

  return 0;
}
