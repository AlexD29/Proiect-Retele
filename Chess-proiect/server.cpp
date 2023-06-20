#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ctype.h>
#include <iostream>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

using namespace std;
#define PORT 2024

void fnc_child_done(int signo)
{
    while(waitpid(0,NULL,WNOHANG)>0);
}

char * conv_addr (struct sockaddr_in address)
{
    static char str[25];
    char port[7];

    strcpy (str, inet_ntoa (address.sin_addr));
    bzero (port, 7);
    sprintf (port, ":%d", ntohs (address.sin_port));
    strcat (str, port);
    return (str);
}

string convert_board_into_string(char board[100][100])
{
    int i;
    string string="";

    for (i = 0; i < 100; i++) 
    {
        if (i != 0 && i % 10 == 0)
        {
            string += "\n";
        }
        string += board[i];
        string += " ";
    }
    return string;
        
}

string get_piece_from_position(int i,int j,char board[100][100])
{
    return board[i*10+j];
}

string get_piece(string move_message)
{
    return move_message.substr(0,2);
}

string get_move(string move_message)
{
    return move_message.substr(3,5);
}

string move_errors(int verify_error)
{
    if(verify_error%100==0)
    {
        return "\n=================================================================================\n\nInvalid move.\nYou may not kill your own pieces.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==10)
    {
        return "\n=================================================================================\n\nInvalid move.\nYour move must have 8 characters.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==20)
    {
        return "\n=================================================================================\n\nInvalid move.\nThe piece you selected does not exist.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==30)
    {
        return "\n=================================================================================\n\nInvalid move.\nThe piece you selected is not on this position.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==40)
    {
        return "\n=================================================================================\n\nInvalid move.\nYou can't move your opponent's pieces!\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==50)
    {
        return "\n=================================================================================\n\nInvalid move.\nThe coordinate from which you are moving is the same as the destination coordinate.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==60)
    {
        return "\n=================================================================================\n\nInvalid move.\nYou must use dashes('-') to separate the coordinates.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==51)
    {
        return "\n=================================================================================\n\nInvalid move.\nThe coordinate from which you are moving is not correct.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==52)
    {
        return "\n=================================================================================\n\nInvalid move.\nThe destination coordinate is not correct.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==53)
    {
        return "\n=================================================================================\n\nInvalid move.\nThe coordinates you entered are not correct.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==110)
    {
        return "\n=================================================================================\n\nInvalid move.\nA Rook moves any number of vacant squares horizontally or vertically.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==120)
    {
        return "\n=================================================================================\n\nInvalid move.\nRocks cannot jump over other pieces.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==210)
    {
        return "\n=================================================================================\n\nInvalid move.\nA Knight moves two squares horizontally then one square vertically, OR one square horizontally then two squares vertically—i.e. in an 'L' pattern.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==310)
    {
        return "\n=================================================================================\n\nInvalid move.\nA Bishop moves any number of vacant squares diagonally.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==320)
    {
        return "\n=================================================================================\n\nInvalid move.\nBishops cannot jump over other pieces.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==410)
    {
        return "\n=================================================================================\n\nInvalid move.\nThe King moves exactly one square horizontally, vertically, or diagonally.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==510)
    {
        return "\n=================================================================================\n\nInvalid move.\nThe Queen moves any number of vacant squares horizontally, vertically, or diagonally.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==520)
    {
        return "\n=================================================================================\n\nInvalid move.\nThe Queen cannot jump over other pieces.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==610)
    {
        return "\n=================================================================================\n\nInvalid move.\nA Pawn moves straight forward one square, if that square is vacant. If it has not yet moved, a Pawn also has the option of moving two squares straight forward, provided both squares are vacant. Pawns cannot move backwards.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==620)
    {
        return "\n=================================================================================\n\nInvalid move.\nPawns cannot jump over other pieces.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==630)
    {
        return "\n=================================================================================\n\nInvalid move.\nA Pawn can capture an enemy piece on either of the two squares diagonally in front of the pawn. It cannot move to those squares when vacant.\nPlease enter a correct move:\n\n";
    }
    else if(verify_error==640)
    {
        return "\n=================================================================================\n\nInvalid move.\nA Pawn can capture an enemy piece on either of the two squares diagonally in front of the pawn.\nPlease enter a correct move:\n\n";
    }
    return "Nimic";
}

void to_upper(char* username)
{
    int j = 0;
    while (username[j])
    {
        username[j]=toupper(username[j]);
        j++;
    }
}

void translate(string from, string dest, int& i1, int& j1, int& i2, int& j2)
{
    if(from.substr(1,1)=="1")
    {   
        i1=1;
    }
    else if(from.substr(1,1)=="2")
    {   
        i1=2;
    }
    else if(from.substr(1,1)=="3")
    {   
        i1=3;
    }
    else if(from.substr(1,1)=="4")
    {   
        i1=4;
    }
    else if(from.substr(1,1)=="5")
    {   
        i1=5;
    }
    else if(from.substr(1,1)=="6")
    {   
        i1=6;
    }
    else if(from.substr(1,1)=="7")
    {   
        i1=7;
    }
    else if(from.substr(1,1)=="8")
    {   
        i1=8;
    }

    if(from.substr(0,1)=="a")
    {   
        j1=1;
    }
    else if(from.substr(0,1)=="b")
    {   
        j1=2;
    }
    else if(from.substr(0,1)=="c")
    {   
        j1=3;
    }
    else if(from.substr(0,1)=="d")
    {   
        j1=4;
    }
    else if(from.substr(0,1)=="e")
    {   
        j1=5;
    }
    else if(from.substr(0,1)=="f")
    {   
        j1=6;
    }
    else if(from.substr(0,1)=="g")
    {   
        j1=7;
    }
    else if(from.substr(0,1)=="h")
    {   
        j1=8;
    }


    if(dest.substr(1,1)=="1")
    {   
        i2=1;
    }
    else if(dest.substr(1,1)=="2")
    {   
        i2=2;
    }
    else if(dest.substr(1,1)=="3")
    {   
        i2=3;
    }
    else if(dest.substr(1,1)=="4")
    {   
        i2=4;
    }
    else if(dest.substr(1,1)=="5")
    {   
        i2=5;
    }
    else if(dest.substr(1,1)=="6")
    {   
        i2=6;
    }
    else if(dest.substr(1,1)=="7")
    {   
        i2=7;
    }
    else if(dest.substr(1,1)=="8")
    {   
        i2=8;
    }

    if(dest.substr(0,1)=="a")
    {   
        j2=1;
    }
    else if(dest.substr(0,1)=="b")
    {   
        j2=2;
    }
    else if(dest.substr(0,1)=="c")
    {   
        j2=3;
    }
    else if(dest.substr(0,1)=="d")
    {   
        j2=4;
    }
    else if(strcmp(dest.substr(0,1).c_str(),"e")==0)
    {   
        j2=5;
    }
    else if(dest.substr(0,1)=="f")
    {   
        j2=6;
    }
    else if(dest.substr(0,1)=="g")
    {   
        j2=7;
    }
    else if(dest.substr(0,1)=="h")
    {   
        j2=8;
    }
    
}

int first_in_alphabetical_order(char* username1,char* username2)
{
    char* username1_copy = strdup(username1);
    char* username2_copy = strdup(username2);
    to_upper(username1_copy);
    to_upper(username2_copy);
    while(*username1_copy != '\0'  && *username2_copy != '\0' )
    {
        if (*username1_copy < *username2_copy)
        {
            return 1;
        }
        else if (*username1_copy > *username2_copy)
        {
            return 2; 
        }
        username1_copy++;
        username2_copy++;
    }
    return 1;
    
}

int length_check(string move_message)
{
    if(move_message.length()!=8)
    {
        return 0;
    }
    return 1;
}

int valid_piece_check(string move_message)
{
    char piece[2];
    strcpy (piece,move_message.substr(0,2).c_str());
    if(strstr("bR,bN,bB,bK,bQ,bP,wP,wQ,wK,wB,wN,wR",piece)==0)
    {
        return 0;
    }
    return 1;
}

int position_check(string move_message,char board[100][100])
{
    int i1,j1,i2,j2,i,j;
    string move=get_move(move_message);
    string piece=get_piece(move_message);
    string from=move.substr(0,2);
    string dest=move.substr(3,2);
    
    translate(from,dest,i1,j1,i2,j2);
    string s=board[i1*10+j1];

    if(piece!=s)
    {
        return 0;
    }
    return 1;
}

int dashes_check(string move_message)
{
    string first_dash=move_message.substr(2,1);
    string second_dash=move_message.substr(5,1);

    if(first_dash!="-" || second_dash!="-")
    {
        return 0;
    }
    return 1;
}

int basic_check(string from, string dest)
{
    char possibilities[]="a1,a2,a3,a4,a5,a6,a7,a8,b1,b2,b3,b4,b5,b6,b7,b8,c1,c2,c3,c4,c5,c6,c7,c8,d1,d2,d3,d4,d5,d6,d7,d8,e1,e2,e3,e4,e5,e6,e7,e8,f1,f2,f3,f4,f5,f6,f7,f8,g1,g2,g3,g4,g5,g6,g7,g8,h1,h2,h3,h4,h5,h6,h7,h8";
    if(strstr(possibilities,from.c_str())==0 && strstr(possibilities,dest.c_str())==0)
    {
        return 53;
    }
    else if(strstr(possibilities,from.c_str())==0)
    {
        return 51;
    }
    else if(strstr(possibilities,dest.c_str())==0)
    {
        return 52;
    }
    else if(from==dest)
    {
        return 50;
    }
    return 1;
}

int rock_move_check(string move, char board[100][100])
{
    int i1,j1,i2,j2,i,j;
    string from=move.substr(0,2);
    string dest=move.substr(3,2);
    translate(from,dest,i1,j1,i2,j2);
    
    if(basic_check(from,dest)!=1)
    {
        basic_check(from,dest);
    }
    else if(get_piece_from_position(i1,j1,board).substr(0,1)==get_piece_from_position(i2,j2,board).substr(0,1))
    {
        return 100;
    }
    else if((j1!=j2) && (i1!=i2))
    {
        return 110;
    }
    else if((j1!=j2) && (i1==i2))
    {
        if(j1<j2)
        {
            for(j=i1*10+j1+1;j<i2*10+j2;j++)
                if(strcmp(board[j],"__")!=0)
                {
                    return 120;
                }
        }
        else if(j2<j1)
        {
            for(j=i1*10+j1-1;j>i2*10+j2;j--)
                if(strcmp(board[j],"__")!=0)
                {
                    return 120;
                }
        }
    }
    else if((j1==j2) && (i1!=i2))
    {
        if(i1<i2)
        {
            for(i=i1*10+j1+10;i<i2*10+j2;i=i+10)
                if(strcmp(board[i],"__")!=0)
                {
                    return 120;
                }
        }
        else if(i2<i1)
        {
            for(i=i1*10+j1-10;i>i2*10+j2;i=i-10)
                if(strcmp(board[i],"__")!=0)
                {
                    return 120;
                }
        }
    }
    return 1;
        
}

int knight_move_check(string move,char board[100][100])
{
    int i1,j1,i2,j2,i,j;
    string from=move.substr(0,2);
    string dest=move.substr(3,2);
    translate(from,dest,i1,j1,i2,j2);
    
    if(basic_check(from,dest)!=1)
    {
        return basic_check(from,dest);
    }
    else if(get_piece_from_position(i1,j1,board).substr(0,1)==get_piece_from_position(i2,j2,board).substr(0,1))
    {
        return 200;
    }
    else if(!(((i1==i2+2)&&(j1==j2+1))||((i1==i2+1)&&(j1==j2+2))||((i1==i2-1)&&(j1=j2+2))||((i1==i2-2)&&(j1==j2+1))||((i1==i2-2)&&(j1==j2-1))||((i1==i2-1)&&(j1==j2-2))||((i1==i2+1)&&(j1==j2-2))||((i1==i2+2)&&(j1==j2-1))))
    {
        return 210;
    }
    return 1;
}

int bishop_move_check(string move,char board[100][100])
{
    int i1,j1,i2,j2,i,j;
    string from=move.substr(0,2);
    string dest=move.substr(3,2);
    translate(from,dest,i1,j1,i2,j2);
    
    if(basic_check(from,dest)!=1)
    {
        return basic_check(from,dest);
    }
    else if(get_piece_from_position(i1,j1,board).substr(0,1)==get_piece_from_position(i2,j2,board).substr(0,1))
    {
        return 300;
    }
    else if((i1-i2!=j1-j2)&&(i1-i2!=j2-j1)&&(i2-i1!=j1-j2)&&(i2-i1!=j2-j1))
    {
        return 310;
    }
    else if(i2<i1 && j2<j1)
    {
        for(i=(i1-1)*10+j1-1;i>i2*10+j2;i=i-11)
        {
            if(strcmp(board[i],"__")!=0)
            {
                return 320;
            }
        }  
    }
    else if(i2<i1 && j1<j2)
    {
        for(i=(i1-1)*10+j1+1;i>i2*10+j2;i=i-9)
        {
            if(strcmp(board[i],"__")!=0)
            {
                return 320;
            }
        }       
    }
    else if(i1<i2 && j2<j1)
    {
        for(i=i1*10+j1+9;i<i2*10+j2;i=i+9)
        {
            if(strcmp(board[i],"__")!=0)
            {
                return 320;
            }
        }
            
    }
    else if(i1<i2 && j1<j2)
    {
        for(i=i1*10+j1+11;i<i2*10+j2;i=i+11)
        {
            if(strcmp(board[i],"__")!=0)
            {
                return 320;
            }
        }
    }
    return 1;
}

int king_move_check(string move,char board[100][100])
{
    int i1,j1,i2,j2,i,j;
    string from=move.substr(0,2);
    string dest=move.substr(3,2);
    translate(from,dest,i1,j1,i2,j2);
    
    if(basic_check(from,dest)!=1)
    {
        return basic_check(from,dest);
    }
    else if(get_piece_from_position(i1,j1,board).substr(0,1)==get_piece_from_position(i2,j2,board).substr(0,1))
    {
        return 400;
    }
    else if(!((i2==i1+1 && j2==j1)||(i2==i1+1 && j2==j1-1)||(i2==i1 && j2==j1-1)||(i2==i1-1 && j2==j1-1)||(i2==i1-1 && j2==j1)||(i2==i1-1 && j2==j1+1)||(i2==i1 && j2==j1+1)||(i2==i1+1 && j2==j1+1)))
    {
        return 410;
    }
    return 1;
}

int queen_move_check(string move,char board[100][100])
{
    int i1,j1,i2,j2,i,j;
    string from=move.substr(0,2);
    string dest=move.substr(3,2);
    translate(from,dest,i1,j1,i2,j2);

    if(basic_check(from,dest)!=1)
    {
        return basic_check(from,dest);
    }
    else if(get_piece_from_position(i1,j1,board).substr(0,1)==get_piece_from_position(i2,j2,board).substr(0,1))
    {
        return 500;
    }
    else
    {
        if((j1!=j2) && (i1==i2))
        {
            if(j1<j2)
            {
                for(j=i1*10+j1+1;j<i2*10+j2;j++)
                    if(strcmp(board[j],"__")!=0)
                    {
                        return 520;
                    }
            }
            else if(j2<j1)
            {
                for(j=i1*10+j1-1;j>i2*10+j2;j--)
                    if(strcmp(board[j],"__")!=0)
                    {
                        return 520;
                    }
            }
        }
        else if((j1==j2) && (i1!=i2))
        {
            if(i1<i2)
            {
                for(i=i1*10+j1+10;i<i2*10+j2;i=i+10)
                    if(strcmp(board[i],"__")!=0)
                    {
                        return 520;
                    }
            }
            else if(i2<i1)
            {
                for(i=i1*10+j1-10;i>i2*10+j2;i=i-10)
                    if(strcmp(board[i],"__")!=0)
                    {
                        return 520;
                    }
            }
        }
        else if(i2<i1 && j2<j1)
        {
            for(i=(i1-1)*10+j1-1;i>i2*10+j2;i=i-11)
                if(strcmp(board[i],"__")!=0)
                {
                    return 520;
                }
        }
        else if(i2<i1 && j1<j2)
        {
            for(i=(i1-1)*10+j1+1;i>i2*10+j2;i=i-9)
            {
                if(strcmp(board[i],"__")!=0)
                {
                    return 520;
                }
            }       
        }
        else if(i1<i2 && j2<j1)
        {
            for(i=i1*10+j1+9;i<i2*10+j2;i=i+9)
            {
                if(strcmp(board[i],"__")!=0)
                {
                    return 520;
                }
            }
                
        }
        else if(i1<i2 && j1<j2)
        {
            for(i=i1*10+j1+11;i<i2*10+j2;i=i+11)
            {
                if(strcmp(board[i],"__")!=0)
                {
                    return 520;
                }
            }
        }
        else return 510;
    }
    return 1;
}

int pawn_move_check(string color,string move,char board[100][100])
{
    int i1,j1,i2,j2;
    string from=move.substr(0,2);
    string dest=move.substr(3,2);
    translate(from,dest,i1,j1,i2,j2);

    if(basic_check(from,dest)!=1)
    {
        return basic_check(from,dest);
    }
    else if(get_piece_from_position(i1,j1,board).substr(0,1)==get_piece_from_position(i2,j2,board).substr(0,1))
    {
        return 600;
    }
    else if(color=="b")
    {
        if(i1==2)
        {
            if(i2==i1+2 && j1==j2)
            {
                if(!(strcmp(board[i2*10+j1],"__")==0 && (strcmp(board[(i2-1)*10+j1],"__")==0)))
                {
                    return 620;
                }
            }
            else if(i2==i1+1 && j1==j2)
            {
                if(!(strcmp(board[i2*10+j2],"__")==0))
                {
                    return 640;
                }
            }
            else if((i2==i1+1 && j1==j2+1)||(i2==i1+1 && j1==j2-1))
            {
                if(strcmp(board[i2*10+j2],"__")==0)
                {
                    return 630;
                }
            }
            else return 610;
            
        }
        else
        {
            if(i2==i1+1 && j1==j2)
            {
                if(!(strcmp(board[i2*10+j2],"__")==0))
                {
                    return 640;
                }
            }
            else if((i2==i1+1 && j1==j2+1)||(i2==i1+1 && j1==j2-1))
            {
                if(strcmp(board[i2*10+j2],"__")==0)
                {
                    return 630;
                }
            }
            else return 610;
        }
    }
    else if(color=="w")
    {
        if(i1==7)
        {
            if(i2==i1-2 && j1==j2)
            {
                if(!(strcmp(board[i2*10+j2],"__")==0 && (strcmp(board[(i2+1)*10+j2],"__")==0)))
                {
                    return 620;
                }
            }
            else if(i2==i1-1 && j1==j2)
            {
                if(!(strcmp(board[i2*10+j2],"__")==0))
                {
                    return 640;
                }
            }
            else if((i2==i1-1 && j1==j2+1)||(i2==i1-1 && j1==j2-1))
            {
                if(strcmp(board[i2*10+j2],"__")==0)
                {
                    return 630;
                }
            }
            else return 610;
            
        }
        else
        {
            if(i2==i1-1 && j1==j2)
            {
                if(!(strcmp(board[i2*10+j2],"__")==0))
                {
                    return 640;
                }
            }
            else if((i2==i1-1 && j1==j2+1)||(i2==i1-1 && j1==j2-1))
            {
                if(strcmp(board[i2*10+j2],"__")==0)
                {
                    return 630;
                }
            }
            else return 610;
        }
    }
    return 1;
}

int verify(string move_message,char board[100][100],string player_color)
{
    if(length_check(move_message)==0)
    {
        return 10;
    }
    else if(valid_piece_check(move_message)==0)
    {
        return 20;
    }
    else if(dashes_check(move_message)==0)
    {
        return 60;
    }
    else if(get_piece(move_message).substr(0,1)!=player_color)
    {
        return 40;
    }
    else if(get_piece(move_message).substr(1,1)=="R")
    {
        return rock_move_check(get_move(move_message),board);
    }
    else if(get_piece(move_message).substr(1,1)=="N")
    {
        return knight_move_check(get_move(move_message),board);
    }
    else if(get_piece(move_message).substr(1,1)=="B")
    {
        return bishop_move_check(get_move(move_message),board);
    }
    else if(get_piece(move_message).substr(1,1)=="K")
    {
        return king_move_check(get_move(move_message),board);
    }
    else if(get_piece(move_message).substr(1,1)=="Q")
    {
        return queen_move_check(get_move(move_message),board);
    }
    else if(get_piece(move_message).substr(1,1)=="P")
    {
        return pawn_move_check(get_piece(move_message).substr(0,1),get_move(move_message),board);
    }
    else if(position_check(move_message,board)==0)
    {
        return 30;
    }
    return 1;
}

void update_board(string move_message,char board[100][100],string &s,bool &win)
{
    int i1,j1,i2,j2,i,j;
    string move = get_move(move_message);
    string from=move.substr(0,2);
    string dest=move.substr(3,2);
    translate(from,dest,i1,j1,i2,j2);
    if(strstr(board[i2*10+j2],"Q")!=0)
    {
        win=true;
    }
    strcpy(board[i2*10+j2],board[i1*10+j1]);
    strcpy(board[i1*10+j1],"__");
    s=convert_board_into_string(board);
    
}

void disconnected(int nfds_disconnected,int nfds_remaining,char* player_disconnected_username,int& end_game)
{
    char player_disconnected_message[200]="\n\n=================================================================================\n\nGAME OVER!\n";
    strcat(player_disconnected_message,player_disconnected_username);
    strcat(player_disconnected_message," disconnected.\nYOU WON!\n\n");
    end_game=1;
    sleep(1);
    printf("The client with the descriptor %d has disconnected from from the game.\n",nfds_disconnected);
    send(nfds_remaining,player_disconnected_message,strlen(player_disconnected_message),MSG_NOSIGNAL);
}

void disconnected_nousername(int nfds_disconnected,int nfds_remaining,int& username_pass)
{
    char player_disconnected_message[]="\n\n=================================================================================\n\nThe opponent disconnected.\nYou'll need to reconnect to the server to find a new match.\n\n"; 
    username_pass=0;
    sleep(1);
    printf("The client with the descriptor %d has disconnected from from the game.\n",nfds_disconnected);
    send(nfds_remaining,player_disconnected_message,strlen(player_disconnected_message),MSG_NOSIGNAL);
}

void disconnected_ready(int nfds_disconnected,int nfds_remaining,char* player_disconnected_username,int& ready_pass)
{
    char player_disconnected_message[200]="\n\n=================================================================================\n\n";
    strcat(player_disconnected_message,player_disconnected_username);
    strcat(player_disconnected_message," disconnected.\nYou'll need to reconnect to the server to find a new match.\n\n");
    ready_pass=0;
    sleep(1);
    printf("The client with the descriptor %d has disconnected from from the game.\n",nfds_disconnected);
    send(nfds_remaining,player_disconnected_message,strlen(player_disconnected_message),MSG_NOSIGNAL);
}

void disconnected_initialize(int nfds_disconnected,int nfds_remaining,char* player_disconnected_username,int& initialize_pass)
{
    char player_disconnected_message[200]="\n\n=================================================================================\n\n";
    strcat(player_disconnected_message,player_disconnected_username);
    strcat(player_disconnected_message," disconnected.\nYou'll need to reconnect to the server to find a new match.\n\n");
    initialize_pass=0;
    sleep(1);
    printf("The client with the descriptor %d has disconnected from from the game.\n",nfds_disconnected);
    send(nfds_remaining,player_disconnected_message,strlen(player_disconnected_message),MSG_NOSIGNAL);
}

void move_made_succesfully(string move_message,int nfds_turn,int nfds_noturn,char* player_turn_username,char* player_noturn_username,char board[100][100],int & ok_move,int &player1_turn,int &player2_turn,int& end_game)
{
    string s;
    char announce_other_player[500];
    char show_table[200];
    bool win_player=false;
    char winner_message[]="\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\nGAME OVER\nYOU WON!CONGRATULATIONS!\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n";
    char loser_message[]="\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\nGAME OVER\nYOU LOST:( BETTER LUCK NEXT TIME!\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n";
    char final_table_and_message_winner[700]="\n\n=================================================================================\n\nYou captured your opponent's Queen!\n\n";
    char final_table_and_message_loser[700]="\n\n=================================================================================\n\nYour Queen has been captured!\n\n";
    char check_connection[]="CHECK_CONNECTION";
    string move = get_move(move_message);
    string from=move.substr(0,2);
    string dest=move.substr(3,2);
    string piece=get_piece(move_message);

    update_board(move_message,board,s,win_player);

    if(win_player==false)
    {
        strcpy(announce_other_player,"\n=================================================================================\n\n");
        strcat(announce_other_player,s.c_str());
        strcat(announce_other_player,"\n\n");
        strcat(announce_other_player,player_turn_username);
        strcat(announce_other_player," moved ");
        strcat(announce_other_player,piece.c_str());
        strcat(announce_other_player," from ");
        strcat(announce_other_player,from.c_str());
        strcat(announce_other_player," to ");
        strcat(announce_other_player,dest.c_str());
        strcat(announce_other_player,".\n");

        strcpy(show_table,"\n=================================================================================\n\n");
        strcat(show_table,move_message.c_str());
        strcat(show_table,"\n");
        strcat(show_table,s.c_str());

        sleep(1);
        if(send(nfds_turn,show_table,strlen(show_table),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds_turn,nfds_noturn,player_turn_username,end_game);
            ok_move=1;
            return;
        }
        if(send(nfds_noturn,announce_other_player,strlen(announce_other_player),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds_noturn,nfds_turn,player_noturn_username,end_game);
            ok_move=1;
            return;
        }
        ok_move=1;
        player1_turn=0;
        player2_turn=1;
    }
    else
    {
        sleep(1);
        strcat(final_table_and_message_winner,s.c_str());
        strcat(final_table_and_message_winner,winner_message);

        strcat(final_table_and_message_loser,s.c_str());
        strcat(final_table_and_message_loser,loser_message);

        if(send(nfds_turn,final_table_and_message_winner,strlen(final_table_and_message_winner),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds_turn,nfds_noturn,player_turn_username,end_game);
            ok_move=1;
            return;
        }
        if(send(nfds_noturn,final_table_and_message_loser,strlen(final_table_and_message_loser),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds_noturn,nfds_turn,player_noturn_username,end_game);
            ok_move=1;
            return;
        }

        player1_turn=0;
        player2_turn=100;
        end_game=1;
        ok_move=1;
        printf("The client with the descriptor %d won the game.\n",nfds_turn);
        fflush(stdout);
    }
    sleep(2);
}

void get_usernames(int nfds1,int nfds2,char* player1_username,char* player2_username,int& username_pass)
{
    sleep(1);
    int username1=0,username2=0,got_both_usernames=0;
    char request_username[]="Choose an username(at least one character) and press ENTER: ";
    char username_player1[100];
    char username_player2[100];
    char incorrect_username[]="\nYou have to use at least one character:\nChoose your username: ";
    
    int flags1 = fcntl(nfds1, F_GETFL);
    flags1 &= ~O_NONBLOCK;
    fcntl(nfds1, F_SETFL, flags1);
    int flags2 = fcntl(nfds2, F_GETFL);
    flags2 &= ~O_NONBLOCK;
    fcntl(nfds2, F_SETFL, flags2);

    while(got_both_usernames==0)
    {
        if(username1==0)
        {
            if(send(nfds1, request_username, sizeof(request_username),MSG_NOSIGNAL) == -1)
            {
                disconnected_nousername(nfds1,nfds2,username_pass);
                break;
            }
            username1=2;
        }
        
        if(username2==0)
        {
            if(send(nfds2, request_username, sizeof(request_username),MSG_NOSIGNAL) == -1)
            {
                disconnected_nousername(nfds2,nfds1,username_pass);
                break;
            }
            username2=2;
        }
        
        if(username1==2)
        {
            if(recv(nfds1, username_player1, sizeof(username_player1),MSG_NOSIGNAL) == 0)
            {
                disconnected_nousername(nfds1,nfds2,username_pass);
                break;

            }
            if(strcmp(username_player1,"\n")!=0)
            {
                username_player1[strlen(username_player1)-1] = '\0';
                strcpy(player1_username,username_player1);
                username1=1;
            }
            else
            {
                if(send(nfds1, incorrect_username, sizeof(incorrect_username),MSG_NOSIGNAL) == -1)
                {
                    disconnected_nousername(nfds1,nfds2,username_pass);
                    break;
                }
            }
        }
        
        if(username2==2)
        {
            if(recv(nfds2, username_player2, sizeof(username_player2),MSG_NOSIGNAL) == 0)
            {
                disconnected_nousername(nfds2,nfds1,username_pass);
                break;
            }
            if(strcmp(username_player2,"\n")!=0)
            {
                username_player2[strlen(username_player2)-1] = '\0';
                strcpy(player2_username,username_player2);
                username2=1;
            }
            else
            {
                if(send(nfds2, incorrect_username, sizeof(incorrect_username),MSG_NOSIGNAL) == -1)
                {
                    disconnected_nousername(nfds2,nfds1,username_pass);
                    break;
                }
            }
        }

        if(username1==1 && username2==1)
        {
            got_both_usernames=1;
        }
    }
}

void get_ready(int nfds1,int nfds2,char* player1_username,char* player2_username,int& ready_pass)
{
    sleep(1);
    int ready1=0,ready2=0;
    int ok3=0;int ok4=0;
    char ENTER_player1[100];
    char ENTER_player2[100];
    char ready_player_1[200]="\n=================================================================================\n\nYour opponent is ";
    strcat(ready_player_1,player2_username);
    strcat(ready_player_1,".\nPress ENTER when you're ready");
    char ready_player_2[200]="\n=================================================================================\n\nYour opponent is ";
    strcat(ready_player_2,player1_username);
    strcat(ready_player_2,".\nPress ENTER when you're ready");
    char check_connection[]="CHECK_CONNECTION";

    while(1)
    {
        if (ready1==0 && ok3==0)
        {
            if(send(nfds1, ready_player_1, strlen(ready_player_1),MSG_NOSIGNAL) == -1)
            {
                disconnected_ready(nfds1,nfds2,player1_username,ready_pass);
                break;
            }
            ok3=1;
        }
        if (ready2==0 && ok4==0)
        {
            if(send(nfds2, ready_player_2, strlen(ready_player_2),MSG_NOSIGNAL) == -1)
            {
                disconnected_ready(nfds2,nfds1,player2_username,ready_pass);
                break;
            }
            ok4=1;
        }

        sleep(1);
        if(send(nfds1, check_connection, strlen(check_connection),MSG_NOSIGNAL) == -1)
        {
            disconnected_ready(nfds1,nfds2,player1_username,ready_pass);
            break;
        }
        if(send(nfds2, check_connection, strlen(check_connection),MSG_NOSIGNAL) == -1)
        {
            disconnected_ready(nfds2,nfds1,player2_username,ready_pass);
            break;
        }

        if (ready1==0)
            if(recv(nfds1, ENTER_player1, 100,MSG_NOSIGNAL) == 0)
            {
                disconnected_ready(nfds1,nfds2,player1_username,ready_pass);
                break;
            }
            else
            {
                if(strstr(ENTER_player1,"\n")!=0)
                {
                    ready1=1;
                }
                
            }
        if (ready2==0)
            if(recv(nfds2, ENTER_player2, 100,MSG_NOSIGNAL) == 0)
            {
                disconnected_ready(nfds2,nfds1,player2_username,ready_pass);
                break;
            }
            else
            {
                if(strstr(ENTER_player2,"\n")!=0)
                {
                    ready2=1;
                }
                
            }
        if(ready1==1 && ready2==1)
            break;
    }
}

void initialize_game(int turn,int nfds1,int nfds2,char* player1_username,char* player2_username,int &player1_turn,int &player2_turn,char board[100][100],int& initialize_pass)
{
    char msg_first[800];
    char msg_second[800];
    char msg_player_first[]="=================================================================================\n\nSTART GAME!\n\nThis is a feminist chess. To win the game, you have to capture your opponent's Queen.\nGOOD LUCK!\n\nThe notations are:\n- R for a Rook\n- N for a Knight\n- B for a Bishop\n- K for a King\n- Q for a Queen\n- P for a Pawn\n\nTo distinguish the colors, the white pieces will be preceded by 'w' and the black pieces will be preceded by 'b'.\n\n";
    char msg_player_second[]="=================================================================================\n\nSTART GAME!\n\nThis is a feminist chess. To win the game, you have to capture your opponent's Queen.\nGOOD LUCK!\n\nThe notations are:\n- R for a Rook\n- N for a Knight\n- B for a Bishop\n- K for a King\n- Q for a Queen\n- P for a Pawn\n\nTo distinguish the colors, the white pieces will be preceded by 'w' and the black pieces will be preceded by 'b'.\n\n";

    string s=convert_board_into_string(board);
    strcpy(msg_first,msg_player_first);
    strcat(msg_first,s.c_str());
    strcat(msg_first,"\n\nYou have the WHITE pieces (those starting with 'w').\n");
    strcpy(msg_second,msg_player_second);
    strcat(msg_second,s.c_str());
    strcat(msg_second,"\n\nYou have the BLACK pieces (those starting with 'b').\n");

    if(turn==1)
    {
        if(send(nfds1,msg_first,strlen(msg_first),MSG_NOSIGNAL) == -1)
        {
            disconnected_initialize(nfds1,nfds2,player1_username,initialize_pass);
        }
        if(send(nfds2,msg_second,strlen(msg_second),MSG_NOSIGNAL) == -1)
        {
            disconnected_initialize(nfds2,nfds1,player2_username,initialize_pass);
        }
        player1_turn=1;
    }
    else
    {
        if(send(nfds2,msg_first,strlen(msg_first),MSG_NOSIGNAL) == -1)
        {
            disconnected_initialize(nfds2,nfds1,player2_username,initialize_pass);
        }
        if(send(nfds1,msg_second,strlen(msg_second),MSG_NOSIGNAL) == -1)
        {
            disconnected_initialize(nfds1,nfds2,player1_username,initialize_pass);
        }
        player2_turn=1;
    }
}

void make_a_move(int nfds1,int nfds2,char* player1_username,char*player2_username,char board[100][100], string player_color,int& player1_turn,int& player2_turn,int& end_game)
{
    sleep(1);
    int ok_move=0; int k=1;
    int verify_error=0;
    string error;
    char move_msg[100];
    char error_msg[100];
    char move_sintax[]="\n=================================================================================\n\nEach square on a chess board is given by a coordinate.\nTo make a move, you have to specify:\n\t-the piece you want to move;\n\t-the coordinate from which you are moving;\n\t-the destination coordinate.\nex: bR-e3-e5\n\n";
    
    while (ok_move==0)
    {
        if(player1_turn==2 && k==1)
        {
            int flags = fcntl(nfds1, F_GETFL);
            flags &= ~O_NONBLOCK;
            fcntl(nfds1, F_SETFL, flags);

            if(send(nfds1,move_sintax,sizeof(move_sintax),MSG_NOSIGNAL)== -1)
            {
                disconnected(nfds1,nfds2,player1_username,end_game);
                break;
            }
            if(recv(nfds1,move_msg,sizeof(move_msg),MSG_NOSIGNAL) == 0)
            {
                disconnected(nfds1,nfds2,player1_username,end_game);
                break;
            }

            move_msg[strlen(move_msg)-1] = '\0';
            string move_message(move_msg);
            if(verify(move_message,board,player_color)==1)
            {
                move_made_succesfully(move_message,nfds1,nfds2,player1_username,player2_username,board,ok_move,player1_turn,player2_turn,end_game);
            }
            else
            {
                k=0;
                verify_error=verify(move_message,board,player_color);
            }

        }
        else if(player1_turn==2 && k==0)
        {
            int flags = fcntl(nfds1, F_GETFL);
            flags &= ~O_NONBLOCK;
            fcntl(nfds1, F_SETFL, flags);

            sleep(0.5);
            error=move_errors(verify_error);

            if(send(nfds1,error.c_str(),error.length(),MSG_NOSIGNAL) == -1)
            {
                disconnected(nfds1,nfds2,player1_username,end_game);
                break;
            }
            if(recv(nfds1,move_msg,sizeof(move_msg),MSG_NOSIGNAL) == 0)
            {
                disconnected(nfds1,nfds2,player1_username,end_game);
                break;
            }

            move_msg[strlen(move_msg)-1] = '\0';
            string move_message(move_msg);

            if(verify(move_message,board,player_color)==1)
            {
                move_made_succesfully(move_message,nfds1,nfds2,player1_username,player2_username,board,ok_move,player1_turn,player2_turn,end_game);
            }
            else
            {
                verify_error=verify(move_message,board,player_color);
            }
        }
        else if(player2_turn==2 && k==1)
        {
            int flags = fcntl(nfds2, F_GETFL);
            flags &= ~O_NONBLOCK;
            fcntl(nfds2, F_SETFL, flags);

            if(send(nfds2,move_sintax,sizeof(move_sintax),MSG_NOSIGNAL) == -1)
            {
                disconnected(nfds2,nfds1,player2_username,end_game);
                break;
            }
            if(recv(nfds2,move_msg,sizeof(move_msg),MSG_NOSIGNAL) == 0)
            {
                disconnected(nfds2,nfds1,player2_username,end_game);
                break;
            }

            move_msg[strlen(move_msg)-1] = '\0';
            string move_message(move_msg);
            
            if(verify(move_message,board,player_color)==1)
            {
                move_made_succesfully(move_message,nfds2,nfds1,player2_username,player1_username,board,ok_move,player2_turn,player1_turn,end_game);
            }
            else
            {
                k=0;
                verify_error=verify(move_message,board,player_color);
            }
            
        }
        else if(player2_turn==2 && k==0)
        {
            int flags = fcntl(nfds2, F_GETFL);
            flags &= ~O_NONBLOCK;
            fcntl(nfds2, F_SETFL, flags);

            sleep(0.5);
            error=move_errors(verify_error);

            if(send(nfds2,error.c_str(),error.length(),MSG_NOSIGNAL) == -1)
            {
                disconnected(nfds2,nfds1,player2_username,end_game);
                break;
            }
            if(recv(nfds2,move_msg,sizeof(move_msg),MSG_NOSIGNAL) == 0)
            {
                disconnected(nfds2,nfds1,player2_username,end_game);
                break;
            }

            move_msg[strlen(move_msg)-1] = '\0';
            string move_message(move_msg);

            if(verify(move_message,board,player_color)==1)
            {
                move_made_succesfully(move_message,nfds2,nfds1,player2_username,player1_username,board,ok_move,player2_turn,player1_turn,end_game);
            }
            else
            {
                verify_error=verify(move_message,board,player_color);
            } 
        } 
    }
}

void send_a_message(int nfds1,int nfds2, char* player1_username,char* player2_username,int& player1_turn,int& player2_turn,int& end_game)
{
    sleep(1);
    char send_syntax[200]="\n=================================================================================\n\nWrite your message to ";
    char send_msg[300];
    char response[500];
    char check_connection[]="CHECK_CONNECTION";
    
    if(player1_turn==2)
    {
        int flags = fcntl(nfds1, F_GETFL);
        flags &= ~O_NONBLOCK;
        fcntl(nfds1, F_SETFL, flags);

        strcpy(response,"\n=================================================================================\n\nMessage received from ");
        strcat(response,player1_username);
        strcat(response," and press ENTER:\n");

        strcat(send_syntax,player2_username);
        strcat(send_syntax,":\n");
        
        if(send(nfds1,send_syntax,strlen(send_syntax),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds1,nfds2,player1_username,end_game);
        }
        if(recv(nfds1,send_msg,sizeof(send_msg),MSG_NOSIGNAL) == 0)
        {
            disconnected(nfds1,nfds2,player1_username,end_game);
        }
        else
        {
            sleep(1);
            strcat(response,send_msg);
            strcat(response,"You have to wait for your turn to reply.\n\n");

            if(send(nfds2,response,strlen(response),MSG_NOSIGNAL) == -1)
            {
                disconnected(nfds2,nfds1,player2_username,end_game);
            }
            
            player1_turn=1;
            player2_turn=3;
            sleep(1);
        }
        
    }
    else if(player2_turn==2)
    {
        int flags = fcntl(nfds2, F_GETFL);
        flags &= ~O_NONBLOCK;
        fcntl(nfds2, F_SETFL, flags);

        strcpy(response,"\n=================================================================================\n\nMessage received from ");
        strcat(response,player2_username);
        strcat(response,":\n");

        strcat(send_syntax,player1_username);
        strcat(send_syntax," and press ENTER:\n");

        if(send(nfds2,send_syntax,strlen(send_syntax),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds2,nfds1,player2_username,end_game);
        }
        if(recv(nfds2,send_msg,sizeof(send_msg),MSG_NOSIGNAL) == 0)
        {
            disconnected(nfds2,nfds1,player2_username,end_game);
        }
        else
        {
            sleep(1);
            strcat(response,send_msg);
            strcat(response,"You have to wait for your turn to reply.\n\n");

            if(send(nfds1,response,strlen(response),MSG_NOSIGNAL) == -1)
            {
                disconnected(nfds1,nfds2,player1_username,end_game);
            }
            
            player2_turn=1;
            player1_turn=3;
            sleep(1);
        }
    }
    
}

void request_a_draw(int nfds1,int nfds2, char* player1_username,char* player2_username,int& player1_turn,int& player2_turn,int& end_game,int& used_request_draw)
{
    sleep(1);
    int choose=0;
    char draw_message[]="\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\nGAME OVER!\nDRAW BY MUTUAL AGREEMENT.\nGOOD GAME!\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n";
    char declined_draw_message[200];
    char declined_confirmation[200];
    char confirmation_message[]="\n=================================================================================\n\nYou can request a draw once per game. Are you sure you want to request a draw?\nType 'y' and press ENTER to confirm.\nOR\nType anything else and press ENTER to go back to the options.\n\n";
    char wait_for_answer[]="\nWaiting for the opponent to give an answer...\n\n=================================================================================\n\n";
    char used_draw_message[]="\n=================================================================================\n\nYou have already requested a draw in this game.\nYou have to keep playing.\n";
    char between12[]="\n\n=================================================================================\n\nYou have to choose either 1.Accept or 2.Decline.\n\n";
    char accept_decline[200];
    char player_confirmation[100];
    char opponent_response[100];

    if(player1_turn==2 && used_request_draw==0)
    {
        if(send(nfds1,confirmation_message,strlen(confirmation_message),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds1,nfds2,player1_username,end_game);
            return;
        }
        if(recv(nfds1,player_confirmation,sizeof(player_confirmation),MSG_NOSIGNAL) == 0)
        {
            disconnected(nfds1,nfds2,player1_username,end_game);
            return;
        }
        else
        {
            player_confirmation[strlen(player_confirmation)-1] = '\0';
            if(strcmp(player_confirmation,"y")==0)
            {
                used_request_draw=1;
                sleep(1);
                strcpy(accept_decline,"\n=================================================================================\n\n");
                strcat(accept_decline,player1_username);
                strcat(accept_decline," has request a draw.\n1.Accept\n2.Decline\n\nTo choose an option, just write its number and press ENTER.\n\n");

                if(send(nfds1,wait_for_answer,strlen(wait_for_answer),MSG_NOSIGNAL) == -1)
                {
                    disconnected(nfds1,nfds2,player1_username,end_game);
                    return;
                }
                if(send(nfds2,accept_decline,strlen(accept_decline),MSG_NOSIGNAL) == -1)
                {
                    disconnected(nfds2,nfds1,player2_username,end_game);
                    return;
                }
                
                while(choose==0)
                {
                    if(recv(nfds2,opponent_response,sizeof(opponent_response),MSG_NOSIGNAL) == 0)
                    {
                        disconnected(nfds2,nfds1,player2_username,end_game);
                        return;
                    }
                    else
                    {
                        opponent_response[strlen(opponent_response)-1] = '\0';
                        if(strcmp(opponent_response,"1")==0)
                        {
                            sleep(2);

                            if(send(nfds1,draw_message,strlen(draw_message),MSG_NOSIGNAL) == -1)
                            {
                                disconnected(nfds1,nfds2,player1_username,end_game);
                                return;
                            }
                            if(send(nfds2,draw_message,strlen(draw_message),MSG_NOSIGNAL) == -1)
                            {
                                disconnected(nfds2,nfds1,player2_username,end_game);
                                return;
                            }

                            end_game=1;
                            choose=1;
                            printf("The game between client %d and client %d ended in a draw.\n",nfds1,nfds2);
                            fflush(stdout);
                        }
                        else if(strcmp(opponent_response,"2")==0)
                        {
                            sleep(1);
                            strcpy(declined_draw_message,player2_username);
                            strcat(declined_draw_message," has declined your request. The game continues.\n");
                            strcpy(declined_confirmation,"\n=================================================================================\n\nYou declined ");
                            strcat(declined_confirmation,player1_username);
                            strcat(declined_confirmation,"'s request. The game continues.\n");

                            if(send(nfds1,declined_draw_message,strlen(declined_draw_message),MSG_NOSIGNAL) == -1)
                            {
                                disconnected(nfds1,nfds2,player1_username,end_game);
                                return;
                            }
                            if(send(nfds2,declined_confirmation,strlen(declined_confirmation),MSG_NOSIGNAL) == -1)
                            {
                                disconnected(nfds2,nfds1,player2_username,end_game);
                                return;
                            }

                            player1_turn=1;
                            player2_turn=3;
                            choose=1;

                            sleep(1);
                        }
                        else
                        {
                            sleep(1);
                            if(send(nfds2,between12,strlen(between12),MSG_NOSIGNAL) == -1)
                            {
                                disconnected(nfds2,nfds1,player2_username,end_game);
                                return;
                            }
                        }
                    }
                }
                
            }
            else
            {
                player1_turn=1;
                player2_turn=3;
                sleep(1);
            }
        }
            
    }
    else if(player1_turn==2 && used_request_draw==1)
    {
        if(send(nfds1,used_draw_message,strlen(used_draw_message),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds1,nfds2,player1_username,end_game);
            return;
        }

        player1_turn=1;
        player2_turn=3;
        sleep(1);
    }
    else if(player2_turn==2 && used_request_draw==0)
    {
        if(send(nfds2,confirmation_message,strlen(confirmation_message),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds2,nfds1,player2_username,end_game);
            return;
        }
        if(recv(nfds2,player_confirmation,sizeof(player_confirmation),MSG_NOSIGNAL) == 0)
        {
            disconnected(nfds2,nfds1,player2_username,end_game);
            return;
        }
        else
        {
            player_confirmation[strlen(player_confirmation)-1] = '\0';
            if(strcmp(player_confirmation,"y")==0)
            {
                used_request_draw=1;
                sleep(1);
                strcpy(accept_decline,"\n=================================================================================\n");
                strcat(accept_decline,player2_username);
                strcat(accept_decline," has request a draw.\n1.Accept\n2.Decline\n\nTo choose an option, just write its number and press ENTER.\n\n");

                if(send(nfds2,wait_for_answer,strlen(wait_for_answer),MSG_NOSIGNAL) == -1)
                {
                    disconnected(nfds2,nfds1,player2_username,end_game);
                    return;
                }
                if(send(nfds1,accept_decline,strlen(accept_decline),MSG_NOSIGNAL) == -1)
                {
                    disconnected(nfds1,nfds2,player1_username,end_game);
                    return;
                }
                
                while(choose==0)
                {
                    if(recv(nfds1,opponent_response,sizeof(opponent_response),MSG_NOSIGNAL) == 0)
                    {
                        disconnected(nfds1,nfds2,player1_username,end_game);
                        return;
                    }
                    else
                    {
                        opponent_response[strlen(opponent_response)-1] = '\0';
                        if(strcmp(opponent_response,"1")==0)
                        {
                            sleep(2);

                            if(send(nfds1,draw_message,strlen(draw_message),MSG_NOSIGNAL) == 1)
                            {
                                disconnected(nfds1,nfds2,player1_username,end_game);
                                return;
                            }
                            if(send(nfds2,draw_message,strlen(draw_message),MSG_NOSIGNAL) == -1)
                            {
                                disconnected(nfds2,nfds1,player2_username,end_game);
                                return;
                            }

                            end_game=1;
                            choose=1;
                            printf("The game between client %d and client %d ended in a draw.\n",nfds1,nfds2);
                            fflush(stdout);
                        }
                        else if(strcmp(opponent_response,"2")==0)
                        {
                            sleep(1);
                            strcpy(declined_draw_message,player1_username);
                            strcat(declined_draw_message," has declined your request. The game continues.\n");
                            strcpy(declined_confirmation,"\n=================================================================================\n\nYou declined ");
                            strcat(declined_confirmation,player2_username);
                            strcat(declined_confirmation,"'s request. The game continues.\n");

                            if(send(nfds2,declined_draw_message,strlen(declined_draw_message),MSG_NOSIGNAL) == -1)
                            {
                                disconnected(nfds2,nfds1,player2_username,end_game);
                                return;
                            }
                            if(send(nfds1,declined_confirmation,strlen(declined_confirmation),MSG_NOSIGNAL) == -1)
                            {
                                disconnected(nfds1,nfds2,player1_username,end_game);
                                return;
                            }

                            player2_turn=1;
                            player1_turn=3;
                            choose=1;
                            sleep(1);
                        }
                        else
                        {
                            sleep(1);

                            if(send(nfds1,between12,strlen(between12),MSG_NOSIGNAL) == -1)
                            {
                                disconnected(nfds1,nfds2,player1_username,end_game);
                                return;
                            }
                        }
                    }
                }
            }
            else
            {
                player2_turn=1;
                player1_turn=3;
                sleep(1);
            }
        }
             
    }
    else if(player2_turn==2 && used_request_draw==1)
    {
        if(send(nfds2,used_draw_message,strlen(used_draw_message),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds2,nfds1,player2_username,end_game);
            return;
        }

        player2_turn=1;
        player1_turn=3;
        sleep(1);
    }
}

void surrender(int nfds1,int nfds2, char* player1_username,char* player2_username,int& player1_turn,int& player2_turn,int& end_game)
{
    sleep(1);
    char winner_message[]="\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\nGAME OVER\nYOUR OPPONENT SURRENDERED. YOU WON!\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n";
    char loser_message[]="\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\nGAME OVER\nYOU SURRENDERED:( BETTER LUCK NEXT TIME!\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n";
    char confirmation_message[]="\n=================================================================================\n\nAre you sure you want to surrender?\nType 'y' to confirm.\nPress any other button to go back to the options.\n\n";
    char never_give_up[]="\n=================================================================================\n\nIt looks like you changed your mind!\nNEVER GIVE UP!";
    char player_confirmation[10];

    if(player1_turn==2)
    {
        int flags1 = fcntl(nfds1, F_GETFL);
        flags1 &= ~O_NONBLOCK;
        fcntl(nfds1, F_SETFL, flags1);

        if(send(nfds1,confirmation_message,strlen(confirmation_message),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds1,nfds2,player1_username,end_game);
        }
        if(recv(nfds1,player_confirmation,sizeof(player_confirmation),MSG_NOSIGNAL) == 0)
        {
            disconnected(nfds1,nfds2,player1_username,end_game);
        }
        else
        {
            player_confirmation[strlen(player_confirmation)-1] = '\0';
            if(strcmp(player_confirmation,"y")==0)
            {
                sleep(2);

                if(send(nfds2,winner_message,strlen(winner_message),MSG_NOSIGNAL) == -1)
                {
                    disconnected(nfds2,nfds1,player2_username,end_game);
                }
                if(send(nfds1,loser_message,strlen(loser_message),MSG_NOSIGNAL) == -1)
                {
                    disconnected(nfds1,nfds2,player1_username,end_game);
                }
                end_game=1;
            }
            else
            {
                sleep(1);

                if(send(nfds1,never_give_up,strlen(never_give_up),MSG_NOSIGNAL) == -1)
                {
                    disconnected(nfds1,nfds2,player1_username,end_game);
                }

                player1_turn=1;
                player2_turn=3;
                sleep(1);
            }
        }
    }
    else if(player2_turn==2)
    {
        int flags2 = fcntl(nfds2, F_GETFL);
        flags2 &= ~O_NONBLOCK;
        fcntl(nfds2, F_SETFL, flags2);

        if(send(nfds2,confirmation_message,strlen(confirmation_message),MSG_NOSIGNAL) == -1)
        {
            disconnected(nfds2,nfds1,player2_username,end_game);
        }
        if(recv(nfds2,player_confirmation,sizeof(player_confirmation),MSG_NOSIGNAL) == 0)
        {
            disconnected(nfds2,nfds1,player2_username,end_game);
        }
        else
        {
            player_confirmation[strlen(player_confirmation)-1] = '\0';
            if(strcmp(player_confirmation,"y")==0)
            {
                sleep(2);

                if(send(nfds1,winner_message,strlen(winner_message),MSG_NOSIGNAL) == -1)
                {
                    disconnected(nfds1,nfds2,player1_username,end_game);
                }
                if(send(nfds2,loser_message,strlen(loser_message),MSG_NOSIGNAL) == -1)
                {
                    disconnected(nfds2,nfds1,player2_username,end_game);
                }

                end_game=1;
            }
            else
            {
                sleep(1);

                if(send(nfds2,never_give_up,strlen(never_give_up),MSG_NOSIGNAL) == -1)
                {
                    disconnected(nfds2,nfds1,player2_username,end_game);
                }

                player2_turn=1;
                player1_turn=3;
                sleep(1);
            }
        }
    }
}

void invalid_option(int nfds_turn,int nfds_noturn,char* player_turn_username, int& end_game)
{
    sleep(1);
    char invalid_option_msg[]="\n\n=================================================================================\n\nOption does not exist. Please choose an available option.\n1.Make a move\n2.Send a message to your opponent\n3.Request a draw\n4.Surrender\n\nTo choose an option, just write its number and press ENTER.\n\n";
    
    if(send(nfds_turn,invalid_option_msg,sizeof(invalid_option_msg),MSG_NOSIGNAL) == -1)
    {
        disconnected(nfds_turn,nfds_noturn,player_turn_username,end_game);
    }
}

void play_game(int nfds1,int nfds2)
{
    char board[100][100]= 
    {
        "  ","a ","b ","c ","d ","e ","f ","g ","h ","  ",
        "1 ","bR","bN","bB","bQ","bK","bB","bN","bR"," 1",
        "2 ","bP","bP","bP","bP","bP","bP","bP","bP"," 2",
        "3 ","__","__","__","__","__","__","__","__"," 3",
        "4 ","__","__","__","__","__","__","__","__"," 4",
        "5 ","__","__","__","__","__","__","__","__"," 5",
        "6 ","__","__","__","__","__","__","__","__"," 6",
        "7 ","wP","wP","wP","wP","wP","wP","wP","wP"," 7",
        "8 ","wR","wN","wB","wQ","wK","wB","wN","wR"," 8",
        "  ","a ","b ","c ","d ","e ","f ","g ","h ","  ",
    };

    int end_game=0;
    int username_pass=1,ready_pass=1,initialize_pass=1;
    int used_request_draw_player1=0;
    int used_request_draw_player2=0;
    int player1_turn,player2_turn;
    char player1_username[100]="\0", player2_username[100]="\0";
    char msg_player1[100];
    char msg_player2[100];
    char msgrasp_player1_first[300]="\n\n=================================================================================\n\nIt's your turn. What do you want to do?\n1.Make a move\n2.Send a message to your opponent\n3.Request a draw\n4.Surrender\n\nTo choose an option, just write its number and press ENTER.\n\n";
    char msgrasp_player1_second[300]="\n\n=================================================================================\n\nIt's not your turn, you have to wait until your opponent makes his move.\n\n";
    char msgrasp_player2_first[300]="\n\n=================================================================================\n\nIt's your turn. What do you want to do?\n1.Make a move\n2.Send a message to your opponent\n3.Request a draw\n4.Surrender\n\nTo choose an option, just write its number and press ENTER.\n\n";
    char msgrasp_player2_second[300]="\n\n=================================================================================\n\nIt's not your turn, you have to wait until your opponent makes his move.\n\n";

    get_usernames(nfds1,nfds2,player1_username,player2_username,username_pass);
    if(username_pass==1)
    {
        get_ready(nfds1,nfds2,player1_username,player2_username,ready_pass);
        if(ready_pass==1)
        {
            sleep(2);
            initialize_game(first_in_alphabetical_order(player1_username,player2_username),nfds1,nfds2,player1_username,player2_username,player1_turn,player2_turn,board,initialize_pass);
            if(initialize_pass==1)
            {
                sleep(2);
                string player1_color,player2_color;
                if(player1_turn==1)
                {
                    player1_color="w";
                    player2_color="b";
                }
                else
                {
                    player1_color="b";
                    player2_color="w";
                }

                while(end_game==0)
                {
                    bzero(msg_player1,sizeof(msg_player1));
                    bzero(msg_player2,sizeof(msg_player2));

                    if(player1_turn==1 && player2_turn==0)
                    {
                        if(send(nfds1,msgrasp_player1_first,strlen(msgrasp_player1_first),MSG_NOSIGNAL) == -1)
                        {
                            disconnected(nfds1,nfds2,player1_username,end_game);
                            break;
                        }
                        if(send(nfds2,msgrasp_player2_second,strlen(msgrasp_player2_second),MSG_NOSIGNAL) == -1)
                        {
                            disconnected(nfds2,nfds1,player2_username,end_game);
                            break;
                        }
                        player1_turn=2;
                    }
                    else if(player1_turn==1 && player2_turn==3)
                    {
                        if(send(nfds1,msgrasp_player1_first,strlen(msgrasp_player1_first),MSG_NOSIGNAL) == -1)
                        {
                            disconnected(nfds1,nfds2,player1_username,end_game);
                            break;
                        }
                        player1_turn=2;
                    }
                    else if(player2_turn==1 && player1_turn==0)
                    {
                        if(send(nfds2,msgrasp_player2_first,strlen(msgrasp_player2_first),MSG_NOSIGNAL) == -1)
                        {
                            disconnected(nfds2,nfds1,player2_username,end_game);
                            break;
                        }
                        if(send(nfds1,msgrasp_player1_second,strlen(msgrasp_player1_second),MSG_NOSIGNAL) == -1)
                        {
                            disconnected(nfds1,nfds2,player1_username,end_game);
                            break;
                        }
                        player2_turn=2;
                    }
                    else if(player2_turn==1 && player1_turn==3)
                    {
                        if(send(nfds2,msgrasp_player2_first,strlen(msgrasp_player2_first),MSG_NOSIGNAL) == -1)
                        {
                            disconnected(nfds2,nfds1,player2_username,end_game);
                            break;
                        }
                        player2_turn=2;
                    }

                    if(player1_turn==2)
                    {
                        if(recv(nfds1, msg_player1, sizeof(msg_player1),MSG_NOSIGNAL) == 0)
                        {
                            disconnected(nfds1,nfds2,player1_username,end_game);
                            break;
                        }
                        else if(strlen(msg_player1)!=0)
                        {
                            msg_player1[strlen(msg_player1)-1] = '\0';
                            if(strcmp(msg_player1,"1")==0)
                            {
                                make_a_move(nfds1,nfds2,player1_username,player2_username,board,player1_color,player1_turn,player2_turn,end_game);
                            }
                            else if(strcmp(msg_player1,"2")==0)
                            {
                                send_a_message(nfds1,nfds2,player1_username,player2_username,player1_turn,player2_turn,end_game);
                            }
                            else if(strcmp(msg_player1,"3")==0)
                            {
                                request_a_draw(nfds1,nfds2,player1_username,player2_username,player1_turn,player2_turn,end_game,used_request_draw_player1);
                            }
                            else if(strcmp(msg_player1,"4")==0)
                            {
                                surrender(nfds1,nfds2,player1_username,player2_username,player1_turn,player2_turn,end_game);
                            }
                            else
                            {
                                invalid_option(nfds1,nfds2,player1_username,end_game);
                                continue;
                            }
                        }
                    }
                    
                    if(player2_turn==2)
                    {
                        if(recv(nfds2, msg_player2, sizeof(msg_player2),MSG_NOSIGNAL) == 0)
                        {
                            disconnected(nfds2,nfds1,player2_username,end_game);
                            break;
                        }
                        else if(strlen(msg_player2)!=0)
                        {
                            msg_player2[strlen(msg_player2)-1] = '\0';
                            if(strcmp(msg_player2,"1")==0)
                            {
                                make_a_move(nfds1,nfds2,player1_username,player2_username,board,player2_color,player1_turn,player2_turn,end_game);
                            }
                            else if(strcmp(msg_player2,"2")==0)
                            {
                                send_a_message(nfds1,nfds2,player1_username,player2_username,player1_turn,player2_turn,end_game);
                            }
                            else if(strcmp(msg_player2,"3")==0)
                            {
                                request_a_draw(nfds1,nfds2,player1_username,player2_username,player1_turn,player2_turn,end_game,used_request_draw_player2);
                            }
                            else if(strcmp(msg_player2,"4")==0)
                            {
                                surrender(nfds1,nfds2,player1_username,player2_username,player1_turn,player2_turn,end_game);
                            }
                            else
                            {
                                invalid_option(nfds2,nfds1,player2_username,end_game);
                                continue;
                            }
                        }
                    }  
                }
            }            
        }        
    }   
}

int main ()
{
    int sd;
    int optval=1;
    int nr_of_rooms=0;
    struct sockaddr_in server;
    struct sockaddr_in from;
    
    if(signal(SIGCHLD,fnc_child_done) == SIG_ERR)
    {
        perror("Eroare la semnal");
        exit(3);
    }

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server]Eroare la socket().\n");
        return errno;
    }

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));

    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

      server.sin_family = AF_INET;
      server.sin_addr.s_addr = htonl (INADDR_ANY);
      server.sin_port = htons (PORT);

    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

    if (listen (sd, 5) == -1)
    {
        perror ("[server]Eroare la listen().\n");
        return errno;
    }

    fd_set readfds;
    fd_set actfds;
    int nfds,client;
    unsigned int length;
    struct timeval tv;

    FD_ZERO (&actfds);
    FD_SET (sd, &actfds);
    pid_t childpid;
    int v[504] = {0};

    tv.tv_sec = 1;
    tv.tv_usec = 0;
    nfds = sd;

    printf("Server open.\n\n");

    while (1)
    {
        bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds));

        select(nfds+1, &readfds, NULL, NULL, &tv);

        if (FD_ISSET (sd, &readfds))
        {
            length = sizeof (from);
            bzero (&from, sizeof (from));

            client = accept (sd, (struct sockaddr *) &from, &length);

            if (client < 0)
            {
                perror ("[server] Eroare la accept().\n");
                continue;
            }

            if (nfds < client)
            {
                nfds = client;
            }

            FD_SET (client, &actfds);
            printf("The client with the descriptor %d, from address %s, has connected to the server.\n",client, conv_addr (from));
            fflush(stdout);
        }

        if(nfds % 2 == 1 && nfds > 3 && !v[nfds - 1] && !v[nfds])
        {
            nr_of_rooms++;
            v[nfds - 1] = 1;
			v[nfds] = 1;
            printf("The room number %d has been created.\n",nr_of_rooms);
            if((childpid = fork()) == 0)
            {
                int flags = fcntl(nfds,F_GETFL,0);
                fcntl(nfds, F_SETFL, flags | O_NONBLOCK);
                int flags1 = fcntl(nfds-1,F_GETFL,0);
                fcntl(nfds-1, F_SETFL, flags1 | O_NONBLOCK);
                close(sd);

                play_game(nfds-1,nfds);

                FD_CLR (nfds-1, &actfds);
                close(nfds-1);
                FD_CLR(nfds, &actfds);
                close(nfds);
                v[nfds-1] = 0;
                v[nfds] = 0;
                printf("The room number %d has been closed.\n\n",nr_of_rooms);
                exit(0);
            }
        }
    } 
}
