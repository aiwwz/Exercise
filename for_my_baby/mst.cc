/*********************************************
 * file:   mst.cc
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-07-27 11:52:22
 **********************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
using std::cout;  using std::endl;  using std::cerr;
using std::ifstream;  using std::stringstream;
using std::vector;  using std::string;

//求皮尔逊相关系数
double PearsonCorrelation(vector<double> &A, vector<double> &B) {
    //检查输入是否合法
    if(A.size() != B.size()) {
        cout << "Error input!" << endl;
        exit(-1);
    }

    //求平均值
    double sumA(0), sumB(0), aveA(0), aveB(0);
    sumA = std::accumulate(A.begin(), A.end(), 0);
    sumB = std::accumulate(B.begin(), B.end(), 0);
    aveA = sumA / A.size();
    aveB = sumB / B.size();

    //计算相关系数
    double Cov(0), VarA(0), VarB(0);
    size_t length = A.size();
    for (size_t i = 0; i < length; i++) {
        Cov += (A[i] - aveA) * (B[i] - aveB);
        VarA += pow((A[i] - aveA), 2);
        VarB += pow((B[i] - aveB), 2);
    }

    return (Cov / sqrt(VarA * VarB)); //返回相关系数
}

//求权重
double Weight(vector<double> &A, vector<double> &B) {
    return sqrt(2 * (1 - PearsonCorrelation(A, B)));
}

//顶点: 代表国家
typedef struct {
    string country; //国家名
    size_t id;      //顶点编号
    vector<double> indexs; //若干指标
}Vertex;

//边
typedef struct {
    int u, v; //u -> v
    double weight; //weight = 根号下2(1-Rij); Rij为皮尔逊相关系数
}Edge;

bool cmp(const Edge &lhs, const Edge &rhs) {
    return lhs.weight < rhs.weight;
}

class Graph {
public:
    Graph(ifstream &input) {
        //从文件中读取顶点信息
        string line;
        int id = 0;
        while(std::getline(input, line)) {
            stringstream record(line);

            Vertex vex;
            record >> vex.country;
            if(0 == strlen(vex.country.c_str())) {
                continue;
            }
            vex.id = id++;
            double index;
            while(record >> index) {
                vex.indexs.push_back(index);
            }

            m_vertexs.push_back(vex); //将顶点加入图G中
        }

        //构造边, 共有N(N-1)/2条边
        int size = m_vertexs.size();
        for(int i = 0; i < size; ++i) {
            for(int j = i + 1; j < size; ++j) {
                m_edges.push_back({i, j, Weight(m_vertexs[i].indexs, m_vertexs[j].indexs)});
            }
        }

        //按照边的weight排序
        sort(m_edges.begin(), m_edges.end(), cmp);
    }

    vector<Vertex> m_vertexs; //顶点集
    vector<Edge> m_edges;     //边集
};

//查找顶点v所在的连通分量
int FindRoot(int parent[], int v) {
    int s;
    for(s = v; parent[s] >= 0; s = parent[s]);
    while(s != v) { //便于下次查找
        int tmp = parent[v];
        parent[v] = s;
        v = tmp;
    }
    return s;
}

//生成最小生成树的Kruskal算法
void Kruskal(Graph &G) {
    int vex1, vex2;
    double mstSum = 0;

    int *parent = new int[G.m_vertexs.size()];
    for(size_t i = 0; i < G.m_vertexs.size(); ++i) {
        parent[i] = -1;
    }

    for(size_t num = 0, i = 0; i < G.m_edges.size(); ++i) {
        vex1 = FindRoot(parent, G.m_edges[i].u);
        vex2 = FindRoot(parent, G.m_edges[i].v);
        if(vex1 != vex2) { //若不在同一个连通分量中
            cout << "<" << G.m_vertexs[G.m_edges[i].u].country << "-" << G.m_edges[i].u << " --> " << G.m_vertexs[G.m_edges[i].v].country << "-" << G.m_edges[i].v  << "> : " << G.m_edges[i].weight << endl;
            mstSum += G.m_edges[i].weight;
            parent[vex2] = vex1; //将vex2代表的连通分量加入到vex1代表的连通分量中
            if(++num == G.m_vertexs.size() - 1) { //处理N-1条边即结束, 不做无用功
                cout << "最小生成树权值之和为: " << mstSum << endl;
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if(1 == argc) {
        cout << "Usage: mst data_file" << endl;
        return -1;
    }

    ifstream input(argv[1]);
    if(!input.good()) {
        cerr << "Cann't open file: " << argv[1] << endl;
        return -1;
    }

    //根据数据内容构建图
    Graph G(input);

    //使用kruskal算法构造最小生成树
    Kruskal(G);

    return 0;
}
