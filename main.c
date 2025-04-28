#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

typedef struct {
    char player1[50];
    char player2[50];
    int score1;
    int score2;
} Match;

typedef struct {
    char name[50];
    int wins;
    int losses;
} Player;

void clear(){
    #ifdef _WIN32
    system("cls");  // Windows
    #else
    system("clear");  // Linux/macOS
    #endif
}

void header()
{
    clear();
    printf("%s--------------\n%s",BOLD,RESET);
    printf("%s1 - %sAdd Match\n%s",BOLD,GREEN,RESET);
    printf("%s2 - %sLeaderboard\n%s",BOLD,YELLOW,RESET);
    printf("%s0 - %sExit\n%s",BOLD,RED,RESET);
    printf("%s--------------\n%s",BOLD,RESET);
}

void add() 
{
    clear();
    char input[110], player1[50], player2[50];
    int score1, score2;

    printf("Enter match result (Format: Player1 Score1 / Score2 Player2): ");
    getchar();  
    fgets(input, sizeof(input), stdin);  
    input[strcspn(input, "\n")] = 0;  

    if (sscanf(input, "%s %d / %d %s", player1, &score1, &score2, player2) == 4 && (score1==21 ^ score2==21) && (score1<=21 && score2<=21)) {
        printf("%sMatch Added%s: %s %d / %d %s\n", GREEN, RESET, player1, score1, score2, player2);
        FILE *file = fopen("matches.txt", "a");
        fprintf(file, "%s,%d,%d,%s\n",player1,score1,score2,player2);
        fclose(file);
    } else {
        printf("%sInvalid Match Format!%s Please follow: Player1 Score1 / Score2 Player2\n",RED,RESET);
    }

    printf("\nPress %s%sEnter%s to return to menu...",BOLD,YELLOW,RESET);
    getchar();
}
 
void player(char* name, Player *array,int n){
    clear();
    int exist=0;

    printf("%sPlayer%s: %s\n",YELLOW,RESET,name);

    for(int i=0;i<n;i++){
        if(strcmp(array[i].name,name)==0){
            double winrate = (double) array[i].wins / (array[i].wins + array[i].losses);
            printf("%sWins%s - %d\n", GREEN,RESET, array[i].wins);
            printf("%sLosses%s - %d\n", RED,RESET, array[i].losses);
            printf("%sWin-rate%s - %.2f%%\n",CYAN,RESET, winrate*100);
            exist++;
            break;
        }
    }

    if(!exist){
        clear();
        printf("Player %s%s%s%s has not played any matches \n",BOLD,RED,name,RESET);
        printf("Press %s%sEnter%s to return to Option Select Menu\n",BOLD,YELLOW,RESET);
        getchar();
        return;
    }

    char player1[50],player2[50];
    int score1,score2;

    FILE *history=fopen("matches.txt","r");

    while(fscanf(history, "%49[^,],%d,%d,%49[^\n]\n", player1, &score1, &score2, player2)==4){
        if(strcmp(name,player1)==0 || strcmp(name,player2)==0)
            printf("%s %d / %d %s\n", player1, score1, score2, player2);
    }

    fclose(history);

    printf("Press Enter to return to Option Select Menu\n");
    getchar();
}

int player_in_file(const char *player, int *wins, int *losses) {
    FILE *file = fopen("players.txt", "r");
    char name[50];
    int w, l;
    while (fscanf(file, "%49[^,],%d,%d\n", name, &w, &l) == 3) {
        if (strcmp(name, player) == 0) {
            *wins = w;
            *losses = l;
            fclose(file);
            return 1;  
        }
    }
    fclose(file);
    return 0;  
}

void add_player(const char *player) {
    FILE *file = fopen("players.txt", "a");
    fprintf(file, "%s,0,0\n", player);
    fclose(file);
}

void update_stats(const char *player, int wins, int losses) {
    FILE *file = fopen("players.txt", "r");

    FILE *temp = fopen("temp.txt", "w");

    char name[50];
    int w, l;
    int found = 0;

    while (fscanf(file, "%49[^,],%d,%d\n", name, &w, &l) == 3) {
        if (strcmp(name, player) == 0) {
            fprintf(temp, "%s,%d,%d\n", name, wins, losses);
            found = 1;
        } else {
            fprintf(temp, "%s,%d,%d\n", name, w, l);
        }
    }
    
    fclose(file);
    fclose(temp);

    remove("players.txt");
    rename("temp.txt", "players.txt");
}

int cmp_winrate(const void *a, const void *b) {
    Player *playerA = (Player *)a;
    Player *playerB = (Player *)b;
    
    double winrateA =  (double)playerA->wins / (playerA->wins + playerA->losses) ;
                      
    double winrateB =  (double)playerB->wins / (playerB->wins + playerB->losses) ;
                      
    
    return (winrateB - winrateA > 0) ? 1 : (winrateB - winrateA < 0) ? -1 : 0;
}

int cmp_wins(const void *a, const void *b) {
    Player *playerA = (Player *)a;
    Player *playerB = (Player *)b;
    return playerB->wins - playerA->wins;  
}

int cmp_losses(const void *a, const void *b) {
    Player *playerA = (Player *)a;
    Player *playerB = (Player *)b;
    return playerA->losses - playerB->losses;  
}

void leaderboard() {
    clear();
    
    printf("%sChoose Leaderboard Filter\n",BOLD);
    printf("1 - By %swin-rate\n%s",CYAN,RESET);
    printf("2 - By %swins\n%s",GREEN,RESET);
    printf("3 - By %slosses\n%s",RED,RESET);
    int filter;
    scanf("%d", &filter);
    getchar();
    char player1[50], player2[50];
    int score1, score2;
    clear();
    FILE *file = fopen("matches.txt", "r");

    while (fscanf(file, "%49[^,],%d,%d,%49[^\n]\n", player1, &score1, &score2, player2) == 4) {
        int wins1 = 0, losses1 = 0, wins2 = 0, losses2 = 0;

        if (!player_in_file(player1, &wins1, &losses1)) {
            add_player(player1);
        }
        if (!player_in_file(player2, &wins2, &losses2)) {
            add_player(player2);
        }

        if (score1 > score2) {
            wins1++;
            losses2++;
        } else {
            wins2++;
            losses1++;
        }

        update_stats(player1, wins1, losses1);
        update_stats(player2, wins2, losses2);
    }
    fclose(file);

    printf("%s%sLeaderboard\n%s",BOLD,YELLOW,RESET);
    printf("%s%s----------------------------\n%s",BOLD,YELLOW,RESET);

    FILE *player_file = fopen("players.txt", "r");

    Player players[100];
    int player_count = 0;
    char name[50];
    int wins, losses;

    while (fscanf(player_file, "%49[^,],%d,%d\n", name, &wins, &losses) == 3) {
        strcpy(players[player_count].name, name);
        players[player_count].wins = wins;
        players[player_count].losses = losses;
        player_count++;
    }
    fclose(player_file);

    if (filter == 1) {
        qsort(players, player_count, sizeof(Player), cmp_winrate);
    }
    else if (filter == 2) {
        qsort(players, player_count, sizeof(Player), cmp_wins);
    }
    else if (filter == 3) {
        qsort(players, player_count, sizeof(Player), cmp_losses);
    }

    for (int i = 0; i < player_count; i++) {
        if (filter == 1) { 
            double winrate = ((double)players[i].wins / (players[i].wins + players[i].losses)) * 100 ;
            printf("%d. %s - %.2f%%\n", i + 1, players[i].name, winrate);
        }
        else if (filter == 2) { 
            printf("%d. %s - Wins: %d\n", i + 1, players[i].name, players[i].wins);
        }
        else if (filter == 3) { 
            printf("%d. %s - Losses: %d\n", i + 1, players[i].name, players[i].losses);
        }
    }

    printf("%s%s----------------------------\n%s",BOLD,YELLOW,RESET);
    printf("Enter Player name for individual stats and match history:");
    char individual[50];
    scanf("%s",individual);
    getchar();
    player(individual, players, player_count);
}

void menu(int op)
{
    switch(op)
    {
    case 1:
        add();
        break;
    case 2:
        FILE *file=fopen("players.txt","w");
        fclose(file);
        leaderboard();
        break;
    default:
        printf("%s%sEXIT!\n%s",BOLD,RED,RESET);
    }
}

int main()
{
    int option;

    do{
        header();
        printf("%sEnter option: %s",BOLD,RESET);
        scanf("%d", &option);
        menu(option);
    } while(option > 0 && option < 3);
    return 0;
}
