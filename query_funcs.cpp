#include "query_funcs.h"
#include <iomanip>
#include <sstream>

void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg) {
    work W(*C);
    stringstream sql;
    sql << "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, "
           "RPG, APG, SPG, BPG) VALUES ("
        << team_id << ", " << jersey_num << ", " << W.quote(first_name) << ", "
        << W.quote(last_name) << ", " << mpg << ", " << ppg << ", " << rpg << ", " << apg
        << ", " << spg << ", " << bpg << ");";
    W.exec(sql.str());
    W.commit();
}


void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses) {
    work W(*C);
    stringstream sql;
    sql << "INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES ("
        << W.quote(name) << ", " << state_id << ", " << color_id << ", " << wins << ", "
        << losses << ");";
    W.exec(sql.str());
    W.commit();
}


void add_state(connection *C, string name) {
    work W(*C);
    string sql;
    sql = "INSERT INTO STATE (NAME) VALUES (" + W.quote(name) + ");";
    W.exec(sql);
    W.commit();
}


void add_color(connection *C, string name) {
    work W(*C);
    string sql;
    sql = "INSERT INTO COLOR (NAME) VALUES (" + W.quote(name) + ");";
    W.exec(sql);
    W.commit();
}

struct int_dou {
    int int_data[4];
    double dou_data[2];
};
typedef struct int_dou int_dou;

void query1(connection *C,
            int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg
) {
    int use_arr[6] = {use_mpg, use_ppg, use_rpg, use_apg, use_spg, use_bpg};
    int_dou min_arr = {min_mpg, min_ppg, min_rpg, min_apg, min_spg, min_bpg};
    int_dou max_arr = {max_mpg, max_ppg, max_rpg, max_apg, max_spg, max_bpg};
    string XPG[6] = {"MPG", "PPG", "RPG", "APG", "SPG", "BPG"};
    stringstream sql;
    sql << "SELECT * FROM PLAYER";
    bool first_flag = true;
    for (int i = 0; i < 6; i++) {
        if (use_arr[i]) {
            if (first_flag) {
                sql << " WHERE ";
                first_flag = false;
            } else
                sql << " AND ";
            if (i < 4) {
                sql << XPG[i] << " >= " << min_arr.int_data[i] << " AND " << XPG[i]
                    << " <= " << max_arr.int_data[i];
            } else
                sql << XPG[i] << " >= " << min_arr.dou_data[i - 4] << " AND " << XPG[i]
                    << " <= " << max_arr.dou_data[i - 4];
        }
    }
    sql << ";";

    nontransaction N(*C);
    result R(N.exec(sql.str()));
    cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG"
         << endl;
    for (auto c: R) {
        cout << c[0].as<int>() << " " << c[1].as<int>() << " " << c[2].as<int>() << " "
             << c[3].as<string>() << " " << c[4].as<string>() << " " << c[5].as<int>() << " "
             << c[6].as<int>() << " " << c[7].as<int>() << " " << c[8].as<int>() << " " << setprecision(1) << fixed
             << c[9].as<double>() << " " << c[10].as<double>() << endl;
    }
}


void query2(connection *C, string team_color) {
    work W(*C);
    stringstream sql;
    sql << "SELECT TEAM.NAME FROM TEAM, COLOR WHERE TEAM.COLOR_ID = COLOR.COLOR_ID AND "
           "COLOR.NAME = "
        << W.quote(team_color) << ";";
    W.commit();
    nontransaction N(*C);
    result R(N.exec(sql.str()));
    cout << "NAME" << endl;
    for (auto c: R) {
        cout << c[0].as<string>() << endl;
    }
}


void query3(connection *C, string team_name) {
    work W(*C);
    stringstream sql;
    sql << "SELECT FIRST_NAME, LAST_NAME FROM PLAYER, TEAM WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.NAME = "
        << W.quote(team_name) << " ORDER BY PPG DESC;";
    W.commit();
    nontransaction N(*C);
    result R(N.exec(sql.str()));
    cout << "FIRST_NAME LAST_NAME" << endl;
    for (auto c: R) {
        cout << c[0].as<string>() << " " << c[1].as<string>() << endl;
    }
}


void query4(connection *C, string team_state, string team_color) {
    work W(*C);
    stringstream sql;
    sql << "SELECT UNIFORM_NUM, FIRST_NAME, LAST_NAME FROM PLAYER, STATE, COLOR, TEAM "
           "WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.STATE_ID = STATE.STATE_ID "
           "AND TEAM.COLOR_ID = COLOR.COLOR_ID AND STATE.NAME = "
        << W.quote(team_state) << " AND COLOR.NAME = "
        << W.quote(team_color) << ";";
    W.commit();
    nontransaction N(*C);
    result R(N.exec(sql.str()));
    cout << "UNIFORM_NUM FIRST_NAME LAST_NAME" << endl;
    for (auto c: R) {
        cout << c[0].as<int>() << " " << c[1].as<string>() << " " << c[2].as<string>() << endl;
    }
}


void query5(connection *C, int num_wins) {
    work W(*C);
    stringstream sql;
    sql << "SELECT FIRST_NAME, LAST_NAME, NAME, WINS FROM PLAYER, TEAM "
           "WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID "
           "AND WINS >" << num_wins << ";";
    W.commit();
    nontransaction N(*C);
    result R(N.exec(sql.str()));
    cout << "FIRST_NAME LAST_NAME NAME WINS" << endl;
    for (auto c: R) {
        cout << c[0].as<string>() << " " << c[1].as<string>() << " "
             << c[2].as<string>() << " " << c[3].as<int>() << endl;
    }
}
