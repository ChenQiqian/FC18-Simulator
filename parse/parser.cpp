#include "../game/game.h"
#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// 要完成的任务：从给的info里面解析出地图；注入到 simulator 中；
// 从 前一回合的 info 和这一回合的 command 中，调用simulator得出下一个回合
// 然后与下一个回合的 info 比较。
// 一个是全用他维护的信息，一个是用我们 Simulator 模拟的信息
// 两者比较，如果错了，立刻报错。

Game _g(15, 15, 4);  // 全用它的数据，确切的说是拿他的数据和我模拟出来的对照
Game _g_my(15, 15, 4);  // 全用我的数据

const int ROUND_NUMBER = 1201;

struct Round {
    vector<vector<int>> corps, tower, map, player;
};

size_t         round_cnt = 0;
vector<string> round_string[ROUND_NUMBER];
Round          round_info[ROUND_NUMBER];

string &L(size_t r, size_t l) {
    return round_string[r][l];
}

bool is_c(size_t r, size_t l, char c) {
    return (L(r, l)[0] == '#' && L(r, l)[1] == c);
}

void splitRoundInfo(istream &fin) {
    string        tmp;
    istringstream strin;

    while(true) {
        while(true) {
            getline(fin, tmp, '\n');
            if(tmp.length() <= 1) {  // maybe = 1? 有没有最后的换行符号？
                break;
            }
            round_string[round_cnt].push_back(tmp);
        }
        round_cnt++;
        if(round_string[round_cnt - 1].size() == 0) {
            break;
        }
        // fin.get(); // 假设只有一个 \n
    }
    // for(auto &round : round_string) {
    //     for(auto &line : round) {
    //         cerr << line << endl;
    //     }
    //     cerr << "------" << endl;
    // }
    for(size_t r = 0; r < round_cnt; r++) {
        size_t nowl = 0;
        // find corp
        // round_info.push_back({{},{},{},{}});
        while(nowl < round_string[r].size() && !is_c(r, nowl, 'c')) {
            nowl++;
        }
        nowl++;
        // process corp
        /*
        #corps<下面若干行为兵团信息>
        示例：
        #corps
        0 6 0
        1 11 0
        2 12 10
        3 3 11 4 1 -1 0 30 50 0 
        <一行3/10个数，所有存活兵团均被记录>
        <如果该兵团属性相比之前没有变化，则只有3个数，分别是兵团编号，x坐标与z坐标>
        <若兵团属性相比之前，至少一条发生变化，那么记录10个数，依次与下面的结构体信息对应>
        1        public string corps_id;     //兵团编号
        2        public string x;                       //坐标
        3        public string z;
        4        public string player;              //所属玩家
        5        public string type;                 //类型，0-战士，1-弓箭手，2-骑兵，3-建造，4-开拓
        6        public string builder_point;//工程兵团劳动力
        7        public string movePoint;    //行动力
        8        public string battlePoint;  //战斗力
        9        public string headlthPoint; //生命值
        10      public string newC;         //是否为新兵团，0/1
        */
        while(nowl < round_string[r].size() && !is_c(r, nowl, 't')) {
            strin = istringstream(L(r, nowl));
            int cid, x, y, player, type, builder_point, movepoint, battlepoint,
                healthpoint, newc;
            strin >> cid >> x >> y;
            strin >> player >> type >> builder_point >> movepoint >>
                battlepoint >> healthpoint >> newc;
            if(strin.fail()) {
                round_info[r].corps.push_back({cid, x, y});
            }
            else {
                round_info[r].corps.push_back({cid, x, y, player, type,
                                               builder_point, movepoint,
                                               battlepoint, healthpoint, newc});
            }

            nowl++;
        }
        nowl++;
        /*
        #towers<下面若干行为防御塔信息>
        示例：
        0 4 1
        1 10 1
        2 10 11
        3 2 13 4 15 4 10 120 27 2 0
        <一行3/11个数，所有存活防御塔均被记录>
        <如果该塔属性相比之前没有变化，则只有3个数，分别是塔编号，x坐标与z坐标>
        <若塔属性相比之前，至少一条发生变化，那么记录10个数，依次与下面的结构体信息对应>
        1        public string tower_id;               //塔编号
        2        public string x;                       //坐标
        3        public string z;
        4        public string player;                 //所属玩家
        5        public string buildPoint;             //生产力
        6        public string type;                   //生产任务类型
        7        public string buildCost;              //任务的生产力消耗（还需要消耗多少生产力才能完成当前任务）
        8        ublic string healthPoint;          //生命值
        9        public string battlePoint;         //战斗力
        10        public string starLevel;          //等级
        11        public string newT;               //是否为新塔，0/1*/
        // process tower
        while(nowl < round_string[r].size() && !is_c(r, nowl, 'm')) {
            int tower_id, x, y, player, buildPoint, type, buildCost,
                healthPoint, BattlePoint, starLevel, newT;
            strin = istringstream(L(r, nowl));
            strin >> tower_id >> x >> y;
            strin >> player >> buildPoint >> type >> buildCost >> healthPoint >>
                BattlePoint >> starLevel >> newT;
            if(strin.fail()) {
                round_info[r].tower.push_back({tower_id, x, y});
            }
            else {
                round_info[r].tower.push_back(
                    {tower_id, x, y, player, buildPoint, type, buildCost,
                     healthPoint, BattlePoint, starLevel, newT});
                // cerr << "id:changed" << tower_id << endl;
            }
            nowl++;
        }
        nowl++;
        // #map<下面若干行为地图方格信息>
        // 示例：
        // 7 5 2 3
        // 8 5 1 3
        // 7 6 3 3
        // 8 6 2 3
        // 9 6 2 3
        // <一行4个数，分别与下面的结构体信息对应>
        // 1        public string x;                       //方块坐标
        // 2        public string z;
        // 3        public string type;                    //方格用地类型（0 塔|1
        // 平原|2 山地|3 森林|4 沼泽|5 道路） 4        public string owner;
        // //拥有者序号(暂定-1 代表“过渡区域”，0代表“公共区域”)
        // <当且仅当地图方格的地形或所属玩家属性变化时，才会被记录，如果一回合内所有地图方格这两个属性均维持原状，那么#map数据为空，#map后紧接着#players一行>
        while(nowl < round_string[r].size() && !is_c(r, nowl, 'p')) {
            strin = istringstream(L(r, nowl));
            int x, y, type, owner;
            strin >> x >> y >> type >> owner;
            round_info[r].map.push_back({x, y, type, owner});
            nowl++;
        }
        _g.copyToBackup();
        _g_my.copyToBackup();
        nowl++;
        // 处理 playerinfo
        // #players<下面若干行为所有4位玩家的信息>
        // 示例：
        // 1 player1 2 1 38 1
        // 2 player2 2 1 38 2
        // 3 player3 1 2 34 3
        // 4 player4 2 1 28 4
        // <一行6条数据，分别与下面的结构体信息对应>
        // 1        public string id;           //玩家id
        // 2        public string name;         //玩家名称
        // 3        public string corpsNum;     //拥有兵团个数
        // 4        public string towerNum;     //拥有塔的数量
        // 5        public string score;        //玩家积分
        // 6        public string rank;         //玩家排名
        // <相邻回合记录间有1行空行>

        while(nowl < round_string[r].size() && !is_c(r, nowl, '\0')) {
            strin = istringstream(L(r, nowl));
            int    id;
            string name;
            int    corpsNum, towerNum, score, rank;
            strin >> id >> name >> corpsNum >> towerNum >> score >> rank;
            round_info[r].player.push_back(
                {id, corpsNum, towerNum, score, rank});
            nowl++;
        }
        nowl++;
    }
}

vector<string>         command_string[ROUND_NUMBER];
vector<array<int, 10>> round_command[ROUND_NUMBER];

void splitRoundCommand(istream &fin) {
    string tmp;
    size_t i = 0;
    while(true) {
        getline(fin, tmp, '\n');
        if(tmp.length() == 0) {
            i++;
        }
        else {
            command_string[i].push_back(tmp);
        }
    }
    assert(i == round_cnt);
    // read parameters
    istringstream strin;
    for(size_t r = 0; r < round_cnt; r++) {
        round_command[r].resize(command_string[r].size());
        for(size_t l = 0; l < command_string[r].size(); l++) {
            strin = istringstream(command_string[r][l]);
            for(int i = 0; i < 9; i++) {
                strin >> round_command[r][l][i];
            }
        }
    }
    // get!
}

void initGameMap() {  // use round 0 to get init info!
    size_t r = 0;
    // 最先创建空地图和空用户信息（在game初始化的时候就做好了
    // 给地图上色
    for(vector<int> &m : round_info[r].map) {
        _g.block({m[0], m[1]}) =
            (mapBlock(getTerrainEnum(m[2]), m[3], NOTOWER));
        _g_my.block({m[0], m[1]}) =
            (mapBlock(getTerrainEnum(m[2]), m[3], NOTOWER));
    }
    _g.copyToBackup();
    _g_my.copyToBackup();

    // process tower
    for(vector<int> &t : round_info[r].tower) {
        // length must be very long
        _g.addTower(t[3], {t[1], t[2]});
        _g_my.addTower(t[3], {t[1], t[2]});
    }

    // process player
    for(vector<int> &p : round_info[r].player) {
        PlayerInfo &player = _g.playerInfo[p[0] - 1];
        player.score = p[3], player.rank = p[4];
    }
}

corpsMoveDir getMoveDir(TPoint from, TPoint to) {
    TPoint tmp = to - from;
    int    t   = -1;
    for(int i = 0; i < 4; i++) {
        if(tmp == mp[i]) {
            assert(t == -1);
            t = i;
        }
    }
    assert(t != -1);
    switch(t) {
        case 0: return CUp;
        case 1: return CDown;
        case 2: return CLeft;
        case 3: return CRight;
        default: assert(0);
    }
}

Command getCommand(array<int, 10> &par) {
    /*<文件头无空行>
    round 0<回合数>
    #command
    <下面每行是一条玩家命令，所有命令均为成功执行的命令，按执行的先后顺序被记录在这里>
    示例：
    1 7 -1 4 1 -1 -1 -1 -1
    <一行9个数，依次与下面的结构体信息对应>
    1        public string from_id;           //发起者的id
    2        public string cm_tpye;           //发出命令的类型，包括所有可能的命令类型，兵团操作的cm_type与枚举值相等，塔生产任务和攻击兵团的cm_type分别为10和11
    3        public string getcm_id;          //执行命令的兵团/塔编号
    4        public string aim_x;               //被攻击的兵团/塔的x坐标，移动目标点的x坐标，原地不移动为-1
    5        public string aim_z;               //被攻击的兵团/塔的z坐标，移动目标点的z坐标，原地不移动为-1
    6        public string result;               //被攻击的塔/兵团是否被摧毁/消灭（0双方都存活|1只有攻方存活|2只有对方存活|3双方都死亡）
    7        public string dT;                   //修改地形时的目标地形
    8        public string pT;                   //生产兵团时的兵团种类
    9        public string another_id;      //被攻击的塔、兵团的id，生产出来新塔的id
    <回合与回合间有1行空行间隔>
    <文件尾，即最后一回合所有命令之后，紧接着一行空行>
    */
    assert(par[0] == _g_my.myID);
    // clang-format off
    switch(par[1]) {
        case JMove:
            return Command(
                corpsCommand,
                {par[1], par[2],
                 getMoveDir(_g_my.corpsInfo[par[2]].pos, {par[3], par[4]})});
        case JAttackCorps:
        case JAttackTower:
            return Command(corpsCommand, {par[1], par[2], par[8]});
        case JBuild: 
            return Command(corpsCommand, {par[1], par[2]});
        case JRepair: 
            return Command(corpsCommand, {par[1], par[2]});
        case JChangeTerrain:
            return Command(corpsCommand, {par[1], par[2], par[6]});
        case JProduct: 
            return Command(towerCommand, {par[1], par[2], par[7]});
        case JTowerAttackCorps:
            return Command(towerCommand, {par[1], par[2], par[8]});
        default: assert(0); break;
    }
    // clang-format on
}
// the point is that, this is updated real-time.

// so you cannot process the whole command list, you must process one, then do one.

void getCommandList(Game &g, size_t r) {
    // 考虑第 r 个回合
    // 当前的 _g 和 _g_my 里面都是截至上一个回合的数据，可以作为参考。
    for(size_t i = 0; i < round_command[r].size(); i++) {
        Command tmp = getCommand(round_command[r][i]);
        g.doCommand(_g_my.myID, tmp);
        // 好像还应该干点啥来着
    }
}

void updateFromInfo(Game &g, size_t r){
    Round &info = round_info[r];
    for(vector<int> par : info.map){
        g.block({par[0],par[1]}).type = getTerrainEnum(par[2]);
        g.block({par[0],par[1]}).owner = par[3];
    }
    for(vector<int> par : info.player) {
        
    }
    for(vector<int> par : info.corps){

    }
    info.tower;
}


void parse() {
    // 维护它的主要是是要记录出兵的位置，是要服务于命令list的。
    ifstream fround_info("everyround_info.txt"), fcommand_info("log_info.txt");
    splitRoundInfo(fround_info);
    splitRoundCommand(fcommand_info);
    fround_info.close(), fcommand_info.close();
    initGameMap();
    _g_my.updateInfo();
    _g.updateInfo();
    assertSame(_g, _g_my);
}

void compare() {
    TPlayerID nowPlayer = 0;
    for(size_t r = 0; r < round_cnt; r++) {
        nowPlayer++;
        if(nowPlayer > 4)
            nowPlayer -= 4;
        _g.myID    = nowPlayer;
        _g_my.myID = nowPlayer;
        // 开始模拟！
        // 先处理command！（可能会需要上一回合的game资料）
        // 运用mygame上面
        _g.refresh();  // 全局刷新，不是你的也刷新了
        _g_my.refresh();
        getCommandList(_g_my, r);  // get & do command list
        _g_my.updateProduct(_g_my.myID);
        _g_my.updateInfo();
        // 再根据新的局面更新game
        updateFromInfo(_g, r);
        // 比较！
        assertSame(_g, _g_my);
    }
}

int main() {
    // cerr << round_command[1][2] << endl;
    parse();
    compare();
    return 0;
}
