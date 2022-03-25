#include <iostream>
#include <fstream>
#include <string>
#include <pqxx/pqxx>

#include "exerciser.h"

using namespace std;
using namespace pqxx;

//Execute all commands a certain sql file.
void runSQL(string sql_file, connection *C) {
    string sql;
    ifstream newfile;
    newfile.open(sql_file.c_str(), ifstream::in);
    if (newfile.is_open()) {
        string line;
        while (std::getline(newfile, line)) {
            sql += line;
        }
        newfile.close();
    }
    work W(*C);
    W.exec(sql);
    W.commit();
}

void insertState(string state_file, connection *C) {
    string name;
    int state_id;
    ifstream newfile;
    newfile.open(state_file.c_str(), ifstream::in);
    if (newfile.is_open()) {
        string line;
        stringstream ss;
        while (std::getline(newfile, line)) {
            ss.clear();
            ss << line;
            ss >> state_id >> name;
            add_state(C, name);
        }
        newfile.close();
    }
}

void insertColor(string color_file, connection *C) {
    string name;
    int color_id;
    ifstream newfile;
    newfile.open(color_file.c_str(), ifstream::in);
    if (newfile.is_open()) {
        string line;
        stringstream ss;
        while (std::getline(newfile, line)) {
            ss.clear();
            ss << line;
            ss >> color_id >> name;
            add_color(C, name);
        }
        newfile.close();
    }
}

void insertTeam(string team_file, connection *C) {
    string name;
    int team_id, state_id, color_id, wins, losses;
    ifstream newfile;
    newfile.open(team_file.c_str(), ifstream::in);
    if (newfile.is_open()) {
        string line;
        stringstream ss;
        while (std::getline(newfile, line)) {
            ss.clear();
            ss << line;
            ss >> team_id >> name >> state_id >> color_id >> wins >> losses;
            add_team(C, name, state_id, color_id, wins, losses);
        }
        newfile.close();
    }
}

void insertPlayer(string player_file, connection *C) {
    string first_name, last_name;
    int player_id, team_id, jersey_num, mpg, ppg, rpg, apg;
    double spg, bpg;
    ifstream newfile;
    newfile.open(player_file.c_str(), ifstream::in);
    if (newfile.is_open()) {
        string line;
        stringstream ss;
        while (std::getline(newfile, line)) {
            ss.clear();
            ss << line;
            ss >> player_id >> team_id >> jersey_num >> first_name >> last_name >> mpg >> ppg >> rpg >> apg >> spg
               >> bpg;
            add_player(C, team_id, jersey_num, first_name, last_name, mpg, ppg, rpg, apg, spg, bpg);
        }
        newfile.close();
    }
}

int main(int argc, char *argv[]) {

    //Allocate & initialize a Postgres connection object
    connection *C;

    try {
        //Establish a connection to the database
        //Parameters: database name, user name, user password
        C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
        if (C->is_open()) {
            cout << "Opened database successfully: " << C->dbname() << endl;
            //Drop all existed tables
            runSQL("drop_tables.sql", C);
            //Create all required tables
            runSQL("create_tables.sql", C);
            //Inset the data of state into the state table
            insertState("state.txt", C);
            //Inset the data of color into the color table
            insertColor("color.txt", C);
            //Inset the data of team into the team table
            insertTeam("team.txt", C);
            //Inset the data of player into the color table
            insertPlayer("player.txt", C);

            // cout<<"Successfully executed!"<<endl;
        } else {
            cout << "Can't open database" << endl;
            return 1;
        }
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return 1;
    }
    exercise(C);

    //Close database connection
    C->disconnect();

    return 0;
}


